#include <chrono>
#include <fstream>
#include <random>
#include <vector>

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
void MergeSortRec(std::vector<T> &a, std::vector<T> &b, int left, int right) {
  if (right - left <= 1) {
    return;
  }
  int mid = left + (right - left) / 2;
  MergeSortRec(a, b, left, mid);
  MergeSortRec(a, b, mid, right);
  MergeArrays(a, b, left, mid, right);
}

template <typename T>
void MergeSort(std::vector<T> &a) {
  if (a.empty()) {
    return;
  }
  std::vector<T> b(a.size());
  MergeSortRec(a, b, 0, a.size());
}

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
void MergeSortHybridRec(std::vector<T> &a, std::vector<T> &b, int left, int right, int threshold) {
  const int len = right - left;
  if (len <= threshold) {
    InsertionSort(a, left, right);
    return;
  }
  int mid = left + len / 2;
  MergeSortHybridRec(a, b, left, mid, threshold);
  MergeSortHybridRec(a, b, mid, right, threshold);
  MergeArrays(a, b, left, mid, right);
}

template <typename T>
void MergeSortHybrid(std::vector<T> &a, int threshold) {
  if (a.empty()) {
    return;
  }
  std::vector<T> b(a.size());
  MergeSortHybridRec(a, b, 0, a.size(), threshold);
}

class ArrayGenerator {
 public:
  enum class Type { FULLY_RANDOM, REVERSED_SORTED, NEARLY_SORTED };

  ArrayGenerator(const int max_size, const int min_val, const int max_val)
      : max_size_(max_size)
      , min_val_(min_val)
      , max_val_(max_val)
      , rng_(static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())) {
    GenerateBases();
  }

  std::vector<int> GetArray(const Type type, int size) {
    size = std::min(size, max_size_);
    switch (type) {
      case Type::FULLY_RANDOM:
        return {base_random_.begin(), base_random_.begin() + size};
      case Type::REVERSED_SORTED:
        return {base_reversed_.begin(), base_reversed_.begin() + size};
      case Type::NEARLY_SORTED:
        return {base_nearly_sorted_.begin(), base_nearly_sorted_.begin() + size};
    }
    return {};
  }

 private:
  int max_size_;
  int min_val_, max_val_;
  std::mt19937_64 rng_;

  std::vector<int> base_random_;
  std::vector<int> base_reversed_;
  std::vector<int> base_nearly_sorted_;

  void GenerateBases() {
    std::uniform_int_distribution dist(min_val_, max_val_);
    base_random_.resize(max_size_);
    for (int i = 0; i < max_size_; ++i) {
      base_random_[i] = dist(rng_);
    }

    base_reversed_ = base_random_;
    sort(base_reversed_.begin(), base_reversed_.end(), std::greater<int>());

    base_nearly_sorted_ = base_random_;
    sort(base_nearly_sorted_.begin(), base_nearly_sorted_.end());
    const int swaps = std::max(1, max_size_ / 100);
    std::uniform_int_distribution idx(0, max_size_ - 1);
    for (int k = 0; k < swaps; ++k) {
      const int i = idx(rng_);
      if (const int j = idx(rng_); i != j) {
        std::swap(base_nearly_sorted_[i], base_nearly_sorted_[j]);
      }
    }
  }
};

class SortTester {
 public:
  enum class Algorithm { MERGE_STD, MERGE_HYBRID };

  SortTester(ArrayGenerator &gen, const int min_size, const int max_size, const int step, const int repetitions)
      : gen_(gen), min_size_(min_size), max_size_(max_size), step_(step), repetitions_(repetitions) {
  }

  void RunAllToCsv(const std::string &filename) const {
    std::ofstream out(filename);
    out << "size,data_type,algorithm,threshold,iteration,time_µs\n";

    std::vector thresholds = {5, 10, 15, 20, 30, 50};

    for (int n = min_size_; n <= max_size_; n += step_) {
      for (int t = 0; t < 3; ++t) {
        auto type = static_cast<ArrayGenerator::Type>(t);
        std::string type_name = DataTypeToString(type);

        for (int run = 0; run < repetitions_; ++run) {
          std::vector<int> a = gen_.GetArray(type, n);
          auto start = std::chrono::high_resolution_clock::now();
          MergeSort(a);
          auto elapsed = std::chrono::high_resolution_clock::now() - start;
          int64_t msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

          out << n << "," << type_name << ",merge_std," << 0 << "," << run << "," << msec << "\n";
        }

        for (int thr : thresholds) {
          for (int run = 0; run < repetitions_; ++run) {
            std::vector<int> a = gen_.GetArray(type, n);
            auto start = std::chrono::high_resolution_clock::now();
            MergeSortHybrid(a, thr);
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            int64_t msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

            out << n << "," << type_name << ",merge_hybrid," << thr << "," << run << "," << msec << "\n";
          }
        }
        out.flush();
      }
    }
  }

 private:
  ArrayGenerator &gen_;
  int min_size_, max_size_, step_, repetitions_;

  static std::string DataTypeToString(const ArrayGenerator::Type type) {
    switch (type) {
      case ArrayGenerator::Type::FULLY_RANDOM:
        return "random";
      case ArrayGenerator::Type::REVERSED_SORTED:
        return "reversed";
      case ArrayGenerator::Type::NEARLY_SORTED:
        return "nearly_sorted";
    }
    return "unknown";
  }
};

int main() {
  constexpr int kMaxSize = 100000;
  constexpr int kMinSize = 500;
  constexpr int kStep = 100;
  constexpr int kRepetitions = 5;
  constexpr int kMinVal = 0;
  constexpr int kMaxVal = 6000;

  ArrayGenerator gen(kMaxSize, kMinVal, kMaxVal);
  const SortTester tester(gen, kMinSize, kMaxSize, kStep, kRepetitions);

  tester.RunAllToCsv("./results.csv");

  return 0;
}