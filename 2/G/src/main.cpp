#include <iostream>
#include <vector>

size_t MergeAndCount(std::vector<int>& arr, std::vector<int>& temp, size_t left, size_t mid, size_t right);

size_t CountInverstions(std::vector<int>& arr, std::vector<int>& temp, size_t left, size_t right);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t n = 0;
    std::cin >> n;

    std::vector<int> arr(n);
    for (size_t i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    std::vector<int> temp(n);
    size_t inversions = CountInverstions(arr, temp, 0, n);

    std::cout << inversions << std::endl;

    return 0;
}

size_t MergeAndCount(std::vector<int>& arr, std::vector<int>& temp, size_t left, size_t mid, size_t right)
{
    size_t leftInd = left;
    size_t rightInd = mid;
    size_t tempInd = left;
    size_t invCnt = 0;

    while (leftInd < mid && rightInd < right) {
        if (arr[leftInd] <= arr[rightInd]) {
            temp[tempInd++] = arr[leftInd++];
        } else {
            temp[tempInd++] = arr[rightInd++];
            invCnt += mid - leftInd;
        }
    }

    while (leftInd < mid) {
        temp[tempInd++] = arr[leftInd++];
    }

    while (rightInd < right) {
        temp[tempInd++] = arr[rightInd++];
    }

    for (size_t pos = left; pos < right; ++pos) {
        arr[pos] = temp[pos];
    }

    return invCnt;
}

size_t CountInverstions(std::vector<int>& arr, std::vector<int>& temp, size_t left, size_t right)
{
    if (right - left <= 1) {
        return 0;
    }

    size_t mid = (left + right) / 2;
    size_t invCnt = 0;

    invCnt += CountInverstions(arr, temp, left, mid);
    invCnt += CountInverstions(arr, temp, mid, right);
    invCnt += MergeAndCount(arr, temp, left, mid, right);

    return invCnt;
}