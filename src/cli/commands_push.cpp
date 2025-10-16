#include "args.hpp"
#include "../core/gitops.hpp"
#include "../core/logger.hpp"
#include "../core/scanner.hpp"
#include "../core/utils.hpp"
#include <iostream>
#include <ctime>

namespace rogue
{

    int command_push(const CliOptions &opt)
    {
        Logger logger;
        GitOps git(logger);

        std::string msg = opt.commitMessage.value_or("");
        if (msg.empty())
        {
            char buf[32];
            std::time_t t = std::time(nullptr);
            std::tm *tm = std::localtime(&t);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
            msg = std::string("chore(import): add Workshop sources & docs (") + buf + ")";
        }

        if (opt.dryRun)
        {
            // Just compute plan via a scan
            ScanOptions sopt;
            sopt.root = opt.root;
            sopt.maxSizeMb = 50;
            sopt.includeSecrets = opt.includeSecrets;
            auto inv = scan_workspace(sopt, logger);
            std::string mode = (inv.ok && inv.totalSize > (100ull * 1024ull * 1024ull)) ? "chunked (~50MB)" : "single";
            logger.info("push-all", "[dry-run] Would commit and push", {{"message", msg}, {"branch", opt.branch.value_or("main")}, {"mode", mode}});
            return 0;
        }

        // Chunking logic: re-scan and commit in ~50MB chunks if total >100MB
        ScanOptions sopt;
        sopt.root = opt.root;
        sopt.maxSizeMb = 50;
        sopt.includeSecrets = opt.includeSecrets;
        auto inv = scan_workspace(sopt, logger);
        if (inv.ok && inv.totalSize > (100ull * 1024ull * 1024ull))
        {
            std::uintmax_t chunk = 0;
            std::vector<std::string> staged;
            for (auto &f : inv.files)
            {
                if (f.size > 50ull * 1024ull * 1024ull)
                {
                    logger.warn("push-all", std::string("skip >50MB: ") + f.path);
                    continue;
                }
                staged.push_back(f.path);
                chunk += f.size;
                if (chunk >= 50ull * 1024ull * 1024ull)
                {
                    if (!git.stage_all(opt.root))
                    {
                        logger.error("push-all", "Failed to stage files");
                        return 6;
                    }
                    // Commit can fail if nothing to commit - that's OK
                    git.commit(opt.root, msg + " [chunk]");
                    staged.clear();
                    chunk = 0;
                }
            }
            if (!staged.empty())
            {
                if (!git.stage_all(opt.root))
                    return 6;
                // Commit can fail if nothing to commit - that's OK
                git.commit(opt.root, msg + " [chunk]");
            }
        }
        else
        {
            if (!git.stage_all(opt.root))
            {
                logger.error("push-all", "Failed to stage files");
                return 6;
            }
            if (!git.commit(opt.root, msg))
            {
                logger.warn("push-all", "Commit failed (possibly nothing to commit); will attempt push anyway");
                // Don't return error - might be "nothing to commit" which is OK
            }
        }
        // push (always attempt, even if commit was skipped)
        if (!git.push(opt.root, opt.branch.value_or("main")))
        {
            logger.error("push-all", "Failed to push");
            return 8;
        }

        logger.info("push-all", "Pushed successfully");
        return 0;
    }

}
