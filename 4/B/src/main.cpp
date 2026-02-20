

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

#include <cstddef>
#include <cstdint>
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
    
                    Vertex              addVertex   ()                                  ;
                    bool                addEdge     (Vertex& src, Vertex& dst)          ;
    
    [[nodiscard]]   size_t              nVertices   ()                          const   ;
    [[nodiscard]]   size_t              nEdges      ()                          const   ;
    [[nodiscard]]   bool                has         (Edge edge)                 const   ;
    [[nodiscard]]   std::vector<Vertex> getAdjuscent(Vertex vertex)             const   ;

public:

    enum ErrorCode {
        NoError = 0
    };

    [[nodiscard]]   ErrorCode           validate    ()                          const   ;
                    void                dump        (const char* filename)      const   ;

protected:

    /*!SECTION
    something for get and set adj_ 
    */

private:

    std::vector<std::vector<Vertex>> adj_;
};


class PlainGraph: public CommonGraph {

public:

    [[nodiscard]] bool                    isTree                ()  const;
    [[nodiscard]] bool                    isForest              ()  const;
    [[nodiscard]] size_t                  nJointComponents      ()  const;
    [[nodiscard]] std::vector<uint64_t>   getJointComponents    ()  const; //vec[vertex] = id_comp 

    [[nodiscard]] std::vector<Edge>       getBridges            ()  const;
    [[nodiscard]] std::vector<Vertex>     getArticulationPoints ()  const;

};

class DirectionalGraph: public CommonGraph {

public:

    [[nodiscard]]   bool                                                isDAG       ()  const;
    [[nodiscard]]   std::vector<Vertex>                                 getSources  ()  const;
    [[nodiscard]]   std::vector<Vertex>                                 getSinks    ()  const;
    [[nodiscard]]   DirectionalGraph                                    reverse     ()  const;
    [[nodiscard]]   std::vector<Vertex>                                 topological ()  const;
    [[nodiscard]]   std::pair<DirectionalGraph, std::vector<Vertex>>    condence    ()  const;

};