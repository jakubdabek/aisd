import matplotlib
import csv
import matplotlib.pyplot as plt
import numpy as np
import itertools

data = {}
with open("log.csv", newline='') as file:
    data_reader = csv.DictReader(file, fieldnames=[
        "data_size",
        "comparisons", "comparisons_stddev",
        "swaps", "swaps_stddev",
        "time", "time_stddev"])
    header_reader = csv.DictReader(file, fieldnames=["sort_type", "repetitions"])
    try:
        while True:
            header = next(header_reader)
            values = []
            data[header["sort_type"]] = values
            for r in itertools.islice(data_reader, int(header["repetitions"])):
                values.append(r)
    except StopIteration:
        pass


# Data for plotting
# t = np.arange(0.0, 2.0, 0.01)
# s = 1 + np.sin(2 * np.pi * t)
#
# comparisons
fig, ax = plt.subplots()
for t, d in data.items():
    xs = np.fromiter(map(lambda x: int(x["data_size"]), d), int, len(d))
    ys = np.fromiter(map(lambda x: float(x["comparisons"]), d), float, len(d))
    ax.loglog(xs, ys, label=t)
    # ax.plot(xs, ys, label=t)

plt.title("comparisons")
plt.legend()
fig.savefig("comparisons.png")
plt.show()

# swaps
fig, ax = plt.subplots()
for t, d in data.items():
    xs = np.fromiter(map(lambda x: int(x["data_size"]), d), int, len(d))
    ys = np.fromiter(map(lambda x: float(x["swaps"]), d), float, len(d))
    ax.loglog(xs, ys, label=t)
    # ax.plot(xs, ys, label=t)

plt.title("swaps")
plt.legend()
plt.show()
fig.savefig("swaps.png")
