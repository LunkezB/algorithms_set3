import pandas as pd

df = pd.read_csv("results.csv")

ranges = {
    "Небольшие": (500, 3000),
    "Средние": (3000, 30000),
    "Большие": (30000, 100000)
}

types = ["nearly_sorted", "reversed", "random"]

df_h = df[df["algorithm"] == "merge_hybrid"]

for rname, (lo, hi) in ranges.items():
    print(f"##  {rname} данные ({lo}-{hi})")

    df_range = df_h[(df_h["size"] >= lo) & (df_h["size"] <= hi)]

    for t in types:
        print(f"\n### Тип данных: {t}\n")
        dft = df_range[df_range["data_type"] == t]

        table = (
            dft.groupby("threshold")["time_µs"]
            .mean()
            .reset_index()
            .sort_values("threshold")
        )

        print("| threshold | Время (µs) |")
        print("|----------:|----------:|")
        for _, row in table.iterrows():
            thr = int(row["threshold"])
            time = round(row["time_µs"], 1)
            print(f"| {thr:<9} | {time:<10} |")

        print()
