#include "args.hpp"
#include "../core/scanner.hpp"
#include "../core/logger.hpp"
#include "../core/config.hpp"
#include <iostream>

namespace rogue
{

    int command_scan(const CliOptions &opt)
    {
        Logger logger;
        logger.info("scan", "Starting scan", {{"root", opt.root}});
        ScanOptions sopt;
        sopt.root = opt.root;
        sopt.includes = opt.includes;
        sopt.excludes = opt.excludes;
        sopt.maxSizeMb = opt.maxSizeMb.value_or(50);
        sopt.includeSecrets = opt.includeSecrets;
        auto result = scan_workspace(sopt, logger);
        if (!result.ok)
        {
            logger.error("scan", result.errorMessage);
            return 2;
        }
        std::cout << result.inventoryJson << std::endl;
        logger.info("scan", "Completed");
        return 0;
    }

}
