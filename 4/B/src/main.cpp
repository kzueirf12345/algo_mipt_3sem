

/*!SECTION
В этой задаче вам предлагается разработать свою мини библиотеку для работы с невзвешенными графами 
без петель и кратных ребер. Реализуйте структуры данных PlainGraph и DiectionalGraph и методы работы 
с ними.

Ваши типы (или type-alias) должны удовлетворять соответсвующем концептам. 
Сами концепты добавлять в код не надо, они буду присоеденены автоматически.

*/


/*
namespace traits {
    template<class T>
    concept Vertex = std::same_as<uint64_t, T>;

    template<class T>
    concept Edge = Vertex<typename T::Vertex> && requires(T e) {
        { e.src } -> std::same_as<typename T::Vertex&>;
        { e.dst } -> std::same_as<typename T::Vertex&>;
    };

    namespace detail {
        template <typename T, typename InnerType>
        concept rangeOf = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, InnerType>;
    }

    template<class T>
    concept CommonGraph = std::regular<T> && Vertex<typename T::Vertex> && Edge<typename T::Edge> && std::same_as<typename T::Vertex, typename T::Edge::Vertex> &&
    requires(T g, const T& cg, typename T::Edge e, typename T::Vertex v) {
        { g.addVertex()        } -> std::same_as<typename T::Vertex>; // Return new vertex
        { g.addEdge(v, v)      } -> std::same_as<bool>;               // Return false in case of any error.

        { cg.nVertices()       } -> std::same_as<size_t>;
        { cg.nEdges()          } -> std::same_as<size_t>;
        { cg.has(e)            } -> std::same_as<bool>;
        { cg.getAdjuscent(v)   } -> detail::rangeOf<typename T::Vertex>;

        cg.validate();
        cg.dump((const char*)nullptr); // Dump to file.
    };

    template<class T>
    concept PlainGraph = CommonGraph<T> &&
    requires(const T& cg) {
        { cg.isTree()                } -> std::same_as<bool>;
        { cg.isForest()              } -> std::same_as<bool>;
        { cg.nJointComponents()      } -> std::same_as<size_t>;
        { cg.getJointComponents()    } -> detail::rangeOf<uint64_t>; // Id of component corresponding vertex belong to
        { cg.getBridges()            } -> detail::rangeOf<typename T::Edge>;
        { cg.getArticulationPoints() } -> detail::rangeOf<typename T::Vertex>;
    };

    template<class T>
    concept DirectionalGraph = CommonGraph<T> &&
    requires(const T& cg) {
        { cg.isDAG()       } -> std::same_as<bool>;
        { cg.getSources()  } -> detail::rangeOf<typename T::Vertex>;
        { cg.getSinks()    } -> detail::rangeOf<typename T::Vertex>;

        { cg.reverse()     } -> std::same_as<T>; // Reverse all edges (vertices must be the same)
        { cg.topological() } -> detail::rangeOf<typename T::Vertex>; //Range of all vertices sorted topologically
        { cg.condense()    } -> std::same_as<std::pair<T, std::vector<typename T::Vertex>>>; // Condensed graph + mapping of vertices from old graph to new one

    };
};

*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

class CommonGraph {

public:

    using Vertex = uint64_t;
    
    struct Edge {

        using Vertex = uint64_t;

        Vertex& src;
        Vertex& dst;

        Edge(Vertex& src, Vertex& dst) noexcept
            : src(src), dst(dst) 
        {}

    };

public:
    
                    virtual Vertex              addVertex   ()                                              ;
                    virtual bool                addEdge     (Vertex src, Vertex dst)                    = 0 ;
    
    [[nodiscard]]   virtual size_t              nVertices   ()                          const noexcept      ;
    [[nodiscard]]   virtual size_t              nEdges      ()                          const           = 0 ;
    [[nodiscard]]   virtual bool                has         (Edge edge)                 const               ;
    [[nodiscard]]   virtual std::vector<Vertex> getAdjuscent(Vertex vertex)             const noexcept      ;

public:

    enum ErrorCode {
        NoError = 0
    };

    [[nodiscard]]   virtual ErrorCode           validate    ()                          const           = 0 ;
                    virtual void                dump        (const char* filename)      const           = 0 ;

protected:

    std::vector<std::vector<Vertex>> adj_;

};


class PlainGraph: public CommonGraph {

public:

    using Component = uint64_t;

    static inline constexpr Component POISON_COMPONENT_NUM = std::numeric_limits<Component>::max();

public:

                    virtual bool                    addEdge                 (Vertex src, Vertex dst)                                    override    final   ;
    [[nodiscard]]   virtual size_t                  nEdges                  ()                                      const   noexcept    override    final   ;

    [[nodiscard]]   virtual ErrorCode               validate                ()                                      const               override    final   ;
                    virtual void                    dump                    (const char* filename)                  const               override    final   ;

public:

    [[nodiscard]]           bool                    isTree                  ()                                      const                                   ;
    [[nodiscard]]           bool                    isForest                ()                                      const                                   ;
    [[nodiscard]]           size_t                  nJointComponents        ()                                      const                                   ;
    [[nodiscard]]           std::vector<Component>  getJointComponents      ()                                      const                                   ; //vec[vertex] = id_comp 
    [[nodiscard]]           std::vector<Edge>       getBridges              ()                                      const                                   ;
    [[nodiscard]]           std::vector<Vertex>     getArticulationPoints   ()                                      const                                   ;

private:

                            bool                    hasCycleHelper          (Vertex vertex, 
                                                                             std::vector<bool>& visited, 
                                                                             Vertex parent)                         const                                   ;

                            void                    markJointComponent      (Vertex vertex, 
                                                                             std::vector<bool>& visited)            const                                   ;

                            void                    fillJointComponent      (Vertex vertex, 
                                                                             std::vector<Component>& components,
                                                                             PlainGraph::Component component)       const                                   ;

};

class DirectionalGraph: public CommonGraph {

public:

                    virtual bool                                                addEdge     (Vertex src, Vertex dst)                        override    final   ;
    [[nodiscard]]   virtual size_t                                              nEdges      ()                          const   noexcept    override    final   ;

    [[nodiscard]]   virtual ErrorCode                                           validate    ()                          const               override    final   ;
                    virtual void                                                dump        (const char* filename)      const               override    final   ;

public:

    [[nodiscard]]           bool                                                isDAG       ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 getSources  ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 getSinks    ()                          const                                   ;
    [[nodiscard]]           DirectionalGraph                                    reverse     ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 topological ()                          const                                   ;
    [[nodiscard]]           std::pair<DirectionalGraph, std::vector<Vertex>>    condence    ()                          const                                   ;

};

//==================================================================================================
// CommonGraph
//==================================================================================================

CommonGraph::Vertex CommonGraph::addVertex() {
    adj_.push_back({});
    return adj_.size() - 1;
}

//--------------------------------------------------------------------------------------------------

size_t CommonGraph::nVertices() const noexcept {
    return adj_.size();
}

bool CommonGraph::has(CommonGraph::Edge edge) const {
    return std::binary_search(adj_[edge.src].begin(), adj_[edge.src].end(), edge.dst);
}

std::vector<CommonGraph::Vertex> CommonGraph::getAdjuscent(
    CommonGraph::Vertex vertex
) const noexcept {
    return adj_[vertex];
}

//==================================================================================================
// PlainGraph
//==================================================================================================

bool PlainGraph::addEdge(PlainGraph::Vertex src, PlainGraph::Vertex dst) try {
    if (src >= adj_.size()) {
        return false;
    }
    if (dst >= adj_.size()) {
        return false;
    }
    if (src == dst) {
        return false;
    }

    {
        auto& src_neighbors = adj_[src];
    
        const auto dst_it = std::lower_bound(src_neighbors.begin(), src_neighbors.end(), dst);
        
        if (*dst_it == dst) {
            return false;
        }
    
        src_neighbors.insert(dst_it, dst);
    } 

    {
        auto& dst_neighbors = adj_[dst];
    
        const auto src_it = std::lower_bound(dst_neighbors.begin(), dst_neighbors.end(), src);
        
        if (*src_it == src) {
            return false;
        }
    
        dst_neighbors.insert(src_it, src);
    }

    return true;
}
catch (...) {
    return false;
}

size_t PlainGraph::nEdges() const noexcept {
    size_t nEdges = 0;

    for (const auto& neighbors : adj_) {
        nEdges += neighbors.size();
    }

    return nEdges / 2;
}

PlainGraph::ErrorCode PlainGraph::validate() const {
    //TODO implement
}

void PlainGraph::dump(const char* filename) const {
   //TODO implement 
}

//--------------------------------------------------------------------------------------------------

bool PlainGraph::isTree() const {

    std::vector<bool> visited(adj_.size(), false);

    if (hasCycleHelper(0, visited, 0)) {
        return false;
    }

    return !std::any_of(visited.begin(), visited.end(), false);
}

bool PlainGraph::isForest() const {

    std::vector<bool> visited(adj_.size(), false);

    for (Vertex vertex = 0; vertex < adj_.size(); ++vertex) {
        if (!visited[vertex]) {
            if (hasCycleHelper(0, visited, 0)) {
                return false;
            }
        }
    }

    return true;
}

size_t PlainGraph::nJointComponents() const {

    std::vector<bool> visited(adj_.size(), false);

    size_t nJointComponents = 0;

    for (Vertex vertex = 0; vertex < adj_.size(); ++vertex) {
        if (!visited[vertex]) {
            markJointComponent(vertex, visited);
            ++nJointComponents;
        }
    }

    return nJointComponents;
}

std::vector<PlainGraph::Component> PlainGraph::getJointComponents() const {

    std::vector<PlainGraph::Component> components(adj_.size(), POISON_COMPONENT_NUM);

    PlainGraph::Component component = 0;

    for (Vertex vertex = 0; vertex < adj_.size(); ++vertex) {
        if (components[vertex] == POISON_COMPONENT_NUM) {
            fillJointComponent(vertex, components, component);
            ++component;
        }
    }

    return components;
}

std::vector<PlainGraph::Edge> PlainGraph::getBridges() const {
// TODO implement
}

std::vector<PlainGraph::Vertex> PlainGraph::getArticulationPoints() const {
// TODO implement
}

//--------------------------------------------------------------------------------------------------

bool PlainGraph::hasCycleHelper(
    PlainGraph::Vertex vertex, 
    std::vector<bool>& visited, 
    PlainGraph::Vertex parent
) const {

    visited[vertex] = true;

    for (auto neighbor : adj_[vertex]) {
        if (!visited[neighbor]) {
            if (hasCycleHelper(neighbor, visited, vertex)) {
                return true;
            }
        }
        else if (neighbor != parent) {
            return true;
        }
    }

    return false;
}

void PlainGraph::markJointComponent(PlainGraph::Vertex vertex, std::vector<bool>& visited) const {

    visited[vertex] = true;

    for (auto neighbor : adj_[vertex]) {
        if (!visited[neighbor]) {
            markJointComponent(neighbor, visited);
        }
    }
}

void PlainGraph::fillJointComponent(
    PlainGraph::Vertex vertex, 
    std::vector<PlainGraph::Component>& components,
    PlainGraph::Component component
) const {

    components[vertex] = component;

    for (auto neighbor : adj_[vertex]) {
        if (components[vertex] != component) {
            fillJointComponent(neighbor, components, component);
        }
    }

}

//==================================================================================================
// DirectionalGraph
//==================================================================================================

bool DirectionalGraph::addEdge(DirectionalGraph::Vertex src, DirectionalGraph::Vertex dst) try {
    if (src >= adj_.size()) {
        return false;
    }
    if (dst >= adj_.size()) {
        return false;
    }
    if (src == dst) {
        return false;
    }
    
    auto& src_neighbors = adj_[src];

    const auto dst_it = std::lower_bound(src_neighbors.begin(), src_neighbors.end(), dst);
    
    if (*dst_it == dst) {
        return false;
    }

    src_neighbors.insert(dst_it, dst);

    return true;
}
catch (...) {
    return false;
}

size_t DirectionalGraph::nEdges() const noexcept {
    size_t nEdges = 0;

    for (const auto& neighbors : adj_) {
        nEdges += neighbors.size();
    }

    return nEdges;
}

DirectionalGraph::ErrorCode DirectionalGraph::validate() const {
    //TODO implement
}

void DirectionalGraph::dump(const char* filename) const {
   //TODO implement 
}

//--------------------------------------------------------------------------------------------------

bool DirectionalGraph::isDAG() const {
// TODO implement
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::getSources() const {
// TODO implement
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::getSinks() const {
// TODO implement
}

DirectionalGraph DirectionalGraph::reverse() const {
// TODO implement
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::topological() const {
// TODO implement
}

std::pair<DirectionalGraph, std::vector<DirectionalGraph::Vertex>> DirectionalGraph::condence() const {
// TODO implement
}
