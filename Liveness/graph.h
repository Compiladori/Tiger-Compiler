#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
namespace graph {
/**
 * Graph
 * 
 * Given some type T and its hasher H, stores its elements internally as integers in the range [0,N)
 * where N is the number of unique elements of type T inserted
 * **/
template <typename T, typename H>
class Graph {
    using AdjacentList = typename std::vector<std::vector<int>>;

    AdjacentList graph;
    std::unordered_map<T, int, H> key_to_id;
    std::unordered_map<int, T> id_to_key;

    int getId(const T& key) {
        if ( key_to_id.count(key) ) {
            return key_to_id[key];
        }
        int new_id = key_to_id.size();
        key_to_id[key] = new_id;
        id_to_key[new_id] = key;

        graph.push_back(std::vector<int>());
        return new_id;
    }

   public:
    Graph() = default;

    auto clear() { graph.clear(), key_to_id.clear(), id_to_key.clear(); }
    auto empty() const { return graph.empty(); }
    auto size() const { return graph.size(); }

    void addDirectedEdge(const T& t1, const T& t2) {
        int id_t1 = getId(t1);
        int id_t2 = getId(t2);
        graph[id_t1].push_back(id_t2);
    }
    bool hasDirectEdge(const T& t1, const T& t2) {
        int id_t1 = getId(t1);
        int id_t2 = getId(t2);
        return std::count(graph[id_t1].begin(), graph[id_t1].end(), id_t2);
    }
    void show_graph() {
        for ( int i = 0; i < graph.size(); ++i ) {
            std::cout << "Adjacent list for node" << i << std::endl;
            for ( int j = 0; j < graph[i].size(); ++j ) {
                std::cout << graph[i][j] << ", ";
            }
            std::cout << std::endl;
        }
    }
};

};    // namespace graph
#endif