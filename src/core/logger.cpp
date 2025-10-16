#include "logger.hpp"
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include "../../third_party/json.hpp"

namespace fs = std::filesystem;

namespace rogue
{

    Logger::Logger() { open(); }
    Logger::~Logger() { close(); }

    void Logger::open() { ensure_log_dir(); }
    void Logger::close() {}

    void Logger::ensure_log_dir()
    {
        fs::create_directories("logs");
    }

    void Logger::mask_secrets_inplace(std::string &line)
    {
        // Very basic masking for tokens and keys
        const char *patterns[] = {"ghp_", "github_pat_", "token", "apikey", "secret", nullptr};
        for (int i = 0; patterns[i]; ++i)
        {
            auto pos = utils::ifind(line, patterns[i]);
            if (pos != std::string::npos)
            {
                for (size_t j = pos + 4; j < line.size() && j < pos + 32; ++j)
                    line[j] = '*';
            }
        }
    }

    void Logger::write_line(const std::string &line)
    {
        std::ofstream f("logs/rogue.log", std::ios::app);
        if (f)
        {
            std::string masked = line;
            mask_secrets_inplace(masked);
            f << masked << "\n";
        }
    }

    void Logger::rotate_if_needed()
    {
        // simple no-op for now
    }

    void Logger::log(const std::string &level, const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv)
    {
        nlohmann::json j;
        j["ts"] = utils::iso_timestamp();
        j["level"] = level;
        j["ctx"] = ctx;
        j["msg"] = msg;
        for (auto &p : kv)
            j[p.first] = p.second;
        auto line = j.dump();
        // console readable
        std::cout << '[' << level << "] " << ctx << ": " << msg << std::endl;
        write_line(line);
    }

    void Logger::info(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv) { log("info", ctx, msg, kv); }
    void Logger::warn(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv) { log("warn", ctx, msg, kv); }
    void Logger::error(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv) { log("error", ctx, msg, kv); }
    void Logger::debug(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv) { log("debug", ctx, msg, kv); }

}
