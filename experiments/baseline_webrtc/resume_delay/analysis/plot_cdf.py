import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# 读取数据
df = pd.read_csv("../raw_data/resume_delay.csv")

# 提取延迟列
rtv = df["rtv_ms"].values

# 排序
rtv_sorted = np.sort(rtv)

# 计算CDF
cdf = np.arange(1, len(rtv_sorted) + 1) / len(rtv_sorted)

# 画图
plt.figure()
plt.plot(rtv_sorted, cdf)

plt.xlabel("Resume Delay (ms)")
plt.ylabel("Cumulative Probability")
plt.title("CDF of Resume-to-Visible Delay (WebRTC Baseline)")
plt.grid(True)

plt.savefig("../results/resume_delay_cdf.png", dpi=300)
plt.show()