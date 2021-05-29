#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>
namespace graph {
/**
 * Graph
 * 
 * Given some type T and its hasher H, stores its elements internally as integers in the range [0,N)
 * where N is the number of unique elements of type T inserted
 * **/
template <typename T, typename H>
class Graph {
    using AdjacentList = typename std::vector<std::set<int>>;

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

        graph.push_back(std::set<int>());
        return new_id;
    }

   public:
    Graph() = default;

    auto clear() { graph.clear(), key_to_id.clear(), id_to_key.clear(); }
    auto empty() const { return graph.empty(); }
    auto size() const { return graph.size(); }

    void addNode(const T& t1) {
        getId(t1);
    }

    void addUndirectedEdge(const T& t1, const T& t2) {
        addDirectedEdge(t1, t2);
        addDirectedEdge(t2, t1);
    }

    void addDirectedEdge(const T& t1, const T& t2) {
        int id_t1 = getId(t1);
        int id_t2 = getId(t2);
        graph[id_t1].insert(id_t2);
    }

    bool hasDirectEdge(const T& t1, const T& t2) {
        int id_t1 = getId(t1);
        int id_t2 = getId(t2);
        return std::count(graph[id_t1].begin(), graph[id_t1].end(), id_t2);
    }

    std::set<int> getSuccessors(int t) {
        return graph[t];
    }

    std::unordered_map<T, std::vector<T>, H> getAdjacentList() {
        std::unordered_map<T, std::vector<T>, H> hash;
        int indx = 0;
        for ( auto node : graph ) {
            for ( auto keys : node ) {
                hash[id_to_key[indx]].push_back(id_to_key[keys]);
            }
            indx++;
        }
        return hash;
    }

    int keyToId(const T& t) {    // ver que onda esto
        return key_to_id[t];
    }

    T idToKey(int t) {
        return id_to_key[t];
    }

    void show_graph() {
        for ( int i = 0; i < graph.size(); ++i ) {
            std::cout << "node" << i << std::endl;
            std::cout << "info: ";
            id_to_key[i].print();
            std::cout << std::endl;
            for ( auto node = graph[i].begin(); node != graph[i].end(); ++node ) {
                id_to_key[*node].print();
                std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }

    int getDegree(const T& t) {
        return graph[key_to_id[t]].size();
    }

    std::vector<T> getNodes() {
        std::vector<T> nodes;
        for ( int i = 0; i < graph.size(); i++ ) {
            nodes.push_back(id_to_key[i]);
        }
        return nodes;
    }
};

};    // namespace graph
#endif