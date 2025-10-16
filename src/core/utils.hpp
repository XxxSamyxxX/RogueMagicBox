#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <cstdint>
#include <chrono>

namespace rogue
{
    namespace utils
    {

        std::string iso_timestamp();
        std::string file_mtime_iso(const std::filesystem::path &p);

        // Ignore patterns
        std::vector<std::string> load_ignore_patterns(const std::filesystem::path &file);
        bool is_ignored(const std::string &relPath, const std::vector<std::string> &patterns);

        // String helpers
        size_t ifind(const std::string &hay, const std::string &needle);

        // SHA256
        std::string sha256_file(const std::string &filepath);

        // System helpers
        int system_in_dir(const std::string &dir, const std::string &cmd, bool hide_output);

        // Token retrieval
        std::string read_github_token();
        std::string getenv(const std::string &key, const std::string &def = "");

        // Files
        void ensure_file_with_content(const std::filesystem::path &p, const std::string &content);

    }
}
