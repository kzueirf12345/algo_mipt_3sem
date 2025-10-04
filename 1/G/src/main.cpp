#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>
#include <climits>
#include <cmath>


int main() {
    size_t target_sum, n;
    std::cin >> target_sum >> n;
    std::vector<size_t> coins(n);

    size_t total_sum = 0;
    for (size_t i = 0; i < n; ++i) {
        std::cin >> coins[i];
        total_sum += 2 * coins[i];
    }

    size_t min_cnt = std::numeric_limits<size_t>::max();
    bool is_resolve = false;

    size_t max_mask = (size_t)powl(3, n);
    

    for (size_t mask = 0; mask < max_mask; ++mask) {
        size_t sum = 0;
        size_t cnt = 0;
        size_t mask2 = mask;

        for (size_t coin_num = 0; coin_num < n; ++coin_num) {
            size_t choice = mask2 % 3;

            sum   += choice * coins[coin_num];
            cnt   += choice;
            mask2 /= 3;
        }

        if (sum == target_sum) {
            is_resolve = true;
            min_cnt = std::min(min_cnt, cnt);
        }
    }

    if (is_resolve) {
        std::cout << min_cnt << "\n";
    } else if (total_sum >= target_sum) {
        std::cout << "Change\n";
    } else {
        std::cout << "Impossible\n";
    }

    return 0;
}