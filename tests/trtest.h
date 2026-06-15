/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *                                                                               *
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Test framework                                                     *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Minimal host-side unit test framework for Trunk.                   *
 ********************************************************************************/
#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace trtest
{
    struct SuiteState
    {
        const char *suite_name = nullptr;
        int passed = 0;
        int failed = 0;
        int total = 0;
    };

    struct GlobalState
    {
        int total_passed = 0;
        int total_failed = 0;
        int total_suites = 0;
    };

    inline GlobalState g_state{};
    inline SuiteState g_suite{};

    static constexpr const char *COL_GREEN = "\033[0;32m";
    static constexpr const char *COL_RED = "\033[0;31m";
    static constexpr const char *COL_CYAN = "\033[0;36m";
    static constexpr const char *COL_YELLOW = "\033[0;33m";
    static constexpr const char *COL_BOLD = "\033[1m";
    static constexpr const char *COL_RESET = "\033[0m";

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : suite_begin                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Begin a named test suite. Resets suite pass/fail counters.         *
     ********************************************************************************/
    inline void suite_begin(const char *name) noexcept
    {
        g_suite = {};
        g_suite.suite_name = name;
        printf("\n  %s%s[ Suite: %s ]%s\n",
               COL_BOLD, COL_CYAN, name, COL_RESET);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : suite_end                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : End the current suite and print its summary.                       *
     ********************************************************************************/
    inline void suite_end() noexcept
    {
        const bool all_passed = g_suite.failed == 0;
        printf("  %s[ %s ]%s  %s — %d/%d passed\n",
               all_passed ? COL_GREEN : COL_RED,
               all_passed ? "OK" : "FAIL",
               COL_RESET,
               g_suite.suite_name,
               g_suite.passed,
               g_suite.total);

        g_state.total_passed += g_suite.passed;
        g_state.total_failed += g_suite.failed;
        g_state.total_suites++;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : record                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Record a single assertion result. Called by all TEST_* macros.     *
     ********************************************************************************/
    inline void record(bool passed, const char *expr,
                       const char *file, int line) noexcept
    {
        g_suite.total++;
        if (passed)
        {
            g_suite.passed++;
            printf("  %s[ PASS ]%s  %s\n", COL_GREEN, COL_RESET, expr);
        }
        else
        {
            g_suite.failed++;
            printf("  %s[ FAIL ]%s  %s\n", COL_RED, COL_RESET, expr);
            printf("           %s%s:%d%s\n", COL_YELLOW, file, line, COL_RESET);
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : summary                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Print global summary across all suites. Returns exit code.         *
     ********************************************************************************/
    inline int summary() noexcept
    {
        const bool all_passed = g_state.total_failed == 0;
        printf("\n  %s%s========================================%s\n",
               COL_BOLD, COL_CYAN, COL_RESET);
        printf("  %sTrunk Test Results%s\n", COL_BOLD, COL_RESET);
        printf("  Suites : %d\n", g_state.total_suites);
        printf("  Passed : %s%d%s\n", COL_GREEN, g_state.total_passed, COL_RESET);
        printf("  Failed : %s%d%s\n",
               g_state.total_failed > 0 ? COL_RED : COL_GREEN,
               g_state.total_failed, COL_RESET);
        printf("  %s%s========================================%s\n\n",
               COL_BOLD, COL_CYAN, COL_RESET);
        return all_passed ? 0 : 1;
    }

} // namespace trtest

#define TEST_ASSERT(expr) \
    trtest::record(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

#define TEST_ASSERT_EQ(a, b) \
    trtest::record((a) == (b), #a " == " #b, __FILE__, __LINE__)

#define TEST_ASSERT_NEQ(a, b) \
    trtest::record((a) != (b), #a " != " #b, __FILE__, __LINE__)

#define TEST_ASSERT_GT(a, b) \
    trtest::record((a) > (b), #a " > " #b, __FILE__, __LINE__)

#define TEST_ASSERT_LT(a, b) \
    trtest::record((a) < (b), #a " < " #b, __FILE__, __LINE__)

#define TEST_ASSERT_GE(a, b) \
    trtest::record((a) >= (b), #a " >= " #b, __FILE__, __LINE__)

#define TEST_ASSERT_LE(a, b) \
    trtest::record((a) <= (b), #a " <= " #b, __FILE__, __LINE__)

#define TEST_ASSERT_STR_EQ(a, b) \
    trtest::record(strcmp((a), (b)) == 0, #a " str== " #b, __FILE__, __LINE__)

#define SUITE_BEGIN(name) trtest::suite_begin(name)
#define SUITE_END() trtest::suite_end()