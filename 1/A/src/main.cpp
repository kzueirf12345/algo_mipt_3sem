#include <iostream>


int main() {

    size_t N = 0;
    std::cin >> N;

    ssize_t result  = 0;
    ssize_t  counter5 = 0;

    for (size_t i = 0; i < N; ++i) {
        ssize_t num = 0;
        std::cin >> num;
        if (num == 5) {
            ++counter5;
        } else {
            ssize_t need5 = (num / 5 - 1);

            if (counter5 < need5) {
                result += need5 - counter5;
                counter5 = 0;
            } else {
                counter5 -= need5;
            }
        }
    }

    std::cout << std::max(0l, result) << std::endl;
}