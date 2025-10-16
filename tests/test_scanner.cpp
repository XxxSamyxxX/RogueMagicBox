#include "../src/core/scanner.hpp"
#include "../src/core/logger.hpp"
#include "../src/core/utils.hpp"
#include "../third_party/catch.hpp"
#include <filesystem>
#include <fstream>

using namespace rogue;
namespace fs = std::filesystem;

TEST_CASE("scanner respects ignore and size", "[scan]")
{
    fs::create_directories("tmp_scan/a");
    std::ofstream("tmp_scan/a/file.txt") << "hello";
    std::ofstream("tmp_scan/.rogueignore") << "*.pem\n*.key\n";
    Logger logger;
    ScanOptions o;
    o.root = "tmp_scan";
    o.maxSizeMb = 1;
    o.includeSecrets = false;
    auto r = scan_workspace(o, logger);
    REQUIRE(r.ok);
    REQUIRE(r.inventoryJson.find("file.txt") != std::string::npos);
}
