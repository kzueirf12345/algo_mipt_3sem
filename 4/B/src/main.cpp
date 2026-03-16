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

    // Список смежности
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

    [[nodiscard]]   bool                            isTree                      ()                                      const                                   ;
    [[nodiscard]]   bool                            isForest                    ()                                      const                                   ;
    [[nodiscard]]   size_t                          nJointComponents            ()                                      const                                   ;
    [[nodiscard]]   const std::vector<Component>&   getJointComponents          ()                                      const                                   ;
    [[nodiscard]]   const std::vector<Edge>&        getBridges                  ()                                      const                                   ;
    [[nodiscard]]   const std::vector<Vertex>&      getArticulationPoints       ()                                      const                                   ;

private:


    // Проверка на мультиребро, используем инвариант, что храним соседей отсортированно
    bool isMultiEdge(Vertex u, Vertex v) const noexcept {
        const auto& adj_u = adj_[u];
        auto it = std::lower_bound(adj_u.begin(), adj_u.end(), v);
        if (it == adj_u.end() || *it != v) return false;
        
        return (it + 1 != adj_u.end() && *(it + 1) == v);
    }

    // Сортируем только когда нужно чекать MultiEdge
    mutable bool is_sorted_ = false;

    struct Cache {
        bool is_tree = true;
        bool is_forest = true;
        size_t n_components = 0;
        std::vector<Component> components;
        std::vector<Edge> bridges;
        std::vector<Vertex> articulation_points;
        bool is_valid = false; // Нужно ли пересчитывать закешированные данные

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
        std::vector<int64_t> tin; // время входа в ноду
        std::vector<int64_t> fup; // вспомогательный массив минимумов времени входа для нахождения мостов и точек сочленения
        int64_t timer = 0; // счётчик времени входа
        bool has_cycle = false;

        struct StackFrame {
            uint32_t vertex; // текущая нода
            uint32_t parent; // родитель текущей ноды
            size_t next_ind; // индекс следующего соседа в adj_[vertex]
            bool is_multi_edge; // мульти ребро или нет
            int32_t children; // количество детей vertex
            bool is_articulation; // флаг ан проверку того, что это точка сочленения (потому что нужно отдельно обрабаывать корень)
            bool entered; // флаг на стартовую инициализацию, когда ещё не начали обходить соседей
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

        if (n == 0) { // дефолтные значения кеша корректны для пустого графа
            cache_.is_valid = true;
            return;
        }

        utils_.reset(n);

        if (!is_sorted_) { // Если соседи неотсортированы, то сортируем
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

        //стартовая вершина
        utils_.stack.emplace_back(start, parent, 0, isMultiEdge(start, parent), 0, false, false); 
        
        while (!utils_.stack.empty()) {
            Utils::StackFrame& frame = utils_.stack.back();
            Vertex v = frame.vertex;
            
            if (!frame.entered) { // ещё не начали обходить соседей
                cache_.components[v] = cache_.n_components;
                utils_.tin[v] = utils_.fup[v] = utils_.timer++;
                frame.entered = true;
            }
            
            const auto& neighbors = adj_[v];
            const size_t neib_cnt = neighbors.size();
            
            bool pushed_child = false; // в стэк добавлен новый фрейм
            while (frame.next_ind < neib_cnt) {
                const Vertex neighbor = neighbors[frame.next_ind];
                do { // скипаем мульти рёбра, чтобы лишний раз по ним не ходить
                    ++frame.next_ind;
                } while (frame.next_ind < neib_cnt && neighbors[frame.next_ind] == neighbor);
                
                if (neighbor == frame.parent) { // если это ребро в родителя. то не обрабатываем
                    continue;
                }
                
                if (utils_.tin[neighbor] == -1) { // сосед ещё не посещён
                    utils_.stack.emplace_back(neighbor, v, 0, isMultiEdge(v, neighbor), 0, false, false);
                    pushed_child = true;
                    break;
                }
                else { // сосед уже посещён
                    utils_.has_cycle = true;
                    utils_.fup[v] = std::min(utils_.fup[v], utils_.tin[neighbor]);
                }
            }
            
            if (pushed_child) { // в стэк добавлен новый фрейм
                continue;
            }
            
            utils_.stack.pop_back(); 
            
            // постобработка 

            if (!utils_.stack.empty()) { // не корень
                Utils::StackFrame& parent_frame = utils_.stack.back();
                Vertex p = parent_frame.vertex;
                const bool is_multi_edge = frame.is_multi_edge;
                
                if (is_multi_edge) { // петля
                    utils_.has_cycle = true;
                }
                
                utils_.fup[p] = std::min(utils_.fup[p], utils_.fup[v]);
                
                if (!is_multi_edge && utils_.fup[v] > utils_.tin[p]) {
                    cache_.bridges.emplace_back(v, p);
                }
                
                if (utils_.fup[v] >= utils_.tin[p]) { // уже гарантируется что не корень
                    parent_frame.is_articulation = true;
                }
                
                ++parent_frame.children;
            }
            else { // корень
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
        White   = 0, // ещё не встречалаь
        Gray    = 1, // сейчас обрабатывается
        Black   = 2  // уже обработана
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
    is_sorted_ = false; // сортировать будем, как понадобиться, при обновлении кэша

    return true;
}

size_t PlainGraph::nEdges() const noexcept {
    return edges_cnt_;
}

PlainGraph::ErrorCode PlainGraph::validate() const {
    const size_t vertexes_cnt = adj_.size();
    
    for (Vertex vertex = 0; vertex < vertexes_cnt; ++vertex) {
        const auto& neighbors = adj_[vertex];

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
    // проверка на кратные рёбра
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

// Проходимся в глубину и на выходе пуши в вектор. То есть по факту сортируем по времени выхода. 
// Ещё заодно проверяем на DAG, иначе нельзя построить топологическую сортировку
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

// В порядке топсорта будем идти по вершинам и искать компоненты сильной связности, для этого будем ходить по развёрнотому графу. 
// На самом деле не топсорт, а сортировка по времени выхода, потому что строго говоря топсорта не существует для не DAG-ов, 
// а конденсация - существует.
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

    // Чтобы проставить рёбра пройдёмся по всем вершинам исходного графа, посмотрим всех соседей. 
    // Если они из разных КСС, то вставляем это ребро
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