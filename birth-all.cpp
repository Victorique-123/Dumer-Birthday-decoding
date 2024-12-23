#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <set>

using namespace std;

struct VectorBoolCompare {
    bool operator()(const vector<bool>& a, const vector<bool>& b) const {
        if (a.size() != b.size()) return a.size() < b.size();
        for (size_t i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) return a[i] < b[i];
        }
        return false;
    }
};

int hammingWeight(const vector<bool>& v) {
    return count(v.begin(), v.end(), true);
}

vector<bool> matrixMultiply(const vector<bool>& vec, const vector<vector<bool>>& matrix) {
    vector<bool> result(matrix.size(), false);
    for (int i = 0; i < matrix.size(); i++) {
        bool sum = false;
        for (int j = 0; j < matrix[0].size(); j++) {
            sum ^= (vec[j] && matrix[i][j]);
        }
        result[i] = sum;
    }
    return result;
}

pair<vector<int>, vector<int>> randomSplitColumns(int n) {
    vector<int> indices(n);
    for (int i = 0; i < n; i++) indices[i] = i;
    
    random_device rd;
    mt19937 gen(rd());
    shuffle(indices.begin(), indices.end(), gen);
    
    int half = n/2;
    return {
        vector<int>(indices.begin(), indices.begin() + half),
        vector<int>(indices.begin() + half, indices.end())
    };
}

vector<vector<bool>> createSubMatrix(const vector<vector<bool>>& H, const vector<int>& indices) {
    vector<vector<bool>> subMatrix(H.size(), vector<bool>(indices.size()));
    for (int i = 0; i < H.size(); i++) {
        for (int j = 0; j < indices.size(); j++) {
            subMatrix[i][j] = H[i][indices[j]];
        }
    }
    return subMatrix;
}

void generateCombinations(int n, int k, const function<void(const vector<bool>&)>& callback) {
    vector<bool> combination(n, false);
    vector<int> positions(k);
    
    for (int i = 0; i < k; i++) {
        positions[i] = i;
        combination[i] = true;
    }
    
    callback(combination);
    
    while (true) {
        int i;
        for (i = k - 1; i >= 0; i--) {
            if (positions[i] < n - (k - i)) break;
        }
        if (i < 0) break;
        
        combination[positions[i]] = false;
        positions[i]++;
        combination[positions[i]] = true;
        
        for (int j = i + 1; j < k; j++) {
            combination[positions[j]] = false;
            positions[j] = positions[j-1] + 1;
            combination[positions[j]] = true;
        }
        
        callback(combination);
    }
}

double binomialCoefficient(int n, int k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    vector<double> C(k + 1, 0);
    C[0] = 1;
    
    for (int i = 1; i <= n; i++) {
        for (int j = min(i, k); j > 0; j--) {
            C[j] = C[j] + C[j-1];
        }
    }
    return C[k];
}

double calculateP(int n, int w) {
    double nCw2 = binomialCoefficient(n/2, w/2);
    double nCw = binomialCoefficient(n, w);
    return (nCw2 * nCw2) / nCw;
}

vector<vector<bool>> birthdayDecoding(const vector<vector<bool>>& H, const vector<bool>& s, int w) {
    int n = H[0].size();
    int n_minus_k = H.size();
    
    double P = calculateP(n, w);
    int attempts = ceil(1.0 / P);
    
    cout << "P = " << P << endl;
    cout << "Number of attempts = " << attempts << endl;
    
    set<vector<bool>, VectorBoolCompare> uniqueSolutions;
    int w1 = w / 2;
    int w2 = w - w1;

    for(int attempt = 0; attempt < attempts; attempt++) {
        auto [left_indices, right_indices] = randomSplitColumns(n);
        vector<vector<bool>> H1 = createSubMatrix(H, left_indices);
        vector<vector<bool>> H2 = createSubMatrix(H, right_indices);
        
        unordered_map<string, vector<vector<bool>>> table;
        
        generateCombinations(H1[0].size(), w1, [&](const vector<bool>& e1) {
            vector<bool> x = matrixMultiply(e1, H1);
            string key;
            for (bool b : x) key += (b ? '1' : '0');
            table[key].push_back(e1);
        });
        
        generateCombinations(H2[0].size(), w2, [&](const vector<bool>& e2) {
            vector<bool> x = matrixMultiply(e2, H2);
            for (int j = 0; j < n_minus_k; j++) x[j] = x[j] ^ s[j];
            
            string key;
            for (bool b : x) key += (b ? '1' : '0');
            
            if (table.count(key)) {
                for (const auto& e1 : table[key]) {
                    vector<bool> e(n, false);
                    for (size_t i = 0; i < left_indices.size(); i++) {
                        e[left_indices[i]] = e1[i];
                    }
                    for (size_t i = 0; i < right_indices.size(); i++) {
                        e[right_indices[i]] = e2[i];
                    }
                    
                    if (hammingWeight(e) == w) {
                        uniqueSolutions.insert(e);
                    }
                }
            }
        });
        
        cout << "Attempt " << attempt + 1 << " completed. Current unique solutions: " 
             << uniqueSolutions.size() << endl;
    }
    
    return vector<vector<bool>>(uniqueSolutions.begin(), uniqueSolutions.end());
}

bool readTestCases(const string& filename, vector<pair<string, tuple<vector<vector<bool>>, vector<bool>, int>>>& testCases) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line, currentTestName;
    vector<vector<bool>> H;
    vector<bool> s;
    int w;
    bool readingCase = false;

    while (getline(file, line)) {
        if (line.empty()) continue;

        if (line.substr(0, 3) == "###") {
            if (line.find("END") != string::npos) break;

            if (readingCase) {
                testCases.push_back({currentTestName, {H, s, w}});
                H.clear();
                s.clear();
            }

            size_t start = line.find(":") + 1;
            currentTestName = line.substr(start, line.find("###", start));
            currentTestName = currentTestName.substr(0, currentTestName.find_last_not_of(" \t") + 1);
            readingCase = true;

            if (!getline(file, line)) return false;
            istringstream iss(line);
            int rows, cols;
            if (!(iss >> rows >> cols)) return false;

            H.resize(rows, vector<bool>(cols));
            for (int i = 0; i < rows; i++) {
                if (!getline(file, line)) return false;
                istringstream row_iss(line);
                for (int j = 0; j < cols; j++) {
                    int val;
                    if (!(row_iss >> val)) return false;
                    H[i][j] = val;
                }
            }

            if (!getline(file, line)) return false;
            istringstream s_iss(line);
            s.resize(rows);
            for (int i = 0; i < rows; i++) {
                int val;
                if (!(s_iss >> val)) return false;
                s[i] = val;
            }

            if (!getline(file, line)) return false;
            istringstream w_iss(line);
            if (!(w_iss >> w)) return false;
        }
    }

    if (readingCase) {
        testCases.push_back({currentTestName, {H, s, w}});
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    vector<pair<string, tuple<vector<vector<bool>>, vector<bool>, int>>> testCases;
    
    if (!readTestCases(argv[1], testCases)) {
        cerr << "Failed to read test cases\n";
        return 1;
    }

    for (const auto& [name, testCase] : testCases) {
        cout << "\n=== Running Test: " << name << " ===" << endl;
        const auto& [H, s, w] = testCase;
        
        vector<vector<bool>> solutions = birthdayDecoding(H, s, w);
        cout << "\nFound " << solutions.size() << " unique solutions:" << endl;
        
        int solutionNum = 1;
        for(const auto& solution : solutions) {
            cout << "Solution " << solutionNum++ << ": ";
            for(bool b : solution) cout << b << " ";
            cout << endl;
            
            // 验证解
            vector<bool> result = matrixMultiply(solution, H);
            bool isValid = true;
            for(size_t i = 0; i < s.size(); i++) {
                if(result[i] != s[i]) {
                    isValid = false;
                    break;
                }
            }
            if(!isValid) {
                cout << "Warning: Invalid solution detected!" << endl;
            }
        }
    }
    return 0;
}