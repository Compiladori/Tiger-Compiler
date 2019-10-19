#ifndef __TOPOSORT_H__
#define __TOPOSORT_H__

#include <vector>
#include <queue>
#include <unordered_map>
#include "../AST/AST.h"

namespace tpsrt {

/**
 * Toposorter
 * 
 * Given some type T and its hasher H, stores its elements internally as integers in the range [0,N)
 * where N is the number of unique elements of type T inserted
 * **/
template <typename T, typename H>
class Toposorter {
    using Graph = typename std::vector<std::vector<int>>;
    
    Graph graph;
    std::unordered_map<T, int, H> key_to_id;
    std::unordered_map<int, T>    id_to_key;
    
    int getId(const T& key){
        if(key_to_id.count(key)){
            return key_to_id[key];
        }
        int new_id = key_to_id.size();
        key_to_id[key] = new_id;
        id_to_key[new_id] = key;
        
        graph.push_back(std::vector<int>());
        return new_id;
    }
    
public:
    Toposorter() {}
    
    auto clear()       { graph.clear(), key_to_id.clear(), id_to_key.clear(); }
    auto empty() const { return graph.empty(); }
    auto size()  const { return graph.size(); }
    
    void addDirectedEdge(const T& t1, const T& t2){
        int id_t1 = getId(t1);
        int id_t2 = getId(t2);
        graph[id_t1].push_back(id_t2);
    }
    
    std::vector<T> sort(){        
        std::vector<int> in_degree(graph.size(), 0);
        
        for(const auto& v : graph)
            for(auto id : v)
                in_degree[id]++;
        
        std::queue<int> ready;
        for(int i = 0; i < graph.size(); i++)
            if(in_degree[i] == 0)
                ready.push(i);
        
        std::vector<T> answer;
        while(not ready.empty()){
            auto id = ready.front();
            ready.pop();
            
            answer.push_back(id_to_key[id]);
            
            for(auto next_id : graph[id]){
                in_degree[next_id]--;
                
                if(in_degree[next_id] == 0)
                    ready.push(next_id);
            }
        }
        
        return answer;
    }
};


};

#endif
