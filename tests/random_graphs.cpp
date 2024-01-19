#include <iostream>
#include <cstring>
#include <random>
#include <chrono>

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    
    int n = atoi(argv[1]);
    double p = atof(argv[2]);
    int tests_count = atoi(argv[3]);
    
    std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
    std::bernoulli_distribution rnd(p);
    
    std::cout << tests_count << '\n';
    
    while (tests_count--) {
        std::cout << n << '\n';
        
        for (int v = 0; v < n; ++v) {
            for (int u = v + 1; u < n; ++u) {
                if (rnd(gen)) {
                    std::cout << v << ' ' << u << '\n';
                }
            }
        }
        
        std::cout << "0 0\n";
    }
}
