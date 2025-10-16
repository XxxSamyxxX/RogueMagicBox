#include "config.hpp"
#include "logger.hpp"
#include <fstream>
#include <sstream>

namespace rogue
{

    static std::string trim(const std::string &s)
    {
        size_t b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos)
            return "";
        size_t e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    }

    bool load_config(const std::string &path, AppConfig &out, Logger *logger)
    {
        std::ifstream f(path);
        if (!f)
            return false;
        std::string line;
        while (std::getline(f, line))
        {
            auto pos = line.find('=');
            if (pos == std::string::npos)
                continue;
            auto key = trim(line.substr(0, pos));
            auto val = trim(line.substr(pos + 1));
            if (key == "root")
                out.root = val;
            else if (key == "repo_name")
                out.repoName = val;
            else if (key == "private")
                out.isPrivate = (val == "true" || val == "1");
        }
        if (logger)
            logger->info("config", std::string("Loaded ") + path);
        return true;
    }

}
