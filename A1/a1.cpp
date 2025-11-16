#include <cmath>
#include <iostream>
#include <array>
#include <iomanip>
#include <random>
#include <cstdint>

struct Circle {
  double x, y, r;
};

double MonteCarloIntersectionArea(const std::array<Circle, 3>& c, const bool use_narrow_rect,
                                  const std::int64_t n_points) {
  double x_min = NAN;
  double x_max = NAN;
  double y_min = NAN;
  double y_max = NAN;

  if (use_narrow_rect) {
    x_min = std::max(std::max(c[0].x - c[0].r, c[1].x - c[1].r), c[2].x - c[2].r);
    x_max = std::min(std::min(c[0].x + c[0].r, c[1].x + c[1].r), c[2].x + c[2].r);
    y_min = std::max(std::max(c[0].y - c[0].r, c[1].y - c[1].r), c[2].y - c[2].r);
    y_max = std::min(std::min(c[0].y + c[0].r, c[1].y + c[1].r), c[2].y + c[2].r);
  } else {
    x_min = std::min(std::min(c[0].x - c[0].r, c[1].x - c[1].r), c[2].x - c[2].r);
    x_max = std::max(std::max(c[0].x + c[0].r, c[1].x + c[1].r), c[2].x + c[2].r);
    y_min = std::min(std::min(c[0].y - c[0].r, c[1].y - c[1].r), c[2].y - c[2].r);
    y_max = std::max(std::max(c[0].y + c[0].r, c[1].y + c[1].r), c[2].y + c[2].r);
  }

  if (x_max <= x_min || y_max <= y_min) {
    return 0.0;
  }

  const double s_rec = (x_max - x_min) * (y_max - y_min);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_real_distribution dist_x(x_min, x_max);
  std::uniform_real_distribution dist_y(y_min, y_max);

  std::int64_t hits = 0;

  for (std::int64_t i = 0; i < n_points; ++i) {
    const double x = dist_x(gen);
    const double y = dist_y(gen);

    bool inside = true;
    for (int k = 0; k < 3; ++k) {
      const double dx = x - c[k].x;
      const double dy = y - c[k].y;
      if (dx * dx + dy * dy > c[k].r * c[k].r) {
        inside = false;
        break;
      }
    }
    if (inside) {
      ++hits;
    }
  }

  const double area_est = static_cast<double>(hits) / static_cast<double>(n_points) * s_rec;

  return area_est;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::array<Circle, 3> circles;
  circles[0] = Circle{1, 1, 1};
  circles[1] = Circle{1.5, 2, std::sqrt(5) / 2};
  circles[2] = Circle{2, 1.5, std::sqrt(5) / 2};

  const double s_exact = 0.25 * M_PI + 1.25 * std::asin(0.8) - 1.0;

  std::cout << std::fixed << std::setprecision(6);

  std::cout << "N"
            << "\t"
            << "Area_wide"
            << "\t"
            << "Err_wide_%"
            << "\t"
            << "Area_narrow"
            << "\t"
            << "Err_narrow_%"
            << "\n";

  for (int n = 100; n <= 100000; n += 500) {
    const double area_wide = MonteCarloIntersectionArea(circles, false, n);
    const double area_narrow = MonteCarloIntersectionArea(circles, true, n);

    const double err_wide = std::abs(area_wide - s_exact) / s_exact * 100.0;
    const double err_narrow = std::abs(area_narrow - s_exact) / s_exact * 100.0;

    std::cout << n << "\t" << area_wide << "\t" << err_wide << "\t" << area_narrow << "\t" << err_narrow << "\n";
  }

  return 0;
}