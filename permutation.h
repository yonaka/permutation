#include <stdexcept>
#include <vector>
#include <algorithm>
#include <utility>
#include <iterator>
#include <string>
#include <string_view>
#include <concepts>
#include <functional>
#include <any>
#include <limits>
#include <cassert>

namespace permutation_algorithms
{
    using elem_type = std::string_view;
    using perm_type = std::vector<elem_type>;
    using perm_iterator_type = typename perm_type::const_iterator;
    using output_each_perm_function_type = std::function<void(const perm_iterator_type, const perm_iterator_type, const std::any &)>;

    namespace permutation_std
    {
        // Permutation generation using a C++ standard library function next_permutation()
        // for reference and testing.
        // Possible implementation use a traditional Algorithm L (Lexicographic permutation generation)
        // written in Knuth, D. The Art of Computer Programming Vol. 4A Combinatorial Algorithms Pt.1
        // 7.2.1.2. Generating all permutations.
        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as the parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm_all(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            perm_type a{first, last}; // a simple concatenation makes a permutation
            std::sort(std::begin(a), std::end(a)); // a first permutation
            do {
                output_each_perm(std::cbegin(a), std::cend(a), user_data);
            } while (std::next_permutation(std::begin(a), std::end(a)));
            
        }
    }

    namespace permutation1
    {
        // Permutation generation of n elemnts, by inserting a new element
        // into all positions of permutations of n-1 elements.
        // Knuth, D. The Art of Computer Programming Vol.1 Fundamental Algorithms 3rd Ed.
        // 1.2.5. Permutations and Factorials Method 1
        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            const auto d = std::distance(first, last);
            assert(d >= 0);
            if (d == 0) return;
            if (d == 1)
            {
                output_each_perm(first, last, user_data);
                return;
            }

            const auto &new_elem = *std::prev(last);
            // Generate n-1 elements using recursion.
            perm(first, std::prev(last),
                [&](const perm_iterator_type pm_first, const perm_iterator_type pm_last, const std::any &pm_user_data) {
                    // [pm_first:pm_last) is a permutation of n-1 elements
                    perm_type cur_perm;
                    for (size_t i = 0; i < d; ++i)
                    {
                        // Make a copy with a new element.
                        cur_perm.clear();
                        cur_perm.reserve(std::distance(pm_first, pm_last) + 1);
                        const auto insertion_point = std::next(pm_first, i);
                        std::copy(pm_first, insertion_point, std::back_inserter(cur_perm));
                        cur_perm.emplace_back(new_elem);
                        std::copy(insertion_point, pm_last, std::back_inserter(cur_perm));

                        output_each_perm(std::cbegin(cur_perm), std::cend(cur_perm), pm_user_data);
                    }
                },
                user_data
            );
        }
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm_all(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            perm_type c{first, last}; // copy
            perm(std::begin(c), std::end(c), output_each_perm, user_data);
        }
    }

    namespace permutation2
    {
        // Permutation generation.
        // Knuth, D. The Art of Computer Programming Vol. 4A Combinatorial Algorithms Pt.1
        // 7.2.1.2 Generating all permutations. Algorithm P (Plain change)
        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm_all(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            const auto sz = std::distance(first, last);
            if (sz > std::numeric_limits<int>::max()) throw std::domain_error("too many elements");
            if (sz == 0) return;

            perm_type a{first, last};
            std::vector<int> c(sz, 0);
            std::vector<signed char> o(sz, 1);
            for (;;)
            {
                output_each_perm(std::cbegin(a), std::cend(a), user_data);
                for (int s = 0, j = sz - 1, q; ; --j)
                {
                    q = c[j] + o[j];
                    if (q >= 0)
                    {
                        if (q != j + 1)
                        {
                            using namespace std;
                            swap(a[j - c[j] + s], a[j - q + s]);
                            c[j] = q;
                            break;
                        }
                        if (j == 0) return;
                        ++s;
                    }
                    o[j] = -o[j];
                }
            }
            /* NOTREACHED */
        }
    }

    namespace permutation3
    {
        // Permutation generation.
        // Heap, B. R. (1963) "Permutations by Interchanges". The Computer Journal 6(3): 293-4.
        // Recursive version.
        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm(const int k, const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            if (k == 1)
            {
                output_each_perm(first, last, user_data);
                return;
            }

            perm<TIter>(k - 1, first, last, output_each_perm, user_data);
            for (int i = 0; i < k - 1; ++i)
            {
                using namespace std;
                if ((k & 1) == 0) swap(first[i], first[k-1]);
                else swap(first[0], first[k-1]);
                perm<TIter>(k - 1, first, last, output_each_perm, user_data);
            }
        }

        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm_all(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            perm_type c{first, last};
            const auto f = std::begin(c), l = std::end(c);
            perm(std::distance(f, l), f, l, output_each_perm, user_data);
        }
    }

    namespace permutation4
    {
        // Permutation generation.
        // Heap, B. R. (1963) "Permutations by Interchanges". The Computer Journal 6(3): 293-4.
        // Non-recursive version.
        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm(const int n, const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            std::vector<int> c(n, 0);
            output_each_perm(first, last, user_data);
            for (int i = 1; i < n;)
            {
                if (c[i] < i)
                {
                    using namespace std;
                    if ((i&1) == 0) swap(first[0], first[i]);
                    else swap(first[c[i]], first[i]);
                    output_each_perm(first, last, user_data);
                    ++c[i];
                    i = 1;
                }
                else
                {
                    c[i] = 0;
                    ++i;
                }
            }
        }

        // [first:last): Elements to permute.
        // output_each_perm: output function of which a permutation should be passed as parameters.
        template <std::random_access_iterator TIter>
            requires std::convertible_to<typename std::iterator_traits<TIter>::value_type, elem_type>
        inline void perm_all(const TIter first, const TIter last, output_each_perm_function_type output_each_perm, const std::any &user_data)
        {
            perm_type c{first, last};
            const auto f = std::begin(c), l = std::end(c);
            perm(std::distance(f, l), f, l, output_each_perm, user_data);
        }
    }

    template <typename TCont> concept SimpleContainer = requires(TCont cont)
    {
        std::cbegin(cont);
        std::cend(cont);
    };

    template <SimpleContainer TOutputCont, typename TPermAllFunc, std::random_access_iterator TIter>
        requires std::convertible_to<
            typename std::iterator_traits<TIter>::value_type,
            typename TOutputCont::value_type::value_type>
    TOutputCont perm_all_container(TPermAllFunc perm_all, const TIter first, const TIter last)
    {
        TOutputCont r;
        perm_all(first, last,
            [&](const auto f, const auto l, const std::any &) {
                r.emplace_back(f, l);
            },
            {}
        );
        return r;
    }
}