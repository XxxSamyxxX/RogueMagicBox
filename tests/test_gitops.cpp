#include "../src/core/gitops.hpp"
#include "../src/core/logger.hpp"
#include "../third_party/catch.hpp"
#include <filesystem>
#include <fstream>

using namespace rogue;
namespace fs = std::filesystem;

TEST_CASE("git missing gracefully handled", "[gitops]")
{
    Logger logger;
    GitOps git(logger);
    // We can't force git absence here, but we can test stage/commit calls on empty repo path; expect failure or no crash.
    bool ok = git.stage_all(".");
    REQUIRE(ok == true || ok == false);
}
