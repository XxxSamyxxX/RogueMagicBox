#pragma once
#include <string>
#include <vector>
#include <map>

namespace rogue
{

    class Logger
    {
    public:
        Logger();
        ~Logger();
        void info(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv = {});
        void warn(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv = {});
        void error(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv = {});
        void debug(const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv = {});

    private:
        void log(const std::string &level, const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv);
        void open();
        void close();
        void ensure_log_dir();
        void write_line(const std::string &line);
        void rotate_if_needed();
        void mask_secrets_inplace(std::string &line);
        void *file_{nullptr};
    };

}
