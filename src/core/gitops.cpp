#include "gitops.hpp"
#include "logger.hpp"
#include "utils.hpp"
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

namespace rogue
{

    bool GitOps::run_git(const std::string &root, const std::vector<std::string> &args, bool hide_output)
    {
        std::ostringstream cmd;
        cmd << "git -C \"" << root << "\"";
        for (auto &a : args)
        {
            cmd << ' ' << a;
        }
        std::string full = cmd.str();
        logger_.info("git", std::string("exec: ") + full);
        int code = system(full.c_str());
        if (code != 0)
            logger_.error("git", "non-zero exit", {{"code", std::to_string(code)}});
        return code == 0;
    }

    bool GitOps::ensure_repo_initialized(const std::string &root)
    {
        if (fs::exists(fs::path(root) / ".git"))
        {
            logger_.info("git", "Repo already initialized");
            return true;
        }
        if (!run_git(root, {"init"}))
            return false;
        // Create basic files
        utils::ensure_file_with_content(fs::path(root) / ".gitignore", "build/\n*.log\nlogs/\n*.exe\n*.obj\n*.pdb\n");
        // .rogueignore with sensitive defaults
        utils::ensure_file_with_content(fs::path(root) / ".rogueignore",
                                        "*.key\n*.pem\n*.pfx\n*.kdbx\n.env\nid_*\n*token*\n*.iso\n*.zip\n*.tar.gz\n*.7z\n*.mp4\n*.mov\n");
        // LICENSE (MIT)
        utils::ensure_file_with_content(fs::path(root) / "LICENSE",
                                        R"(MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
)");
        // README with disclaimer
        utils::ensure_file_with_content(fs::path(root) / "README.md",
                                        "# Workshop Import\n\nCe dépôt a été initialisé automatiquement par RogueMagicBox.\n\n"
                                        "Avertissement: ne commitez pas de données personnelles ni de secrets en clair.\n"
                                        "Fichiers sensibles ignorés par défaut via .rogueignore.\n\n"
                                        "Usage rapide: voir docs/USAGE.md dans RogueMagicBox.\n");
        return true;
    }

    bool GitOps::add_remote_and_fetch(const std::string &root, const std::string &repoName, const std::optional<std::string> &org)
    {
        std::string owner;
        if (org && !org->empty())
            owner = *org;
        if (owner.empty())
        {
            owner = utils::getenv("GITHUB_USER", "");
            if (owner.empty())
            {
                // Use gh CLI to get authenticated user
                std::string tmpfile = (std::filesystem::path(root) / "owner_tmp.txt").string();
                std::string cmd = "gh api user --jq .login > \"" + tmpfile + "\"";
                int code = std::system(cmd.c_str());
                (void)code;
                std::ifstream f(tmpfile);
                if (f)
                {
                    std::getline(f, owner);
                    // Trim whitespace
                    owner.erase(0, owner.find_first_not_of(" \t\n\r"));
                    owner.erase(owner.find_last_not_of(" \t\n\r") + 1);
                }
                std::error_code ec;
                std::filesystem::remove(tmpfile, ec);
            }
        }
        if (owner.empty())
        {
            logger_.error("git", "Cannot determine GitHub owner for remote URL");
            return false;
        }
        std::string full = "https://github.com/" + owner + "/" + repoName + ".git";
        run_git(root, {"remote", "remove", "origin"}, true);
        if (!run_git(root, {"remote", "add", "origin", full}))
            return false;
        // Skip fetch for new empty repos to avoid auth prompts
        // run_git(root, {"fetch", "origin"}, true);
        return true;
    }

    bool GitOps::stage_all(const std::string &root)
    {
        return run_git(root, {"add", "-A"});
    }

    bool GitOps::commit(const std::string &root, const std::string &message)
    {
        // Use double quotes for Windows/cmd safety
        std::string quoted = std::string("\"") + message + "\"";
        return run_git(root, {"commit", "-m", quoted});
    }

    bool GitOps::push(const std::string &root, const std::string &branch)
    {
        return run_git(root, {"push", "-u", "origin", branch});
    }

}
