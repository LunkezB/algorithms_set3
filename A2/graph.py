import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("results.csv")

grouped = (
    df.groupby(["size", "data_type", "algorithm", "threshold"])["time_µs"]
    .median()
    .reset_index()
)

grouped.to_csv("results_agg.csv", index=False)

data_types = grouped["data_type"].unique()

for dt in data_types:
    sub = grouped[grouped["data_type"] == dt]

    std = sub[sub["algorithm"] == "merge_std"]
    hybrids = sub[sub["algorithm"] == "merge_hybrid"]
    thresholds = sorted(hybrids["threshold"].unique())

    std.to_csv(f"results_{dt}_merge_std.csv", index=False)
    hybrids.to_csv(f"results_{dt}_hybrid_all_thresholds.csv", index=False)
    sub.to_csv(f"results_{dt}_combined.csv", index=False)

    plt.figure()

    plt.plot(std["size"], std["time_µs"], label="merge_std", color="blue", linewidth=2)

    plt.title(f"Стандартный MergeSort, тип данных: {dt}")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мкс")
    plt.grid(True)
    plt.legend()

    plt.savefig(f"plot_{dt}_merge_std.png", dpi=600, bbox_inches="tight")
    plt.close()

    plt.figure()

    plt.plot(std["size"], std["time_µs"], label="merge_std", linewidth=2)

    for thr in thresholds:
        part = hybrids[hybrids["threshold"] == thr]
        plt.plot(part["size"], part["time_µs"], label=f"hybrid thr={thr}")

    plt.title(f"MergeSort и Hybrid MergeSort, тип данных: {dt}")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, мкс")
    plt.grid(True)
    plt.legend()

    plt.savefig(f"plot_{dt}_combined.png", dpi=600, bbox_inches="tight")
    plt.close()

print("done")
