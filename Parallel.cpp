#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <omp.h>

const int SEQUENTIAL_THRESHOLD = 1000;

void quickSortSequential(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    quickSortSequential(arr, left, j);
    quickSortSequential(arr, i, right);
}

void quickSortParallel(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;

    if (right - left < SEQUENTIAL_THRESHOLD) {
        quickSortSequential(arr, left, right);
        return;
    }

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    #pragma omp task shared(arr)
    {
        quickSortParallel(arr, left, j);
    }
    #pragma omp task shared(arr)
    {
        quickSortParallel(arr, i, right);
    }
    #pragma omp taskwait
}

int main() {
    std::vector<int> arr(1000000); 
    for (int& num : arr) {
        num = rand() % 1000000;
    }

    auto start = std::chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        #pragma omp single
        {
            quickSortParallel(arr, 0, arr.size() - 1);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Parallel Quicksort time: " << duration.count() << " microseconds" << std::endl;

    return 0;
}
