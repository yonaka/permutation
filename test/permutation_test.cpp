#include <algorithm>
#include <cstdint>
#include <vector>
#include <string_view>
#include <algorithm>
#include <gtest/gtest.h>
#include "../permutation.h"

using namespace permutation_algorithms;
using namespace std::literals::string_view_literals;

namespace 
{
    using test_elems_type = std::vector<std::string_view>;
    const test_elems_type test_elems{"5"sv, "1"sv, "2"sv, "3"sv, "4"sv};

    // Return n! (n>=0)
    inline int64_t factor(unsigned int n)
    {
        if (n == 0) return 1;
        int64_t r = n;
        while (--n > 1) r *= n;
        return r;
    }

    template <SimpleContainer TCont>
    inline void check_perm(TCont perm_cont, const int elem_size)
    {
        // The number of permutations is n!
        const auto perm_num = std::size(perm_cont);
        EXPECT_EQ(perm_num, factor(elem_size));

        // All permutations are unique
        std::sort(std::begin(perm_cont), std::end(perm_cont));
        auto it = std::unique(std::begin(perm_cont), std::end(perm_cont));
        EXPECT_EQ(std::distance(std::begin(perm_cont), it), perm_num);
    }

    template <std::random_access_iterator TElemIter>
    inline auto get_permutation_std(const TElemIter first, TElemIter last)
    {
        using namespace permutation_std;
        using cont_type = std::vector<perm_type>;
        static cont_type r = perm_all_container<cont_type>(perm_all<TElemIter>, first, last);
        return r;
    }
}

TEST(permutation_test, permutation_std_test)
{
    using namespace permutation_std;
    const auto r = get_permutation_std(std::cbegin(test_elems), std::end(test_elems));
    check_perm(r, std::size(test_elems));
}

#define PERMUTATION_TEST(name_space) do {   \
    using namespace name_space;   \
    const auto expected = get_permutation_std(std::cbegin(test_elems), std::cend(test_elems));  \
    auto actual = perm_all_container<std::vector<perm_type>>(   \
        perm_all<decltype(std::cbegin(test_elems))>, std::cbegin(test_elems), std::cend(test_elems));   \
    check_perm(actual, std::size(test_elems));  \
    std::sort(std::begin(actual), std::end(actual));    \
    EXPECT_EQ(expected, actual);    \
} while (false)

TEST(permutation_test, permutation1_test)
{
    PERMUTATION_TEST(permutation1);
}
TEST(permutation_test, permutation2_test)
{
    PERMUTATION_TEST(permutation2);
}
TEST(permutation_test, permutation3_test)
{
    PERMUTATION_TEST(permutation3);
}
TEST(permutation_test, permutation4_test)
{
    PERMUTATION_TEST(permutation4);
}
