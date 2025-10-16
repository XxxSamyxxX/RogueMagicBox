#pragma once
#include <string>
#include <vector>
#include <optional>

namespace rogue
{

    struct CliOptions
    {
        std::string command;
        std::string root;
        std::vector<std::string> includes;
        std::vector<std::string> excludes;
        std::optional<int> maxSizeMb;
        bool dryRun{false};
        std::string repoName;
        std::optional<std::string> org;
        bool makePrivate{true};
        bool noRemote{false};
        std::optional<std::string> branch;
        std::optional<std::string> commitMessage;
        std::optional<std::string> configFile;
        bool includeSecrets{false};
    };

    CliOptions parse_args(int argc, char **argv);

}
