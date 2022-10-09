#include <iostream>
#include <vector>
#include <string>
#include "boost/program_options.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/parsers.hpp"
#include "boost/program_options/positional_options.hpp"
#include "boost/program_options/variables_map.hpp"
#include "permutation.h"

using namespace permutation_algorithms;
using namespace std::literals::string_literals;

namespace
{
    bool opt_count = false;
    std::string opt_algorithm;
    std::vector<std::string> opt_elements;
}

void process_cmdline(int argc, char *argv[])
{
    using namespace boost::program_options;

    options_description opts("options");
    opts.add_options()
        ("count,c", "Print the number of permutations only.")
        ("algorithm,a", value<std::string>()->default_value("std"s), "Permutation algorithm. Possible values are 1, 2, 3, 4 or std.")
        ("elements", value<std::vector<std::string>>()->required(), "Elements to permute.")
        ("help,H", "Print this help.")
    ;
    positional_options_description args;
    args.add("elements", -1);
    variables_map vm;
    store(command_line_parser(argc, argv).options(opts).positional(args).run(), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << opts << std::endl;
        throw opts;
    }

    opt_count = vm.count("count");
    opt_algorithm = vm["algorithm"].as<std::string>();
    opt_elements = vm["elements"].as<std::vector<std::string>>();
}

template <std::random_access_iterator TIter>
void output_each_perm(const TIter first, const TIter last, const std::any &user_data)
{
    auto pCount = std::any_cast<int64_t *>(user_data);
    if (pCount) [[likely]] (*pCount)++;
    if (!opt_count)
    {
        for (auto it = first; it != last; ++it) std::cout << *it << " ";
        std::cout << "\n";
    }
}

void run_perm()
{
    std::vector<elem_type> elems;
    std::copy(std::cbegin(opt_elements), std::cend(opt_elements), std::back_inserter(elems));
    int64_t count = 0;

    if (opt_algorithm == "std")
    {
        using namespace permutation_std;
        perm_all(std::cbegin(elems), std::cend(elems), output_each_perm<perm_iterator_type>, &count);
    }
    else if (opt_algorithm == "1")
    {
        using namespace permutation1;
        perm_all(std::cbegin(elems), std::cend(elems), output_each_perm<perm_iterator_type>, &count);
    }
    else if (opt_algorithm == "2")
    {
        using namespace permutation2;
        perm_all(std::cbegin(elems), std::cend(elems), output_each_perm<perm_iterator_type>, &count);
    }
    else if (opt_algorithm == "3")
    {
        using namespace permutation3;
        perm_all(std::cbegin(elems), std::cend(elems), output_each_perm<perm_iterator_type>, &count);
    }
    else if (opt_algorithm == "4")
    {
        using namespace permutation4;
        perm_all(std::cbegin(elems), std::cend(elems), output_each_perm<perm_iterator_type>, &count);
    }
    else
    {
        throw std::domain_error("unknown algorithm "s + opt_algorithm);
    }

    if (opt_count) std::cout << count << "\n";
}

int main(int argc, char**argv)
try
{
    std::ios::sync_with_stdio(false);
    using namespace permutation_std;
    process_cmdline(argc, argv);
    run_perm();

    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
catch (...)
{
    return 1;
}
