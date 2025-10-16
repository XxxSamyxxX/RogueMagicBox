#include "scanner.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

// minimal nlohmann/json header (vendored simplified include path)
#include "../../third_party/json.hpp"

namespace fs = std::filesystem;

namespace rogue
{

    static bool is_sensitive(const fs::path &p)
    {
        auto name = p.filename().string();
        std::string lower = name;
        for (auto &c : lower)
            c = (char)tolower((unsigned char)c);
        return lower == ".env" || lower.find(".pem") != std::string::npos || lower.find(".key") != std::string::npos || lower.find(".pfx") != std::string::npos || lower.find("token") != std::string::npos;
    }

    ScanResult scan_workspace(const ScanOptions &options, Logger &logger)
    {
        using nlohmann::json;
        json j;
        j["root"] = options.root;
        j["generated_at"] = utils::iso_timestamp();
        j["files"] = json::array();

        // Load .rogueignore
        auto ignore = utils::load_ignore_patterns(fs::path(options.root) / ".rogueignore");

        ScanResult r; // declare early for use in loop
        std::uintmax_t total = 0;
        for (auto &entry : fs::recursive_directory_iterator(options.root))
        {
            if (!entry.is_regular_file())
                continue;
            auto rel = fs::relative(entry.path(), options.root).generic_string();
            if (utils::is_ignored(rel, ignore))
            {
                logger.debug("scan", std::string("ignored ") + rel);
                continue;
            }
            if (!options.includeSecrets && is_sensitive(entry.path()))
            {
                logger.warn("scan", std::string("sensitive skipped: ") + rel);
                continue;
            }
            auto sz = entry.file_size();
            if ((sz / (1024 * 1024)) > (std::uintmax_t)options.maxSizeMb)
            {
                logger.warn("scan", std::string("too large, skipped: ") + rel);
                continue;
            }
            total += sz; // Accumulate total size
            nlohmann::json fj;
            fj["path"] = rel;
            fj["size"] = sz;
            fj["hash"] = utils::sha256_file(entry.path().string());
            fj["mtime"] = utils::file_mtime_iso(entry.path());
            j["files"].push_back(fj);
            // structured
            FileEntry fe;
            fe.path = rel;
            fe.size = sz;
            r.files.push_back(fe);
        }
        j["total_size"] = total;

        r.ok = true;
        r.inventoryJson = j.dump(2);
        r.totalSize = total; // Populate total size in ScanResult
        return r;
    }

}
