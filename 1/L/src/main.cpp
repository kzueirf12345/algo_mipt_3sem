#include <iostream>
#include <vector>

void intersect(const std::vector<size_t>& arr1, const std::vector<size_t>& arr2, std::vector<size_t>& res);


int main()
{
    size_t n = 0, m = 0;

    std::cin >> n >> m;
    std::vector<size_t> arr1(n);
    std::vector<size_t> arr2(m);

    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr1[i];
    }

    for (size_t i = 0; i < m; ++i) {
        std::cin >> arr2[i];
    }

    std::vector<size_t> res;

    intersect(arr1, arr2, res);

    for (const auto& elem : res) {
        std::cout << elem << " ";
    }

    std::cout << "\n";

    return 0;
}

void intersect(const std::vector<size_t>& arr1, const std::vector<size_t>& arr2, std::vector<size_t>& res) {
    size_t ind1 = 0;
    size_t ind2 = 0;

    while (ind1 < arr1.size() && ind2 < arr2.size()) {
        if (arr1[ind1] < arr2[ind2]) {
            ++ind1;
        } else if (arr1[ind1] > arr2[ind2]) {
            ++ind2;
        } else {
            res.push_back(arr1[ind1]);
            ++ind1;
            ++ind2;
        }
    }
}