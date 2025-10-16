#include "cli/args.hpp"
#include "core/logger.hpp"
#include "core/scanner.hpp"
#include "core/gitops.hpp"
#include "core/github_api.hpp"
#include "core/config.hpp"
#include "core/utils.hpp"
#include "rogue/commands.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace rogue;

static void print_help()
{
    std::cout << "roguebox CLI\n"
              << "Commands:\n"
              << "  scan --root <path> [--include <glob> ...] [--exclude <glob> ...] [--max-size-mb <int>] [--dry-run]\n"
              << "  init-repo --root <path> --repo-name <name> [--org <org>] [--private|--public] [--no-remote]\n"
              << "  push-all --root <path> [--branch <name>] [--commit-message \"<msg>\"] [--dry-run]\n"
              << "  full-run --root <path> --repo-name <name> [options...]\n"
              << std::endl;
}

namespace rogue
{

    static bool arg_flag(char *a, const char *name) { return std::string(a) == name; }

    CliOptions parse_args(int argc, char **argv)
    {
        CliOptions o;
        if (argc < 2)
            return o;
        o.command = argv[1];
        for (int i = 2; i < argc; ++i)
        {
            std::string k = argv[i];
            auto next = [&](std::string &out)
            { if (i+1<argc) { out = argv[++i]; return true; } return false; };
            if (k == "--root")
                next(o.root);
            else if (k == "--include")
            {
                std::string v;
                if (next(v))
                    o.includes.push_back(v);
            }
            else if (k == "--exclude")
            {
                std::string v;
                if (next(v))
                    o.excludes.push_back(v);
            }
            else if (k == "--max-size-mb")
            {
                std::string v;
                if (next(v))
                    o.maxSizeMb = std::stoi(v);
            }
            else if (k == "--dry-run")
                o.dryRun = true;
            else if (k == "--repo-name")
                next(o.repoName);
            else if (k == "--org")
            {
                std::string v;
                if (next(v))
                    o.org = v;
            }
            else if (k == "--private")
                o.makePrivate = true;
            else if (k == "--public")
                o.makePrivate = false;
            else if (k == "--no-remote")
                o.noRemote = true;
            else if (k == "--branch")
            {
                std::string v;
                if (next(v))
                    o.branch = v;
            }
            else if (k == "--commit-message")
            {
                std::string v;
                if (next(v))
                    o.commitMessage = v;
            }
            else if (k == "--config")
            {
                std::string v;
                if (next(v))
                    o.configFile = v;
            }
            else if (k == "--include-secrets")
                o.includeSecrets = true;
        }
        return o;
    }

}

int main(int argc, char **argv)
{
    auto opt = parse_args(argc, argv);
    Logger logger;
    if (opt.command.empty())
    {
        print_help();
        return 1;
    }

    if (opt.configFile)
    {
        AppConfig cfg;
        if (load_config(*opt.configFile, cfg, &logger))
        {
            if (opt.root.empty())
                opt.root = cfg.root;
            if (opt.repoName.empty())
                opt.repoName = cfg.repoName;
            if (!cfg.isPrivate)
                opt.makePrivate = false;
        }
    }

    if (opt.command == "scan")
    {
        return rogue::command_scan(opt);
    }
    else if (opt.command == "init-repo")
    {
        return rogue::command_init(opt);
    }
    else if (opt.command == "push-all")
    {
        return rogue::command_push(opt);
    }
    else if (opt.command == "full-run")
    {
        // scan
        ScanOptions sopt{opt.root, opt.includes, opt.excludes, opt.maxSizeMb.value_or(50), opt.includeSecrets};
        auto result = scan_workspace(sopt, logger);
        if (!result.ok)
            return 2;
        // init
        int ec = rogue::command_init(opt);
        if (ec != 0)
            return ec;
        // push
        int pc = rogue::command_push(opt);
        // proof of work append
        std::filesystem::create_directories("docs");
        std::ofstream pf("docs/PROOF_OF_WORK.md", std::ios::app);
        if (pf)
        {
            pf << "# PROOF OF WORK\n\nGenerated at: " << rogue::utils::iso_timestamp() << "\n\n";
            pf << "## Inventory\n\n````json\n"
               << result.inventoryJson << "\n````\n";
        }
        return pc;
    }
    else
    {
        print_help();
        return 1;
    }
}
