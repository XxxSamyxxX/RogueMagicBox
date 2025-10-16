#pragma once
#include <string>
#include <optional>
#include <vector>

namespace rogue
{

    class Logger;

    class GitOps
    {
    public:
        explicit GitOps(Logger &logger) : logger_(logger) {}
        bool ensure_repo_initialized(const std::string &root);
        bool add_remote_and_fetch(const std::string &root, const std::string &repoName, const std::optional<std::string> &org);
        bool stage_all(const std::string &root);
        bool commit(const std::string &root, const std::string &message);
        bool push(const std::string &root, const std::string &branch);

    private:
        Logger &logger_;
        bool run_git(const std::string &root, const std::vector<std::string> &args, bool hide_output = false);
    };

}
