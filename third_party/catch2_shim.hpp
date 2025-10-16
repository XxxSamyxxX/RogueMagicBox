// Minimal shim if Catch2 isn't installed; provide trivial test macros
#pragma once
// Minimal Catch2-like shim to run basic tests without external dependency.
#include <iostream>
#include <vector>
#include <cstdlib>

namespace catch_shim
{
    using TestFn = void (*)();
    inline std::vector<TestFn> &registry()
    {
        static std::vector<TestFn> r;
        return r;
    }
    inline int run_all()
    {
        int fails = 0;
        for (auto fn : registry())
        {
            try
            {
                fn();
            }
            catch (...)
            {
                ++fails;
            }
        }
        return fails;
    }
}

#define CATCH_CONCAT_IMPL(a, b) a##b
#define CATCH_CONCAT(a, b) CATCH_CONCAT_IMPL(a, b)
#define CATCH_TEST_IMPL(id)                                                                                      \
    static void CATCH_CONCAT(catch_test_, id)();                                                                 \
    namespace                                                                                                    \
    {                                                                                                            \
        struct CATCH_CONCAT(catch_reg_, id)                                                                      \
        {                                                                                                        \
            CATCH_CONCAT(catch_reg_, id)() { catch_shim::registry().push_back(&CATCH_CONCAT(catch_test_, id)); } \
        } CATCH_CONCAT(catch_reg_inst_, id);                                                                     \
    }                                                                                                            \
    static void CATCH_CONCAT(catch_test_, id)()

#define TEST_CASE(name, tags) CATCH_TEST_IMPL(__COUNTER__)

#define REQUIRE(expr)                                           \
    do                                                          \
    {                                                           \
        if (!(expr))                                            \
        {                                                       \
            std::cerr << "REQUIRE failed: " #expr << std::endl; \
            std::exit(1);                                       \
        }                                                       \
    } while (0)
#define TEST_CASE(name, tags) CATCH_TEST_IMPL(__COUNTER__)

#define REQUIRE(expr)                                           \
    do                                                          \
    {                                                           \
        if (!(expr))                                            \
        {                                                       \
            std::cerr << "REQUIRE failed: " #expr << std::endl; \
            std::exit(1);                                       \
        }                                                       \
    } while (0)

#ifdef CATCH_SHIM_MAIN
int main()
{
    int fails = catch_shim::run_all();
    return fails == 0 ? 0 : 1;
}
#endif
