#include <vector>
#include <string>

using namespace std;
class BackTracking 
{
public:
    static vector<string> solveMaze()
    {
        vector<vector<int>> m
        { {1, 0, 0, 0},
         {1, 1, 0, 1},
         {1, 1, 0, 0},
         {0, 1, 1, 1} };

        return BackTracking{}.findPath(m, 4);
    }

private:
    vector<string> findPath(vector<vector<int>>& m, int n) {
        vector<string> res;
        vector<vector<int>> visited(n, vector<int>(n, 0));
        findPathUtil(m, n, 0, 0, visited, res, "");
        print(visited, n);
        for (auto& r : res) cout << r << ' ';
        std::cout << endl;

        return res;
    }

    bool findPathUtil(auto& m, int n, int i, int j, auto& visited, auto& res, string dir)
    {
        if (i == n - 1 && j == n - 1) {
            res.push_back(dir);
            visited[i][j] = 1;
            return true;
        }

        if (!isValid(m, n, i, j)) return false;

        if (visited[i][j] == 1) return false;

        visited[i][j] = 1;

        // go up
        if (findPathUtil(m, n, i - 1, j, visited, res, dir + "U")) {
            //return true;
        }

        // go left
        if (findPathUtil(m, n, i, j - 1, visited, res, dir + "L")) {
            //return true;
        }

        // go right
        if (findPathUtil(m, n, i, j + 1, visited, res, dir + "R")) {
            //return true;
        }

        // go down
        if (findPathUtil(m, n, i + 1, j, visited, res, dir + "D")) {
            //return true;
        }

        visited[i][j] = 0;
        return false;
    }

    bool isValid(vector<vector<int>>& m, int n, int i, int j)
    {
        return i >= 0 && i < n
            && j >= 0 && j < n
            && m[i][j] == 1;
    }
    void print(auto& m, int n)
    {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                std::cout << m[i][j] << ' ';
            std::cout << std::endl;
        }
    }
};