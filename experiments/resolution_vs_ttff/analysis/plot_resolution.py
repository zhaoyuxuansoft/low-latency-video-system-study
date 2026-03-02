import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("resolution_ttff.csv")

plt.figure()
plt.errorbar(df["resolution"], df["mean"], yerr=df["std"], marker='o')
plt.xlabel("Resolution")
plt.ylabel("Mean TTFF (ms)")
plt.title("Resolution vs TTFF")
plt.grid(True)
plt.savefig("resolution_vs_ttff.png")
plt.show()
