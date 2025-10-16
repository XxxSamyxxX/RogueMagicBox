#include "args.hpp"
#include "../core/gitops.hpp"
#include "../core/github_api.hpp"
#include "../core/logger.hpp"
#include "../core/utils.hpp"
#include <iostream>

namespace rogue
{

    int command_init(const CliOptions &opt)
    {
        Logger logger;
        GitOps git(logger);
        if (opt.dryRun)
        {
            logger.info("init-repo", "[dry-run] Would initialize git repo and create standard files (.gitignore, .rogueignore, LICENSE, README)");
            if (!opt.noRemote)
            {
                std::string remote = opt.org && !opt.org->empty() ? ("https://github.com/" + *opt.org + "/" + opt.repoName + ".git") : ("https://github.com/" + opt.repoName + ".git");
                logger.info("init-repo", "[dry-run] Would create and set GitHub remote", {{"url", remote}, {"private", opt.makePrivate ? "true" : "false"}});
            }
            return 0;
        }
        if (!git.ensure_repo_initialized(opt.root))
        {
            logger.error("init-repo", "Failed to initialize repo");
            return 3;
        }
        if (opt.noRemote)
        {
            logger.info("init-repo", "Skipping remote creation");
            return 0;
        }

#ifdef HAVE_LIBCURL
        GitHubApi gh(logger);
        auto token = utils::read_github_token();
        if (token.empty())
        {
            logger.warn("init-repo", "No GitHub token found. Falling back to gh CLI if available.");
            if (!gh.create_repo_via_gh(opt.root, opt.repoName, opt.org.value_or(""), opt.makePrivate))
            {
                logger.error("init-repo", "Failed to create repo via gh CLI");
                return 4;
            }
        }
        else
        {
            if (!gh.create_repo_via_api(opt.repoName, opt.org, opt.makePrivate, token))
            {
                logger.error("init-repo", "Failed to create repo via GitHub API");
                return 4;
            }
        }
#else
        GitHubApi gh(logger);
        if (!gh.create_repo_via_gh(opt.root, opt.repoName, opt.org.value_or(""), opt.makePrivate))
        {
            logger.error("init-repo", "Failed to create repo via gh CLI (libcurl not found)");
            return 4;
        }
#endif

        if (!git.add_remote_and_fetch(opt.root, opt.repoName, opt.org))
        {
            logger.error("init-repo", "Failed to add remote");
            return 5;
        }

        logger.info("init-repo", "Repository initialized and remote configured");
        return 0;
    }

}
