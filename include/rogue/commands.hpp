#pragma once
#include "../../src/cli/args.hpp"

namespace rogue
{
    int command_scan(const CliOptions &opt);
    int command_init(const CliOptions &opt);
    int command_push(const CliOptions &opt);
}
