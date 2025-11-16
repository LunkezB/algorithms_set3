#include <cmath>
#include <iostream>
#include <array>
#include <iomanip>
#include <random>

struct Circle {
  double x, y, r;
};

double MonteCarloIntersectionArea(const std::array<Circle, 3>& c, const bool use_narrow_rect) {
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

  constexpr int64_t kTotal = 100000;
  int64_t hits = 0;
  double area_est = 0.0;

  for (int i = 0; i < kTotal; ++i) {
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

  area_est = static_cast<double>(hits) / static_cast<double>(kTotal) * s_rec;

  return area_est;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::array<Circle, 3> circles;
  for (int i = 0; i < 3; ++i) {
    std::cin >> circles[i].x >> circles[i].y >> circles[i].r;
  }

  const double area = MonteCarloIntersectionArea(circles, true);

  std::cout << std::fixed << std::setprecision(19) << area << "\n";
  return 0;
}