#include <iostream>
#include <vector>

#pragma GCC target("popcnt,abm,bmi,bmi2")

int mds(const std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    int res = n;
    
    for (int mask = 1; mask < (1 << n); ++mask) {
        bool is_ds = true;
        for (int v = 0; v < n; ++v) {
            if ((mask >> v) & 1) {
                continue;
            }
            
            bool covered = false;
            for (int u : graph[v]) {
                if ((mask >> u) & 1) {
                    covered = true;
                    break;
                }
            }
            if (!covered) {
                is_ds = false;
                break;
            }
        }
        
        if (is_ds) {
            res = std::min(res, __builtin_popcount(mask));
        }
    }
    
    return res;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    
    int tests_count;
    std::cin >> tests_count;
    
    while (tests_count--) {
        int n;
        std::cin >> n;
        
        std::vector<std::vector<int>> graph(n);
        while (true) {
            int u, v;
            std::cin >> u >> v;
            if (u == 0 && v == 0) {
                break;
            }
            
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        
        std::cout << mds(graph) << '\n';
    }
}
