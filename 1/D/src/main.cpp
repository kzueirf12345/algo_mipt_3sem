#include <iostream>
#include <vector>

void FillGrayCode(size_t width, std::vector<size_t>& arr);

int main() {

    size_t N = 0;
    std::cin >> N;

    std::vector<size_t> arr;

    FillGrayCode(N, arr);

    for (const auto& elem : arr) {
        size_t mask = 1;
        for (size_t shift = 0; shift < N; ++shift) {
            std::cout << !!(elem & mask);
            mask <<= 1;
        }
        std::cout << std::endl;
    }

    return 0;
}

void FillGrayCode(size_t width,  std::vector<size_t>& arr) {
    if (width == 1) {
        arr.push_back(0);
        arr.push_back(1);
        return;
    }

    FillGrayCode(width - 1, arr);

    size_t size = arr.size();

    for (size_t i = 0; i < size; ++i) {
        size_t cur_ind = size - i - 1;
        arr[cur_ind] = (arr[cur_ind] << 1);
        arr.push_back(arr[cur_ind] + 1);
    }

    return;
}