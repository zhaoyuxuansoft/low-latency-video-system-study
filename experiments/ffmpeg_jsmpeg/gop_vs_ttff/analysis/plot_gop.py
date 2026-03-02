import pandas as pd
import matplotlib.pyplot as plt

# 读取数据
df = pd.read_csv("gop_ttff.csv")

# 计算每个 GOP 的 mean 和 std
grouped = df.groupby("gop")["ttff_ms"]
mean = grouped.mean()
std = grouped.std()
median = grouped.median()
p95 = grouped.quantile(0.95)

# 画图
plt.figure()
# plt.errorbar(mean.index, mean.values, yerr=std.values, marker='o')
plt.errorbar(median.index, median.values, yerr=std.values, marker='o')

plt.xlabel("GOP Size")
# plt.ylabel("Mean TTFF (ms)")
plt.ylabel("Median TTFF (ms)")
plt.title("GOP vs TTFF")
plt.grid(True)

plt.savefig("gop_vs_ttff.png", dpi=300)
plt.show()