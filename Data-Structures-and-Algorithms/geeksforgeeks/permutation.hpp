#include <string>
#include <vector>
#include <iostream>

class permutation
{
public:
	static void permutation_string_by_rotate(std::string const& s);
};

void permute(std::string str, std::string out, std::vector<std::string>& result)
{
    // When size of str becomes 0, out has a
    // permutation (length of out is n)
    if (str.size() == 0)
    {
        result.push_back(out);
        return;
    }

    // One be one move all characters at
    // the beginning of out (or result)
    for (int i = 0; i < str.size(); i++)
    {
        // Remove first character from str and
        // add it to out
        permute(str.substr(1), out + str[0], result);

        // Rotate std::string in a way second character
        // moves to the beginning.
        rotate(str.begin(), str.begin() + 1, str.end());
    }
}

void permutation::permutation_string_by_rotate(std::string const& s)
{
    std::vector<std::string> result;
    permute(s, "", result);
    for (auto const& v : result) std::cout << v << ' ';
    std::cout << std::endl;
}