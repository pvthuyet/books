#include <vector>
#include <string>
#include <algorithm>

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
    static void findMaxPath()
    {
        int n = 5;
        vector<vector<int>> m
        { {1, 1, 0, 0, 0},
          {0, 1, 1, 0, 0},
          {0, 0, 1, 0, 1},
          {1, 0, 0, 0, 1},
          {0, 1, 0, 1, 1},
        };
        int longest = 0;
        vector<pair<int, int>> direction{
            {-1, -1},
            {-1,  0},
            {-1,  1},
            { 0, -1},
            { 0,  1},
            { 1, -1},
            { 1,  0},
            { 1,  1}
        };
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (m[i][j] != 0) {
                    vector<vector<int>> visited(n, vector<int>(n, 0));
                    BackTracking{}.maxPath(m, n, i, j, visited, 0, longest, direction);
                }
            }
        }
        std::cout << "Max path: " << longest << endl;
    }

    static vector<string> generateAllBinaryStrings(int n)
    {
        string str(n, '0');
        vector<string> res;
        BackTracking{}.binary(n, str, res);
        //res = BackTracking{}.generateAllBinaryStrings2(n);
        for (auto& s : res) cout << s << '\n';
        cout << '\n';
        return res;
    }

    static void generateAllStrings(string input, int k)
    {
        vector<string> res;
        string str(k, input[0]);
        int n = input.length();
        BackTracking{}.genStrings(input, n, k, str, res);
        for (auto& s : res) cout << s << '\n';
    }

    static void NQueen(int N)
    {
        vector<vector<int>> result;
        int row[10] = { 0 };
        BackTracking{}.bt(0, N, row, result);
        for (auto& row : result) {
            for (auto& cell : row) cout << cell << ' ';
            cout << endl;
        }
    }

    static void HamiltonianPath()
    {
        int N = 10, M = 14;
        vector<vector<int>> Edges{ {8, 1}, {8, 2}, {1, 3}, {5, 4}, {1, 5}, {8, 6}, {1, 7}, {2, 9}, {5, 10}, {7, 8}, {6, 3}, {3, 5}, {7, 9}, {6, 10} };
        //int N = 4, M = 4;
        //vector<vector<int>> Edges{ {1,2}, {2,3}, {3,4}, {2,4} };

        auto rs = BackTracking{}.checkHam(N, M, Edges);
        cout << "ham result: " << rs << endl;
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

    void maxPath(auto& m, int n, int i, int j, auto& visited, int len, int& longest, auto const& direction)
    {
        if (!isValidMaxPath(m, n, i, j) || (visited[i][j] == 1)) {
            if (longest < len) 
                longest = len;
            return;
        }
        visited[i][j] = 1;
        for (auto& d : direction) {
            maxPath(m, n, i + d.first, j + d.second, visited, len + 1, longest, direction);
        }
        visited[i][j] = 1;
    }
    bool isValidMaxPath(auto& m, int n, int i, int j)
    {
        return (i < n && i >=0 ) && (j < n && j >= 0) && (m[i][j] == 1);
    }

    void binary(int n, string& s, vector<string> &result)
    {
        if (n < 1) {
            result.push_back(s);
            return;
        }
        s[n - 1] = '0';
        binary(n - 1, s, result);
        s[n - 1] = '1';
        binary(n - 1, s, result);
    }
    vector<string> generateAllBinaryStrings2(int n)
    {
        vector<string> res{ string(n, '0'), string(n, '1') };
        string str(n, '0');
        for (int i = n-1; i > 0; --i) {
            str[i] = '1';
            do {
                res.push_back(str);
            } while (std::next_permutation(str.begin(), str.end()));
        }
        return res;
    }

    void genStrings(string const& input, int n, int k, string& s, vector<string>& res)
    {
        if (k < 1) {
            res.push_back(s);
            return;
        }
        for (int i = 0; i < n; ++i) {
            s[k - 1] = input[i];
            genStrings(input, n, k-1, s, res);
        }
    }

    bool place(int r, int c, int row[]) 
    {
        for (int prev = 0; prev < c; prev++) {
            if (row[prev] == r or abs(row[prev] - r) == abs(prev - c))
                return false;
        }
        return true;
    }

    void bt(int c, int n, int row[], auto& result) 
    {
        if (n == 2 or n == 3)
            return;

        if (c == n) {
            vector<int> v;
            for (int i = 0; i < n; i++)
                v.push_back(row[i] + 1);
            result.push_back(v);
        }

        for (int i = 0; i < n; i++) {
            if (place(i, c, row)) {
                row[c] = i;
                bt(c + 1, n, row, result);
            }
        }
    }

    bool checkHam(int N, int M, vector<vector<int>> Edges)
    {
        vector<vector<int>> adj(N, vector<int>(N, 0));
        for (auto& e : Edges) {
            adj[e[0] - 1][e[1] - 1] = adj[e[1] - 1][e[0] - 1] = 1;
        }

        vector<int> path(N, -1);
        path[0] = 0;
        bool rs = false;
        rs = checking(N, adj, path, 0, 1);
        
        cout << "path: ";
        for (auto x : path) cout << x + 1 << ' ';
        cout << endl;
        return rs;
    }

    //void print(auto& v, int N) {
    //    for (int i = 1; i <= N; ++i) {
    //        std::cout << i << ": ";
    //        for (auto& x : v[i]) cout << x << ' ';
    //        cout << endl;
    //    }
    //}

    bool isValid(int N, auto const& adj, auto& path, int v, int pos)
    {
        if (adj[v][path[pos - 1]] == 0) return false;

        for (int i = 0; i < pos; ++i) {
            if (path[i] == v) return false;
        }

        //if (pos == N - 1 && adj[v][path[0]] == 0) {
        //    return false;
        //}
        return true;
    }

    bool checking(int N, auto& adj, auto& path, int v, int pos)
    {
        if (pos == N) return true;
        for (int i = 0; i < N; ++i) {
            if (isValid(N, adj, path, i, pos)) {
                path[pos] = i;
                if (checking(N, adj, path, i, pos + 1)) return true;
                path[pos] = -1;
            }
        }
        return false;
    }
};