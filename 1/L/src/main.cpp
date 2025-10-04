#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n, m;
    cin >> n;
    cin >> m;
    
    vector<int> A(n);
    vector<int> B(m);
    
    for (int i = 0; i < n; i++) {
        cin >> A[i];
    }
    
    for (int i = 0; i < m; i++) {
        cin >> B[i];
    }
    
    // Выбираем меньший массив для итерации
    vector<int>* iter_arr;
    vector<int>* search_arr;
    
    if (n <= m) {
        iter_arr = &A;
        search_arr = &B;
    } else {
        iter_arr = &B;
        search_arr = &A;
    }
    
    vector<int> result;
    
    // Итерируемся по меньшему массиву и ищем элементы в большем
    for (int num : *iter_arr) {
        // Бинарный поиск в большем массиве
        int left = 0;
        int right = search_arr->size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if ((*search_arr)[mid] == num) {
                result.push_back(num);
                break;
            } else if ((*search_arr)[mid] < num) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    }
    
    // Выводим результат
    for (int i = 0; i < result.size(); i++) {
        cout << result[i];
        if (i < result.size() - 1) {
            cout << " ";
        }
    }
    cout << endl;
    
    return 0;
}