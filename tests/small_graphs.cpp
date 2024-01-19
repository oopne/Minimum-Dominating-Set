#include <iostream>

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);
    
    int n = atoi(argv[1]);
    int m = n * (n - 1) / 2;
    
    std::cout << (1 << m) << '\n';
    
    for (int mask = 0; mask < (1 << m); ++mask) {
        std::cout << n << '\n';
        
        int id = 0;
        for (int v = 0; v < n; ++v) {
            for (int u = v + 1; u < n; ++u, ++id) {
                if ((mask >> id) & 1) {
                    std::cout << v << ' ' << u << '\n';
                }
            }
        }
        
        std::cout << "0 0\n";
    }
}
