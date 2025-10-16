#pragma once
#include <string>
#include <optional>
#include <filesystem>

namespace rogue
{

    class Logger;

    class GitHubApi
    {
    public:
        explicit GitHubApi(Logger &logger) : logger_(logger) {}
        bool create_repo_via_api(const std::string &repoName, const std::optional<std::string> &org, bool isPrivate, const std::string &token);
        // Create repo using GitHub CLI in the provided working directory (used as --source)
        bool create_repo_via_gh(const std::filesystem::path &workdir, const std::string &repoName, const std::string &org, bool isPrivate);

    private:
        Logger &logger_;
    };

}
