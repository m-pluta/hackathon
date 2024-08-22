#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <fstream>
#include <cstdint>
#include <string>
#include <cmath>

using namespace std;
typedef uint8_t digit;

int dijkstra(const int N, const vector<digit>& mat) {
    // Priority queue for Dijkstra's algorithm
    using P = pair<int, pair<int, int>>;
    priority_queue<P, vector<P>, greater<P>> heap;
    heap.push({mat[0], {0, 0}});
    
    // Distance table: initialize with infinity
    vector<int> distances(N * N, INT_MAX);
    distances[0] = mat[0];
    
    // Directions for moving up, down, left, right
    vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    while (!heap.empty()) {
        // Pop the cheapest from the priority queue
        auto [current_cost, cell] = heap.top(); heap.pop();
        auto [row, col] = cell;
        
        // If we reached the bottom-right corner, return the current cost
        if (row == N - 1 && col == N - 1) {
            return current_cost;
        }
        
        // Process each neighbor (up, down, left, right)
        for (const auto& direction : directions) {
            int new_row = row + direction.first;
            int new_col = col + direction.second;
            
            // Make sure the new position is within bounds
            if (new_row >= 0 && new_row < N && new_col >= 0 && new_col < N) {
                int idx = new_row * N + new_col; // Convert to 1D index
                int new_cost = current_cost + mat[idx];
                
                // If a shorter path to this neighbor is found
                if (new_cost < distances[idx]) {
                    distances[idx] = new_cost;
                    heap.push({new_cost, {new_row, new_col}});
                }
            }
        }
    }
    
    // If we never reached the bottom-right corner, somehow?
    return -1;
}

int main(int argc, char* argv[]) {
    // Check command call
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    // Check opening the file
    string filename = argv[1];
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return 1;
    }

    // Parse the data in the file
    string line;
    vector<digit> mat;
    while (getline(inputFile, line)) {
        for (char c : line) {
            if (isdigit(c)) {
                mat.push_back(static_cast<digit>(c - '0')); // Correct conversion
            }
        }
    }
    inputFile.close();

    // Check if any digits were parsed
    if (mat.empty()) {
        cerr << "No digits found in " << filename << endl;
        return 1;
    }


    // Verify if N*N equals the count to check for a perfect square
    size_t num_digits = mat.size();
    size_t N = static_cast<size_t>(sqrt(num_digits));
    if (N * N != num_digits) {
        cerr << "The number of digits (" << num_digits << ") does not form a square matrix." << endl;
        return 1;
    }

    // Run dijkstra, print result
    int result = dijkstra(N, mat);
    cout << result << endl;

    return 0;
}
