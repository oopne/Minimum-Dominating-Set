#include <vector>
#include <deque>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <bit>
#include <chrono>

using namespace std::chrono;

#pragma GCC target("popcnt,abm,bmi,bmi2")

using set_t = __uint128_t;
const size_t MASK_SIZE = 128;
const set_t ONE_ELEMENT = 1;

int popcnt(set_t s) {
    return std::popcount((uint64_t)s) + std::popcount((uint64_t)(s >> 64));
}

int ctz(set_t s) {
    int small = std::countr_zero((uint64_t)s);
    if (small == 64) {
        return 64 + std::countr_zero((uint64_t)(s >> 64));
    }
    return small;
}

bool subset(set_t s, set_t r) {
    return (s & r) == s;
}

set_t unite(const std::vector<set_t>& family) {
    set_t result = 0;
    for (auto& set : family) {
        result |= set;
    }
    return result;
}

bool in(size_t i, set_t set) {
    return (set >> i) & ONE_ELEMENT;
}

std::vector<set_t> del(const std::vector<set_t>& family, set_t del_set) {
    std::vector<set_t> result;
    for (auto& set : family) {
        set_t reduced_set = set & (~del_set);
        if (reduced_set != 0) {
            result.push_back(reduced_set);
        }
    }
    return result;
}

namespace PolyMSC {
    class Edmonds {
    private:
        int n;
        
        std::vector<std::vector<int>> graph;
        std::vector<int> base;
        std::vector<int> match;
        std::vector<int> p;
        std::deque<int> q;
        std::vector<bool> blossom;
        
        int lca(int a, int b) {
            std::vector<bool> used(n, false);
            
            while (true) {
                a = base[a];
                used[a] = true;
                if (match[a] == -1) {
                    break;
                }
                a = p[match[a]];
            }
            
            while (true) {
                b = base[b];
                if (used[b]) {
                    return b;
                }
                b = p[match[b]];
            }
        }
        
        void mark_path(int v, int b, int children) {
            while (base[v] != b) {            
                blossom[base[v]] = blossom[base[match[v]]] = true;
                p[v] = children;
                children = match[v];
                v = p[match[v]];
            }
        }
        
        int find_path(int root) {
            std::vector<bool> used(n, 0);
            
            std::fill(p.begin(), p.end(), -1);
            std::iota(base.begin(), base.end(), 0);
         
            used[root] = true;
            q.clear();
            q.push_back(root);
            
            while (!q.empty()) {
                int v = q.front();
                q.pop_front();
                
                for (int to : graph[v]) {
                    if (base[v] == base[to] || match[v] == to) {
                        continue;
                    }
                    
                    if (to == root || (match[to] != -1 && p[match[to]] != -1)) {
                        int curbase = lca(v, to);
                        std::fill(blossom.begin(), blossom.end(), 0);
                        
                        mark_path(v, curbase, to);
                        mark_path(to, curbase, v);
                        
                        for (int i = 0; i < n; ++i) {
                            if (blossom[base[i]]) {
                                base[i] = curbase;
                                if (!used[i]) {
                                    used[i] = true;
                                    q.push_back(i);
                                }
                            }
                        }
                    } else if (p[to] == -1) {
                        p[to] = v;
                        if (match[to] == -1) {
                            return to;
                        }
                        
                        to = match[to];
                        used[to] = true;
                        q.push_back(to);
                    }
                }
            }
            return -1;
        }
        
    public:
        Edmonds(std::vector<std::vector<int>> _graph)
            : n(_graph.size())
            , graph(_graph)
            , base(n)
            , match(n)
            , p(n)
            , blossom(n)
        {}
        
        int maximum_matching() {
            int res = 0;
            
            std::fill(match.begin(), match.end(), -1);
            
            for (int i = 0; i < n; ++i) {
                if (match[i] == -1) {
                    for (int j : graph[i]) {
                        if (match[j] == -1) {
                            ++res;
                            match[j] = i;
                            match[i] = j;
                            break;
                        }
                    }
                }
            }
            
            for (int i = 0; i < n; ++i) {
                if (match[i] == -1) {
                    int v = find_path(i);
                    res += v != -1;
                    
                    while (v != -1) {
                        int pv = p[v], ppv = match[pv];
                        match[v] = pv,  match[pv] = v;
                        v = ppv;
                    }
                }
            }
            
            return res;
        }
    };
    
    int poly_msc(std::vector<set_t> family) {
        int n = popcnt(unite(family));
        std::vector<std::vector<int>> graph(MASK_SIZE);
        
        for (set_t set : family) {
            set_t tmp = set;
            
            int u = ctz(tmp);
            tmp -= ONE_ELEMENT << u;
            int v = ctz(tmp);
            
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        
        Edmonds instance(graph);

        return n - instance.maximum_matching();
    }
}

int msc(std::vector<set_t> family, bool heuristic) {
    if (family.empty()) {
        return 0;
    }
    
    for (auto it1 = family.begin(); it1 != family.end(); ++it1) {
        for (auto it2 = family.begin(); it2 != family.end(); ++it2) {
            if (it1 != it2 && subset(*it1, *it2)) {
                family.erase(it1);
                return msc(family, heuristic);
            }
        }
    }
    
    set_t universe = unite(family);
    while (universe != 0) {
        int i = ctz(universe);
        
        bool found = true;
        set_t unique_set = 0;
        for (auto& set : family) {
            if (in(i, set)) {
                if (unique_set != 0) {
                    found = false;
                    break;
                }
                unique_set = set;
            }
        }
        if (found) {
            return 1 + msc(del(family, unique_set), heuristic);
        }
        
        universe -= ONE_ELEMENT << i;
    }
    
    auto max_it = family.begin();
    for (auto it = family.begin(); it != family.end(); ++it) {
        if (popcnt(*it) > popcnt(*max_it)) {
            max_it = it;
        }
    }
    
    if (popcnt(*max_it) == 2 && heuristic) {
        return PolyMSC::poly_msc(family);
    }
    
    int result = 1 + msc(del(family, *max_it), heuristic);
    family.erase(max_it);
    return std::min(result, msc(family, heuristic));
}

int mds(const std::vector<std::vector<int>>& graph, bool heuristic) {
    std::vector<set_t> family;
    family.reserve(graph.size());
    
    int n = graph.size();
    
    for (int v = 0; v < n; ++v) {
        set_t set = ONE_ELEMENT << v;
        for (int u : graph[v]) {
            set |= ONE_ELEMENT << u;
        }
        family.push_back(set);
    }
    
    return msc(family, heuristic);
}

int main(int argc, char** argv) {
    bool heuristic = atoi(argv[1]);
    
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
        
        auto start = steady_clock::now();
        
        std::cout << mds(graph, heuristic) << '\n';
        
        auto end = steady_clock::now();
        auto interval = duration_cast<microseconds>(end - start);
        
        std::cerr << interval.count() << ' ';
    }
}
