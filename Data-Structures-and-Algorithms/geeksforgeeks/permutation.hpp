#include <string>
#include <vector>
#include <iostream>

class permutation
{
public:
	static void permutation_string_by_rotate(std::string const& s);
};

void permute(std::string s, std::string out, std::vector<std::string>& res)
{
    if (s.empty()) {
        res.push_back(out);
        return;
    }
    for (int i = 0; i < s.length(); ++i) {
        permute(s.substr(1), out + s[0], res);
        std::rotate(s.begin(), s.begin() + 1, s.end());
    }
}

void permutation::permutation_string_by_rotate(std::string const& s)
{
    std::vector<std::string> result;
    permute(s, "", result);
    for (auto const& v : result) std::cout << v << ' ';
    std::cout << std::endl;
}