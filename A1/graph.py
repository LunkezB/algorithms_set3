import csv
import math
import random

import matplotlib.pyplot as plt


class Circle:
    def __init__(self, x: float, y: float, r: float):
        self.x = x
        self.y = y
        self.r = r


def MonteCarloIntersectionArea(circles, use_narrow_rect: bool, n: int) -> float:
    if use_narrow_rect:
        x_min = max(circles[0].x - circles[0].r,
                    circles[1].x - circles[1].r,
                    circles[2].x - circles[2].r)
        x_max = min(circles[0].x + circles[0].r,
                    circles[1].x + circles[1].r,
                    circles[2].x + circles[2].r)
        y_min = max(circles[0].y - circles[0].r,
                    circles[1].y - circles[1].r,
                    circles[2].y - circles[2].r)
        y_max = min(circles[0].y + circles[0].r,
                    circles[1].y + circles[1].r,
                    circles[2].y + circles[2].r)
    else:
        x_min = min(circles[0].x - circles[0].r,
                    circles[1].x - circles[1].r,
                    circles[2].x - circles[2].r)
        x_max = max(circles[0].x + circles[0].r,
                    circles[1].x + circles[1].r,
                    circles[2].x + circles[2].r)
        y_min = min(circles[0].y - circles[0].r,
                    circles[1].y - circles[1].r,
                    circles[2].y - circles[2].r)
        y_max = max(circles[0].y + circles[0].r,
                    circles[1].y + circles[1].r,
                    circles[2].y + circles[2].r)

    if x_max <= x_min or y_max <= y_min:
        return 0.0

    s_rec = (x_max - x_min) * (y_max - y_min)

    hits = 0
    for _ in range(n):
        x = random.uniform(x_min, x_max)
        y = random.uniform(y_min, y_max)

        inside = True
        for c in circles:
            dx = x - c.x
            dy = y - c.y
            if dx * dx + dy * dy > c.r * c.r:
                inside = False
                break

        if inside:
            hits += 1

    area_est = hits / n * s_rec
    return area_est


def main():
    circles = [
        Circle(1.0, 1.0, 1.0),
        Circle(1.5, 2.0, math.sqrt(5.0) / 2.0),
        Circle(2.0, 1.5, math.sqrt(5.0) / 2.0),
    ]

    s_exact = 0.25 * math.pi + 1.25 * math.asin(0.8) - 1.0
    print("Точная площадь (подсчитанная по формуле):", s_exact)

    n_vals = list(range(100, 100001, 500))

    wide_areas = []
    narrow_areas = []
    wide_err = []
    narrow_err = []

    wide_rows = []
    narrow_rows = []

    for N in n_vals:
        area_wide = MonteCarloIntersectionArea(
            circles, use_narrow_rect=False, n=N)
        area_narrow = MonteCarloIntersectionArea(
            circles, use_narrow_rect=True, n=N)
        err_wide = abs(area_wide - s_exact) / s_exact * 100.0
        err_narrow = abs(area_narrow - s_exact) / s_exact * 100

        wide_areas.append(area_wide)
        narrow_areas.append(area_narrow)

        wide_rows.append({
            "N": N,
            "area_wide": area_wide,
            "wide_err_percent": err_wide
        })

        narrow_rows.append({
            "N": N,
            "area_narrow": area_narrow,
            "narrow_err_percent": err_narrow
        })

        wide_err.append(err_wide)
        narrow_err.append(err_narrow)

    filename_wide = "monte_carlo_wide.csv"
    with open(filename_wide, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=["N", "area_wide", "wide_err_percent"]
        )
        writer.writeheader()
        writer.writerows(wide_rows)
    print(f"Результаты широкого прямоугольника сохранены в: {filename_wide}")

    filename_narrow = "monte_carlo_narrow.csv"
    with open(filename_narrow, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=["N", "area_narrow", "narrow_err_percent"]
        )
        writer.writeheader()
        writer.writerows(narrow_rows)
    print(f"Результаты узкого прямоугольника сохранены в: {filename_narrow}")

    plt.figure()
    plt.plot(n_vals, wide_areas, label="Широкий прямоугольник")
    plt.plot(n_vals, narrow_areas, label="Узкий прямоугольник")
    plt.axhline(s_exact, linestyle="--", label="Точная площадь")
    plt.xlabel("Число точек N")
    plt.ylabel("Приближенная площадь")
    plt.title("Приближенная площадь (Монте-Карло)")
    plt.legend()
    plt.grid(True)

    graph_area_filename = "plot_area.png"
    plt.savefig(graph_area_filename, dpi=1200, bbox_inches="tight")
    print(f"График площади сохранён в {graph_area_filename}")

    plt.show()

    plt.figure()
    plt.plot(n_vals, wide_err, label="Широкий прямоугольник")
    plt.plot(n_vals, narrow_err, label="Узкий прямоугольник")
    plt.xlabel("Число точек N")
    plt.ylabel("Отклонение (%)")
    plt.title("Значение отклонения приближенной площади (Монте-Карло)")
    plt.legend()
    plt.grid(True)

    graph_error_filename = "plot_error.png"
    plt.savefig(graph_error_filename, dpi=1200, bbox_inches="tight")
    print(f"График ошибки сохранён в {graph_error_filename}")

    plt.show()


if __name__ == "__main__":
    main()
