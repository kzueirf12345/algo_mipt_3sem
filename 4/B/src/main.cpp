#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <utility>
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

    inline static constexpr Vertex VERTEX_POISON = std::numeric_limits<uint32_t>::max();
    
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

    virtual ~CommonGraph() = default; 

public:
    
                    virtual Vertex              addVertex   ()                                          = 0 ;
                    virtual bool                addEdge     (Vertex src, Vertex dst)                    = 0 ;
    
    [[nodiscard]]   virtual size_t              nVertices   ()                          const noexcept      ;
    [[nodiscard]]   virtual size_t              nEdges      ()                          const           = 0 ;
    [[nodiscard]]   virtual bool                has         (Edge edge)                 const               ;
    [[nodiscard]]   virtual const std::vector<Vertex>& getAdjuscent(Vertex vertex)             const noexcept      ;

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

    mutable std::vector<std::vector<Vertex>> adj_;
    size_t edges_cnt_ = 0;

};


class PlainGraph: public CommonGraph {

public:

    using Component = uint64_t;

public:
                    virtual Vertex                  addVertex                   ()                                                          override    final   ;
                    virtual bool                    addEdge                     (Vertex src, Vertex dst)                                    override    final   ;
    [[nodiscard]]   virtual size_t                  nEdges                      ()                                      const   noexcept    override    final   ;

    [[nodiscard]]   virtual ErrorCode               validate                    ()                                      const               override    final   ;
                    virtual void                    dump                        (const char* filename)                  const   noexcept    override    final   ;

public:

    [[nodiscard]]           bool                           isTree                      ()                                      const                                   ;
    [[nodiscard]]           bool                           isForest                    ()                                      const                                   ;
    [[nodiscard]]           size_t                         nJointComponents            ()                                      const                                   ;
    [[nodiscard]]           const std::vector<Component>&  getJointComponents          ()                                      const                                   ;
    [[nodiscard]]           const std::vector<Edge>&       getBridges                  ()                                      const                                   ;
    [[nodiscard]]           const std::vector<Vertex>&     getArticulationPoints       ()                                      const                                   ;

private:

    bool isMultiEdge(Vertex u, Vertex v) const noexcept {
        const auto& adj_u = adj_[u];
        auto it = std::lower_bound(adj_u.begin(), adj_u.end(), v);
        if (it == adj_u.end() || *it != v) return false;
        
        return (it + 1 != adj_u.end() && *(it + 1) == v);
    }

    mutable bool is_sorted_ = false;

    struct Cache {
        bool is_tree = true;
        bool is_forest = true;
        size_t n_components = 0;
        std::vector<Component> components;
        std::vector<Edge> bridges;
        std::vector<Vertex> articulation_points;
        bool is_valid = false;

        void reset(size_t n) {
            is_tree = true;
            is_forest = true;
            n_components = 0;
            components.assign(n, VERTEX_POISON);
            bridges.clear();
            articulation_points.clear();
            is_valid = false;
        }
    } mutable cache_;

    struct Utils {
        std::vector<int64_t> tin;
        std::vector<int64_t> fup;
        int64_t timer = 0;
        bool has_cycle = false;

        struct StackFrame {
            uint32_t vertex;
            uint32_t parent;
            size_t next_ind;
            bool is_multi_edge;
            int32_t children;
            bool is_articulation;
            bool entered;
        };

        std::vector<StackFrame> stack;

        void reset(size_t n) {
            (void)n;
            tin.assign(n, -1);
            fup.assign(n, -1);
            stack.clear();
            stack.reserve(n);
            timer = 0;
            has_cycle = false;
        }
    } mutable utils_;

    void rebuildCache() const {
        const size_t n = nVertices();

        cache_.reset(n);

        if (n == 0) {
            cache_.is_valid = true;
            return;
        }

        utils_.reset(n);

        if (!is_sorted_) {
            for (std::vector<Vertex>& neibs: adj_) {
                std::sort(neibs.begin(), neibs.end());
            }
            is_sorted_ = true;
        }
        
        
        for (Vertex v = 0; v < n; ++v) {
            if (utils_.tin[v] == -1) {
                dfsCombined(v, VERTEX_POISON);
                ++cache_.n_components;
            }

        }

        cache_.is_tree = (cache_.n_components == 1) && !utils_.has_cycle && (nEdges() == n - 1);
        cache_.is_forest = !utils_.has_cycle;

        cache_.is_valid = true;
    }

    void dfsCombined(Vertex start, Vertex parent) const {
        
        utils_.stack.emplace_back(start, parent, 0, isMultiEdge(start, parent), 0, false, false);
        
        while (!utils_.stack.empty()) {
            Utils::StackFrame& frame = utils_.stack.back();
            Vertex v = frame.vertex;
            
            if (!frame.entered) {
                cache_.components[v] = cache_.n_components;
                utils_.tin[v] = utils_.fup[v] = utils_.timer++;
                frame.entered = true;
            }
            
            const auto& neighbors = adj_[v];
            const size_t neib_cnt = neighbors.size();
            
            bool pushed_child = false;
            while (frame.next_ind < neib_cnt) {
                const Vertex neighbor = neighbors[frame.next_ind];
                do {
                    ++frame.next_ind;
                } while (frame.next_ind < neib_cnt && neighbors[frame.next_ind] == neighbor);
                
                if (neighbor == frame.parent) {
                    continue;
                }
                
                if (utils_.tin[neighbor] == -1) {
                    utils_.stack.emplace_back(neighbor, v, 0, isMultiEdge(v, neighbor), 0, false, false);
                    pushed_child = true;
                    break;
                }
                else {
                    utils_.has_cycle = true;
                    utils_.fup[v] = std::min(utils_.fup[v], utils_.tin[neighbor]);
                }
            }
            
            if (pushed_child) {
                continue;
            }
            
            utils_.stack.pop_back();
            
            if (!utils_.stack.empty()) {
                Utils::StackFrame& parent_frame = utils_.stack.back();
                Vertex p = parent_frame.vertex;
                const bool is_multi_edge = frame.is_multi_edge;
                
                if (is_multi_edge) {
                    utils_.has_cycle = true;
                }
                
                utils_.fup[p] = std::min(utils_.fup[p], utils_.fup[v]);
                
                if (!is_multi_edge && utils_.fup[v] > utils_.tin[p]) {
                    cache_.bridges.emplace_back(v, p);
                }
                
                if (utils_.fup[v] >= utils_.tin[p] && parent_frame.parent != VERTEX_POISON) {
                    parent_frame.is_articulation = true;
                }
                
                ++parent_frame.children;
            }
            else {
                if (frame.children > 1) {
                    frame.is_articulation = true;
                }
            }
            
            if (frame.is_articulation) {
                cache_.articulation_points.push_back(v);
            }
        }
    }

};

class DirectionalGraph: public CommonGraph {

public:

                    virtual Vertex                                              addVertex   ()                                              override    final   ;
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

size_t CommonGraph::nVertices() const noexcept {
    return adj_.size();
}

bool CommonGraph::has(CommonGraph::Edge edge) const {
    if (edge.src >= adj_.size() || edge.dst >= adj_.size()) {
        return false;
    }
    return std::find(adj_[edge.src].begin(), adj_[edge.src].end(), edge.dst) != adj_[edge.src].end();
}

const std::vector<CommonGraph::Vertex>& CommonGraph::getAdjuscent(
    CommonGraph::Vertex vertex
) const noexcept {
    return adj_[vertex];
}

//==================================================================================================
// PlainGraph
//==================================================================================================

PlainGraph::Vertex PlainGraph::addVertex() {
    adj_.push_back({});

    cache_.is_valid = false;

    return adj_.size() - 1;
}

bool PlainGraph::addEdge(PlainGraph::Vertex src, PlainGraph::Vertex dst) {
    if (src >= adj_.size()) {
        return false;
    }
    if (dst >= adj_.size()) {
        return false;
    }

    adj_[src].push_back(dst);

    adj_[dst].push_back(src);
    
    ++edges_cnt_;

    cache_.is_valid = false;
    is_sorted_ = false;

    return true;
}

size_t PlainGraph::nEdges() const noexcept {
    return edges_cnt_;
}

PlainGraph::ErrorCode PlainGraph::validate() const {
    const size_t vertexes_cnt = adj_.size();
    
    for (Vertex vertex = 0; vertex < vertexes_cnt; ++vertex) {
        const auto& neighbors = adj_[vertex];
        
        // // Список смежности должен быть отсортирован
        // if (!std::is_sorted(neighbors.begin(), neighbors.end())) {
        //     return ErrorCode::NotSortedNeighbours;
        // }

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
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.is_tree;
}

bool PlainGraph::isForest() const {
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.is_forest;
}

size_t PlainGraph::nJointComponents() const {
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.n_components;
}

const std::vector<PlainGraph::Component>& PlainGraph::getJointComponents() const {
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.components;
}

const std::vector<PlainGraph::Edge>& PlainGraph::getBridges() const {
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.bridges;
}

const std::vector<PlainGraph::Vertex>& PlainGraph::getArticulationPoints() const {
    if (!cache_.is_valid) {
        rebuildCache();
    }
    return cache_.articulation_points;
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

DirectionalGraph::Vertex DirectionalGraph::addVertex() {
    adj_.push_back({});

    return adj_.size() - 1;
}


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