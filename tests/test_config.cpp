#include "../src/core/config.hpp"
#include "../src/core/logger.hpp"
#include "../third_party/catch.hpp"
#include <fstream>

using namespace rogue;

TEST_CASE("config parser loads keys", "[config]")
{
    std::ofstream("tmp_config.toml") << "root=tmp\nrepo_name=demo\nprivate=true\n";
    AppConfig cfg;
    Logger log;
    bool ok = load_config("tmp_config.toml", cfg, &log);
    REQUIRE(ok);
    REQUIRE(cfg.root == "tmp");
    REQUIRE(cfg.repoName == "demo");
    REQUIRE(cfg.isPrivate == true);
}
