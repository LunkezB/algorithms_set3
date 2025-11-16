#include <vector>
#include <iostream>

constexpr int kThreshold = 15;

template <typename T>
void InsertionSort(std::vector<T> &a, const int left, const int right) {
  for (int i = left + 1; i < right; ++i) {
    T key = a[i];
    int j = i - 1;
    while (j >= left && a[j] > key) {
      a[j + 1] = a[j];
      --j;
    }
    a[j + 1] = key;
  }
}

template <typename T>
void MergeArrays(std::vector<T> &a, std::vector<T> &b, const int left, const int mid, const int right) {
  int i = left;
  int j = mid;
  int k = left;
  while (i < mid && j < right) {
    if (a[i] <= a[j]) {
      b[k++] = a[i++];
    } else {
      b[k++] = a[j++];
    }
  }
  while (i < mid) {
    b[k++] = a[i++];
  }
  while (j < right) {
    b[k++] = a[j++];
  }
  for (int t = left; t < right; ++t) {
    a[t] = b[t];
  }
}

template <typename T>
void MergeSortHybridRec(std::vector<T> &a, std::vector<T> &b, int left, int right) {
  const int len = right - left;
  if (len <= kThreshold) {
    InsertionSort(a, left, right);
    return;
  }
  int mid = left + len / 2;
  MergeSortHybridRec(a, b, left, mid);
  MergeSortHybridRec(a, b, mid, right);
  MergeArrays(a, b, left, mid, right);
}

template <typename T>
void MergeSortHybrid(std::vector<T> &a) {
  if (a.empty()) {
    return;
  }
  std::vector<T> b(a.size());
  MergeSortHybridRec(a, b, 0, a.size());
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n = 0;
  std::cin >> n;

  std::vector<int64_t> a(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> a[i];
  }

  MergeSortHybrid(a);

  for (int i = 0; i < n; ++i) {
    if (i) {
      std::cout << ' ';
    }
    std::cout << a[i];
  }

  return 0;
}