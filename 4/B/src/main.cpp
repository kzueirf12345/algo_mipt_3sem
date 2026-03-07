/*!SECTION
В этой задаче вам предлагается разработать свою мини библиотеку для работы с невзвешенными графами. 
Реализуйте структуры данных PlainGraph и DiectionalGraph и методы работы 
с ними.

Ваши типы (или type-alias) должны удовлетворять соответсвующем концептам. 
Сами концепты добавлять в код не надо, они буду присоеденены автоматически.

*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>
#include <cassert>
#include <fstream>
#include <string>
#include <iostream>

// #include <concepts>

// namespace traits {
//     template<class T>
//     concept Vertex = std::same_as<uint64_t, T>;

//     template<class T>
//     concept Edge = Vertex<typename T::Vertex> && requires(T e) {
//         { e.src } -> std::same_as<typename T::Vertex&>;
//         { e.dst } -> std::same_as<typename T::Vertex&>;
//     };

//     namespace detail {
//         template <typename T, typename InnerType>
//         concept rangeOf = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, InnerType>;
//     }

//     template<class T>
//     concept CommonGraph = std::regular<T> && Vertex<typename T::Vertex> && Edge<typename T::Edge> && std::same_as<typename T::Vertex, typename T::Edge::Vertex> &&
//     requires(T g, const T& cg, typename T::Edge e, typename T::Vertex v) {
//         { g.addVertex()        } -> std::same_as<typename T::Vertex>; // Return new vertex
//         { g.addEdge(v, v)      } -> std::same_as<bool>;               // Return false in case of any error.

//         { cg.nVertices()       } -> std::same_as<size_t>;
//         { cg.nEdges()          } -> std::same_as<size_t>;
//         { cg.has(e)            } -> std::same_as<bool>;
//         { cg.getAdjuscent(v)   } -> detail::rangeOf<typename T::Vertex>;

//         cg.validate();
//         cg.dump((const char*)nullptr); // Dump to file.
//     };

//     template<class T>
//     concept PlainGraph = CommonGraph<T> &&
//     requires(const T& cg) {
//         { cg.isTree()                } -> std::same_as<bool>;
//         { cg.isForest()              } -> std::same_as<bool>;
//         { cg.nJointComponents()      } -> std::same_as<size_t>;
//         { cg.getJointComponents()    } -> detail::rangeOf<uint64_t>; // Id of component corresponding vertex belong to
//         { cg.getBridges()            } -> detail::rangeOf<typename T::Edge>;
//         { cg.getArticulationPoints() } -> detail::rangeOf<typename T::Vertex>;
//     };

//     template<class T>
//     concept DirectionalGraph = CommonGraph<T> &&
//     requires(const T& cg) {
//         { cg.isDAG()       } -> std::same_as<bool>;
//         { cg.getSources()  } -> detail::rangeOf<typename T::Vertex>;
//         { cg.getSinks()    } -> detail::rangeOf<typename T::Vertex>;

//         { cg.reverse()     } -> std::same_as<T>; // Reverse all edges (vertices must be the same)
//         { cg.topological() } -> detail::rangeOf<typename T::Vertex>; //Range of all vertices sorted topologically
//         { cg.condense()    } -> std::same_as<std::pair<T, std::vector<typename T::Vertex>>>; // Condensed graph + mapping of vertices from old graph to new one

//     };
// };

class CommonGraph {

public:

    using Vertex = uint64_t;

    inline static constexpr Vertex VERTEX_POISON = std::numeric_limits<Vertex>::max();
    
    struct Edge {

        using Vertex = uint64_t;

        Vertex src;
        Vertex dst;

        Edge(Vertex src_, Vertex dst_) noexcept
            : src(src_), dst(dst_) 
        {}

    };

    auto operator<=>(const CommonGraph& other) const = default; 

public:
    
                    virtual Vertex              addVertex   ()                                              ;
                    virtual bool                addEdge     (Vertex src, Vertex dst)                    = 0 ;
    
    [[nodiscard]]   virtual size_t              nVertices   ()                          const noexcept      ;
    [[nodiscard]]   virtual size_t              nEdges      ()                          const           = 0 ;
    [[nodiscard]]   virtual bool                has         (Edge edge)                 const               ;
    [[nodiscard]]   virtual std::vector<Vertex> getAdjuscent(Vertex vertex)             const noexcept      ;

public:

    enum ErrorCode {
        NoError             = 0,
        NotSortedNeighbours = 1,
        Asymmetrical        = 4,
        InvalidNeighbourNum = 5,
        Undirected          = 6,
        IncorrectEdgesCnt   = 7,
    };

    [[nodiscard]]   virtual ErrorCode           validate    ()                          const           = 0 ;
                    virtual void                dump        (const char* filename)      const           = 0 ;

protected:

    std::vector<std::vector<Vertex>> adj_;
    size_t edges_cnt_ = 0;

};


class PlainGraph: public CommonGraph {

public:

    using Component = uint64_t;

public:

                    virtual bool                    addEdge                     (Vertex src, Vertex dst)                                    override    final   ;
    [[nodiscard]]   virtual size_t                  nEdges                      ()                                      const   noexcept    override    final   ;

    [[nodiscard]]   virtual ErrorCode               validate                    ()                                      const               override    final   ;
                    virtual void                    dump                        (const char* filename)                  const   noexcept    override    final   ;

public:

    [[nodiscard]]           bool                    isTree                      ()                                      const                                   ;
    [[nodiscard]]           bool                    isForest                    ()                                      const                                   ;
    [[nodiscard]]           size_t                  nJointComponents            ()                                      const                                   ;
    [[nodiscard]]           std::vector<Component>  getJointComponents          ()                                      const                                   ; //vec[vertex] = id_comp 
    [[nodiscard]]           std::vector<Edge>       getBridges                  ()                                      const                                   ;
    [[nodiscard]]           std::vector<Vertex>     getArticulationPoints       ()                                      const                                   ;

private:

                            bool                    hasCycleHelper              (Vertex vertex, 
                                                                                 std::vector<bool>& visited, 
                                                                                 Vertex parent)                         const                                   ;

                            void                    markJointComponent          (Vertex vertex, 
                                                                                 std::vector<bool>& visited)            const                                   ;

                            void                    fillJointComponent          (Vertex vertex, 
                                                                                 std::vector<Component>& components,
                                                                                 PlainGraph::Component component)       const                                   ;

                            void                    FindBridgesHelper           (Vertex vertex, 
                                                                                 std::vector<bool>& visited, 
                                                                                 std::vector<int64_t>& tin,
                                                                                 std::vector<int64_t>& fup,
                                                                                 std::vector<Edge>& bridges,
                                                                                 int64_t& timer,
                                                                                 Vertex parent)                         const                                   ;

                            void                    FindArticulationPointHelper (Vertex vertex, 
                                                                                 std::vector<bool>& visited, 
                                                                                 std::vector<int64_t>& tin,
                                                                                 std::vector<int64_t>& fup,
                                                                                 std::vector<Vertex>& articulation_points,
                                                                                 int64_t& timer,
                                                                                 Vertex parent)                         const                                   ;

};

class DirectionalGraph: public CommonGraph {

public:

                    virtual bool                                                addEdge     (Vertex src, Vertex dst)                        override    final   ;
    [[nodiscard]]   virtual size_t                                              nEdges      ()                          const   noexcept    override    final   ;

    [[nodiscard]]   virtual ErrorCode                                           validate    ()                          const               override    final   ;
                    virtual void                                                dump        (const char* filename)      const   noexcept    override    final   ;

public:

    [[nodiscard]]           bool                                                isDAG       ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 getSources  ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 getSinks    ()                          const                                   ;
    [[nodiscard]]           DirectionalGraph                                    reverse     ()                          const                                   ;
    [[nodiscard]]           std::vector<Vertex>                                 topological ()                          const                                   ;
    [[nodiscard]]           std::pair<DirectionalGraph, std::vector<Vertex>>    condense    ()                          const                                   ;

private:

    enum VertexColor: uint8_t {
        White   = 0,
        Gray    = 1,
        Black   = 2
    };

                            bool                    hasCycleHelper              (Vertex vertex, 
                                                                                 std::vector<VertexColor>& colors)      const                                   ;

                            bool                    topologicalHelper           (Vertex vertex, 
                                                                                 std::vector<VertexColor>& colors,
                                                                                 std::vector<Vertex>& topological)      const                                   ;

    [[nodiscard]]           std::vector<Vertex>     toutSort                    ()                                      const                                   ;

                            void                    toutSortHelper              (Vertex vertex, 
                                                                                 std::vector<bool>& visited,
                                                                                 std::vector<Vertex>& tout_sort)        const                                   ;

};

// static_assert(traits::Vertex<CommonGraph::Vertex>);
// static_assert(traits::Edge<CommonGraph::Edge>);
// static_assert(traits::DirectionalGraph<DirectionalGraph>);
// static_assert(traits::PlainGraph<PlainGraph>);

// int main() {
//     std::cout <<"lol\n";
//     return EXIT_SUCCESS;
// }

//==================================================================================================
// CommonGraph
//==================================================================================================

CommonGraph::Vertex CommonGraph::addVertex() {
    adj_.push_back({});

#ifndef NDEBUG
    assert(validate() == ErrorCode::NoError);
#endif

    return adj_.size() - 1;
}

//--------------------------------------------------------------------------------------------------

size_t CommonGraph::nVertices() const noexcept {
    return adj_.size();
}

bool CommonGraph::has(CommonGraph::Edge edge) const {
    if (edge.src >= adj_.size() || edge.dst >= adj_.size()) {
        return false;
    }
    return std::binary_search(adj_[edge.src].begin(), adj_[edge.src].end(), edge.dst);
}

std::vector<CommonGraph::Vertex> CommonGraph::getAdjuscent(
    CommonGraph::Vertex vertex
) const noexcept {
    if (vertex >= adj_.size()) {
        return {};
    }
    return adj_[vertex];
}

//==================================================================================================
// PlainGraph
//==================================================================================================

bool PlainGraph::addEdge(PlainGraph::Vertex src, PlainGraph::Vertex dst) {
    if (src >= adj_.size()) {
        return false;
    }
    if (dst >= adj_.size()) {
        return false;
    }

    {
        auto& src_neighbors = adj_[src];
    
        const auto dst_it = std::lower_bound(src_neighbors.begin(), src_neighbors.end(), dst);
    
        src_neighbors.insert(dst_it, dst);
    } 

    {
        auto& dst_neighbors = adj_[dst];
    
        const auto src_it = std::lower_bound(dst_neighbors.begin(), dst_neighbors.end(), src);
    
        dst_neighbors.insert(src_it, src);
    }

    ++edges_cnt_;

#ifndef NDEBUG
    assert(validate() == ErrorCode::NoError);
#endif

    return true;
}

size_t PlainGraph::nEdges() const noexcept {
    return edges_cnt_;
}

PlainGraph::ErrorCode PlainGraph::validate() const {
    const size_t vertexes_cnt = adj_.size();
    
    for (Vertex vertex = 0; vertex < vertexes_cnt; ++vertex) {
        const auto& neighbors = adj_[vertex];
        
        // Список смежности должен быть отсортирован
        if (!std::is_sorted(neighbors.begin(), neighbors.end())) {
            return ErrorCode::NotSortedNeighbours;
        }

        // Все соседи должны быть валидными вершинами [0, n)
        for (Vertex neighbor : neighbors) {
            if (neighbor >= vertexes_cnt) {
                return ErrorCode::InvalidNeighbourNum;
            }
        }
        
        // Проверка симметрии (неориентированность)
        for (size_t i = 0; i < neighbors.size();) {
            Vertex neighbour = neighbors[i];
            const auto& neighbour_neighbors = adj_[neighbour];

            size_t cnt_forward = std::count(neighbors.begin(), neighbors.end(), neighbour);
            size_t cnt_backward = std::count(neighbour_neighbors.begin(), neighbour_neighbors.end(), vertex);

            if (cnt_forward != cnt_backward) {
                return ErrorCode::Asymmetrical;
            }

            while (i < neighbors.size() && neighbors[i] == neighbour) {
                ++i;
            }
        }
    }
    
    size_t edge_cnt = 0;
    for (const auto& neighbors : adj_) {
        edge_cnt += neighbors.size();
    }
    
    // Каждое ребро посчитано дважды
    if (edge_cnt % 2 != 0) {
        return ErrorCode::Undirected;
    }
    
    // Проверка согласованности счётчика рёбер
    if (edge_cnt != nEdges() * 2) {
        return ErrorCode::IncorrectEdgesCnt;
    }
    
    return ErrorCode::NoError;
}

void PlainGraph::dump(const char* filename) const noexcept try {

    const std::string output_filename = (filename != nullptr) ? filename : "graph_dump.txt";
    
    std::ofstream out(output_filename);
    if (!out.is_open()) {
        return;
    }
    
    out << "===PlainGraph dump===\n";
    out << "Vertices count: " << nVertices() << '\n';
    out << "Edges count: " << nEdges() << "\n\n";
    

    out << "VERTICES:\n";
    for (Vertex v = 0; v < nVertices(); ++v) {
        out << "  " << v << '\n';
    }
    
    out << "\nEDGES:\n";
    for (Vertex src = 0; src < nVertices(); ++src) {
        for (Vertex dst : adj_[src]) {
            if (src <= dst) {
                out << "  " << src << ' ' << dst << '\n';
            }
        }
    }
}
catch (...) {
    return;
}

//--------------------------------------------------------------------------------------------------

bool PlainGraph::isTree() const {
    if (adj_.empty()) return true;

    std::vector<bool> visited(nVertices(), false);

    if (hasCycleHelper(0, visited, VERTEX_POISON)) {
        return false;
    }

    return std::all_of(visited.begin(), visited.end(), [](bool v) { return v; });
}

bool PlainGraph::isForest() const {
    if (adj_.empty()) return true;

    std::vector<bool> visited(nVertices(), false);

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (!visited[vertex]) {
            if (hasCycleHelper(vertex, visited, VERTEX_POISON)) {
                return false;
            }
        }
    }

    return true;
}

size_t PlainGraph::nJointComponents() const {

    std::vector<bool> visited(nVertices(), false);

    size_t nJointComponents = 0;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (!visited[vertex]) {
            markJointComponent(vertex, visited);
            ++nJointComponents;
        }
    }

    return nJointComponents;
}

std::vector<PlainGraph::Component> PlainGraph::getJointComponents() const {

    std::vector<PlainGraph::Component> components(nVertices(), VERTEX_POISON);

    PlainGraph::Component component = 0;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (components[vertex] == VERTEX_POISON) {
            fillJointComponent(vertex, components, component);
            ++component;
        }
    }

    return components;
}

std::vector<PlainGraph::Edge> PlainGraph::getBridges() const {
    std::vector<bool> visited(nVertices(), false);
    std::vector<int64_t> tin(nVertices(), -1);
    std::vector<int64_t> fup(nVertices(), -1);
    int64_t timer = 0;

    std::vector<Edge> bridges;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (!visited[vertex]) {
            FindBridgesHelper(
                vertex, 
                visited,
                tin,
                fup,
                bridges,
                timer,
                VERTEX_POISON
            );
        }
    }

    return bridges;
}

std::vector<PlainGraph::Vertex> PlainGraph::getArticulationPoints() const {
    std::vector<bool> visited(nVertices(), false);
    std::vector<int64_t> tin(nVertices(), -1);
    std::vector<int64_t> fup(nVertices(), -1);
    int64_t timer = 0;

    std::vector<Vertex> articulation_points;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (!visited[vertex]) {
            FindArticulationPointHelper(
                vertex, 
                visited,
                tin,
                fup,
                articulation_points,
                timer,
                VERTEX_POISON
            );
        }
    }

    return articulation_points;
}

//--------------------------------------------------------------------------------------------------

void PlainGraph::FindArticulationPointHelper(
    Vertex vertex, 
    std::vector<bool>& visited, 
    std::vector<int64_t>& tin,
    std::vector<int64_t>& fup,
    std::vector<Vertex>& articulation_points,
    int64_t& timer,
    Vertex parent
) const {
    visited[vertex] = true;
    tin[vertex] = fup[vertex] = timer++;

    int64_t children = 0;

    bool is_articulation_point = false;

    for (size_t i = 0; i < adj_[vertex].size(); ) {
        Vertex neighbor = adj_[vertex][i];
        
        while (i < adj_[vertex].size() && adj_[vertex][i] == neighbor) {
            ++i;
        }
        
        if (neighbor == parent) continue;

        if (visited[neighbor]) {
            fup[vertex] = std::min(fup[vertex], tin[neighbor]);
        } else {
            FindArticulationPointHelper(neighbor, visited, tin, fup, articulation_points, timer, vertex);
            fup[vertex] = std::min(fup[vertex], fup[neighbor]);
            
            if (fup[neighbor] >= tin[vertex] && parent != VERTEX_POISON) {
                is_articulation_point = true;
            }
            ++children;
        }
    }

    if (is_articulation_point 
     || (parent == VERTEX_POISON && children > 1) // корень
    ) {
        articulation_points.push_back(vertex);
    }
}

void PlainGraph::FindBridgesHelper(
    Vertex vertex, 
    std::vector<bool>& visited, 
    std::vector<int64_t>& tin,
    std::vector<int64_t>& fup,
    std::vector<Edge>& bridges,
    int64_t& timer,
    Vertex parent
) const {
    visited[vertex] = true;
    tin[vertex] = fup[vertex] = timer++;

    for (size_t i = 0; i < adj_[vertex].size(); ) {
        Vertex neighbor = adj_[vertex][i];
        
        size_t multiplicity = 0;
        while (i < adj_[vertex].size() && adj_[vertex][i] == neighbor) {
            ++multiplicity;
            ++i;
        }
        
        if (neighbor == parent) continue;

        if (visited[neighbor]) {
            fup[vertex] = std::min(fup[vertex], tin[neighbor]);
        } else {
            FindBridgesHelper(neighbor, visited, tin, fup, bridges, timer, vertex);
            fup[vertex] = std::min(fup[vertex], fup[neighbor]);
            
            // Ребро является мостом только
            // 1. Оно единственное между vertex и neighbor (multiplicity == 1)
            // 2. Нет обратного ребра из поддерева neighbor в vertex или выше
            if (multiplicity == 1 && fup[neighbor] > tin[vertex]) {
                Vertex src = std::min(vertex, neighbor);
                Vertex dst = std::max(vertex, neighbor);
                bridges.emplace_back(src, dst);
            }
        }
    }
}

bool PlainGraph::hasCycleHelper(
    PlainGraph::Vertex vertex, 
    std::vector<bool>& visited, 
    PlainGraph::Vertex parent
) const {

    visited[vertex] = true;

    for (auto neighbor : adj_[vertex]) {
        if (neighbor == vertex) {
            return true;
        }
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
        if (components[neighbor] == VERTEX_POISON) {
            fillJointComponent(neighbor, components, component);
        }
    }

}

//==================================================================================================
// DirectionalGraph
//==================================================================================================

static void condenseHelper(
    DirectionalGraph::Vertex vertex,
    const DirectionalGraph& reverse_graph,
    std::vector<DirectionalGraph::Vertex>& vertex_component,
    size_t component_num
);

//--------------------------------------------------------------------------------------------------

bool DirectionalGraph::addEdge(DirectionalGraph::Vertex src, DirectionalGraph::Vertex dst) {
    if (src >= adj_.size()) {
        return false;
    }
    if (dst >= adj_.size()) {
        return false;
    }
    
    auto& src_neighbors = adj_[src];

    const auto dst_it = std::lower_bound(src_neighbors.begin(), src_neighbors.end(), dst);

    src_neighbors.insert(dst_it, dst);

    ++edges_cnt_;

#ifndef NDEBUG
    assert(validate() == ErrorCode::NoError);
#endif

    return true;
}

size_t DirectionalGraph::nEdges() const noexcept {
    return edges_cnt_;
}

DirectionalGraph::ErrorCode DirectionalGraph::validate() const {
    const size_t vertexes_cnt = adj_.size();
    
    for (Vertex vertex = 0; vertex < vertexes_cnt; ++vertex) {
        const auto& neighbors = adj_[vertex];
        
        // Список смежности должен быть отсортирован
        if (!std::is_sorted(neighbors.begin(), neighbors.end())) {
            return ErrorCode::NotSortedNeighbours;
        }
        
        // Все соседи должны быть валидными вершинами [0, n)
        for (Vertex neighbor : neighbors) {
            if (neighbor >= vertexes_cnt) {
                return ErrorCode::InvalidNeighbourNum;
            }
        }
    }
    
    size_t edge_cnt = 0;
    for (const auto& neighbors : adj_) {
        edge_cnt += neighbors.size();
    }
    
    // Проверка согласованности счётчика рёбер
    if (edge_cnt != nEdges()) {
        return ErrorCode::IncorrectEdgesCnt;
    }
    
    return ErrorCode::NoError;
}


void DirectionalGraph::dump(const char* filename) const noexcept try {
    const std::string output_filename = (filename != nullptr) ? filename : "graph_dump.txt";
    
    std::ofstream out(output_filename);
    if (!out.is_open()) {
        return;
    }
    
    out << "===DirectionalGraph dump===\n";
    out << "Vertices count: " << nVertices() << '\n';
    out << "Edges count: " << nEdges() << "\n\n";
    
    out << "VERTICES:\n";
    for (Vertex v = 0; v < nVertices(); ++v) {
        out << "  " << v << '\n';
    }
    
    out << "\nEDGES:\n";
    for (Vertex src = 0; src < nVertices(); ++src) {
        for (Vertex dst : adj_[src]) {
            out << "  " << src << ' ' << dst << '\n';
        }
    }
}
catch (...) {
    return;
}

//--------------------------------------------------------------------------------------------------

bool DirectionalGraph::isDAG() const {
    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (std::binary_search(adj_[vertex].begin(), adj_[vertex].end(), vertex)) {
            return false;
        }
    }

    std::vector<VertexColor> colors(nVertices(), VertexColor::White);

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (colors[vertex] == VertexColor::White && hasCycleHelper(vertex, colors)) {
            return false;
        }
    }

    return true;
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::getSources() const {
    std::vector<bool> hasInput(nVertices(), false);

    for (const auto& neighbours: adj_) {
        for (Vertex input_vertex: neighbours) {
            hasInput[input_vertex] = true;
        }
    }

    std::vector<Vertex> sources;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex){
        if (!hasInput[vertex]) {
            sources.push_back(vertex);
        }
    }

    return sources;
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::getSinks() const {
    std::vector<Vertex> sinks;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        const std::vector<Vertex>& neighbours = adj_[vertex];
        if (neighbours.empty()) {
            sinks.push_back(vertex);
        }
    }

    return sinks;
}

DirectionalGraph DirectionalGraph::reverse() const {
    DirectionalGraph graph;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        graph.addVertex();
    }

    for (Vertex src = 0; src < nVertices(); ++src) {
        const std::vector<Vertex>& neighbours = adj_[src];
        for (Vertex dst: neighbours) {
            graph.addEdge(dst, src);
        }
    }

    return graph;
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::topological() const {
    std::vector<VertexColor> colors(nVertices(), VertexColor::White);
    std::vector<Vertex> topological;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (colors[vertex] == VertexColor::White 
         && !topologicalHelper(vertex, colors, topological)) {
            return {};
        }
    }

    std::reverse(topological.begin(), topological.end());

    return topological;
}

std::pair<DirectionalGraph, std::vector<DirectionalGraph::Vertex>> DirectionalGraph::condense() const {
    const DirectionalGraph reverse_graph(reverse());
    const std::vector<Vertex> tout_sort(toutSort());

    DirectionalGraph condence_graph;
    std::vector<Vertex> vertex_component(nVertices(), VERTEX_POISON);
    size_t component_cnt = 0;

    for (Vertex vertex: tout_sort) {
        if (vertex_component[vertex] == VERTEX_POISON) {
            condence_graph.addVertex();
            condenseHelper(vertex, reverse_graph, vertex_component, component_cnt);
            ++component_cnt;
        }
    }

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        for (Vertex neighbour: getAdjuscent(vertex)) {
            const Vertex vertex_component_num = vertex_component[vertex];
            const Vertex neighbour_component_num = vertex_component[neighbour];

            if (vertex_component_num != neighbour_component_num) {
                condence_graph.addEdge(vertex_component_num, neighbour_component_num);
            }
        }
    }

    return {condence_graph, vertex_component};
}


//--------------------------------------------------------------------------------------------------

void condenseHelper(
    DirectionalGraph::Vertex vertex,
    const DirectionalGraph& reverse_graph,
    std::vector<DirectionalGraph::Vertex>& vertex_component,
    size_t component_num
) {
    vertex_component[vertex] = component_num;
    for (DirectionalGraph::Vertex neighbour: reverse_graph.getAdjuscent(vertex)) {
        if (vertex_component[neighbour] == CommonGraph::VERTEX_POISON) {
            condenseHelper(neighbour, reverse_graph, vertex_component, component_num);
        }
    }
}

bool DirectionalGraph::hasCycleHelper(Vertex vertex, std::vector<VertexColor>& colors) const {
    colors[vertex] = VertexColor::Gray;

    for (Vertex neighbour: adj_[vertex]) {
        if (neighbour == vertex) {
            return true;
        }
        
        if (colors[neighbour] == VertexColor::White) {
            if (hasCycleHelper(neighbour, colors)) {
                return true;
            }
        }
        else if (colors[neighbour] == VertexColor::Gray) {
            return true;
        }
    }

    colors[vertex] = VertexColor::Black;

    return false;
}

bool DirectionalGraph::topologicalHelper(
    Vertex vertex, 
    std::vector<VertexColor>& colors,
    std::vector<Vertex>& topological
) const {
    colors[vertex] = VertexColor::Gray;

    for (Vertex neighbour: adj_[vertex]) {
        if (neighbour == vertex) {
            return false;
        }

        if (colors[neighbour] == VertexColor::White) {
            if (!topologicalHelper(neighbour, colors, topological)) {
                return false;
            }
        }
        else if (colors[neighbour] == VertexColor::Gray) {
            return false;
        }
    }

    colors[vertex] = VertexColor::Black;
    topological.push_back(vertex);

    return true;
}

std::vector<DirectionalGraph::Vertex> DirectionalGraph::toutSort() const {
    std::vector<bool> visited(nVertices(), false);
    std::vector<Vertex> tout_sort;

    for (Vertex vertex = 0; vertex < nVertices(); ++vertex) {
        if (!visited[vertex]) {
            toutSortHelper(vertex, visited, tout_sort);
        }
    }

    std::reverse(tout_sort.begin(), tout_sort.end());

    return tout_sort;
}

void DirectionalGraph::toutSortHelper(
    Vertex vertex, 
    std::vector<bool>& visited,
    std::vector<Vertex>& tout_sort
) const {
    visited[vertex] = true;

    for (Vertex neighbour: getAdjuscent(vertex)) {
        if (!visited[neighbour]) {
            toutSortHelper(neighbour, visited, tout_sort);
        }
    }

    tout_sort.push_back(vertex);
}
