#include <algorithm>
#include <iostream>
#include <vector>

void init(std::vector<size_t>& arr);
void sym_dif(const std::vector<size_t>& arr1, const std::vector<size_t>& arr2, std::vector<size_t>& res);


int main()
{
    std::vector<size_t> arr1;
    std::vector<size_t> arr2;

    init(arr1);
    init(arr2);

    std::sort(arr1.begin(), arr1.end());
    std::sort(arr2.begin(), arr2.end());

    std::vector<size_t> res;

    sym_dif(arr1, arr2, res);

    for (const auto& elem : res) {
        std::cout << elem << " ";
    }

    std::cout << "\n";

    return 0;
}

void init(std::vector<size_t>& arr) {
    size_t num = 0;
    while (true) {
        std::cin >> num;
        if (num == 0) {
            break;
        }

        arr.push_back(num);
    }
}

void sym_dif(const std::vector<size_t>& arr1, const std::vector<size_t>& arr2, std::vector<size_t>& res) {
    size_t ind1 = 0;
    size_t ind2 = 0;

    while (ind1 < arr1.size() && ind2 < arr2.size()) {
        if (arr1[ind1] < arr2[ind2]) {
            res.push_back(arr1[ind1]);
            ++ind1;
        } else if (arr1[ind1] > arr2[ind2]) {
            res.push_back( arr2[ind2] );
            ++ind2;
        } else {
            ++ind1;
            ++ind2;
        }
    }

    while (ind1 < arr1.size()) {
        res.push_back(arr1[ind1++]);
    }

    while (ind2 < arr2.size()) {
        res.push_back(arr2[ind2++]);
    }
}