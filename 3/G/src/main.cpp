#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>

int main() {
    size_t N, M;
    std::cin >> N >> M;
    std::vector<int64_t> requests(M);
    for (size_t i = 0; i < M; ++i) {
        std::cin >> requests[i];
    }

    std::unordered_map<int64_t, std::vector<size_t>> futurePositions; // все позиции вхождения элементов
    for (size_t i = 0; i < M; ++i) {
        futurePositions[requests[i]].push_back(i);
    }

    std::set<std::pair<size_t, int64_t>> cache; // текущee состояние кэша. Отсортировано по позиции вхождения 
    std::unordered_map<int64_t,size_t> inCache; // тоже самое что cache, только наоборот по значению получаем следующую позицию
    size_t result = 0;

    for (size_t i = 0; i < M; ++i) {
        int64_t x = requests[i];
        futurePositions[x].erase(futurePositions[x].begin());

        size_t nextPos = (futurePositions[x].empty() ? M + 1 : futurePositions[x][0]); // следующее вхождение (если нет, то фиктивный end (M + 1))

        if (inCache.count(x)) { // попали. Значит подменяем его позицию на его следующую позицию
            cache.erase({inCache[x], x});
            cache.insert({nextPos, x});
            inCache[x] = nextPos;
        } else { // не попали. Засчитываем мис, и если мы полные, то вытесняем --cache.end() (самый поздне-входящий элемент) и точно также обновляем позицию
            ++result;
            if (cache.size() == N) {
                auto it = --cache.end();
                inCache.erase(it->second);
                cache.erase(it);
            }
            cache.insert({nextPos, x});
            inCache[x] = nextPos;
        }
    }

    std::cout << result << std::endl;
}
