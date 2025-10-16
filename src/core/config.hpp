#pragma once
#include <string>
#include <optional>

namespace rogue
{

    struct AppConfig
    {
        std::string root;
        std::string repoName;
        bool isPrivate{true};
    };

    class Logger;

    bool load_config(const std::string &path, AppConfig &out, Logger *logger);

}
