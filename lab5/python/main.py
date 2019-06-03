import csv
from collections import defaultdict
from typing import Dict, Tuple, DefaultDict, List

import matplotlib.pyplot as plt
import numpy as np


def main_match():
    data = defaultdict(lambda: defaultdict(list))
    with open("out/match.txt", newline='') as file:
        data_reader = csv.DictReader(file, delimiter=' ', fieldnames=[
            "size",
            "degree",
            "match",
            "time"])
        for r in data_reader:
            data[int(r["size"])][int(r["degree"])].append((float(r["match"]), float(r["time"])))

    graph_data: DefaultDict[int, Dict[int, Tuple[np.ndarray, np.ndarray]]] = defaultdict(dict)

    for size, size_data in data.items():
        for degree, degree_data in size_data.items():
            graph_data[size][degree] = (np.mean(np.array(degree_data), axis=0), np.std(np.array(degree_data), axis=0))

    # pprint(graph_data)

    # fig, ax = plt.subplots()
    for size, size_data in graph_data.items():
        xs = np.arange(1, size + 1, dtype=int)
        ys = np.array([size_data[i][0][0] for i in range(1, size + 1)])
        print(xs, ys)
        plt.locator_params(axis='x', nbins=size)
        plt.plot(xs, ys, label=str(size), marker='o', markersize=4)
        # ax.plot(xs, ys, label=t)

        plt.xlabel("degree")
        plt.ylabel("max match")
        plt.title("max match")
        plt.legend()
        plt.savefig(f"out/plots/match-{size}.png")
        plt.show()

    for i in range(1, 11):
        xs = np.arange(i, 11, dtype=int)
        ys = np.array([graph_data[j][i][0][1] for j in range(i, 11)])
        print(xs, ys)
        plt.locator_params(axis='x', nbins=11-i)
        plt.plot(xs, ys, label=str(i), marker='o', markersize=4)

        plt.xlabel("size")
        plt.ylabel("time [ms]")
        plt.title("max match time")
        plt.legend()
        plt.savefig(f"out/plots/match-time-{i}.png")
        plt.show()

        plt.locator_params(axis='x', nbins=11 - i)
        plt.semilogy(xs, ys, label=str(i), marker='o', markersize=4)

        plt.xlabel("size")
        plt.ylabel("time [ms]")
        plt.title("max match time")
        plt.legend()
        plt.savefig(f"out/plots/match-time-{i}-log.png")
        plt.show()


def main_flow():
    data = defaultdict(list)
    with open("out/flow.txt", newline='') as file:
        data_reader = csv.DictReader(file, delimiter=' ', fieldnames=[
            "size",
            "flow",
            "augmenting paths",
            "time"])
        for r in data_reader:
            data[int(r["size"])].append((float(r["flow"]), float(r["augmenting paths"]), float(r["time"])))

    graph_data: List[Tuple[np.ndarray, np.ndarray]] = []

    for size, size_data in data.items():
        graph_data.append((np.mean(np.array(size_data), axis=0), np.std(np.array(size_data), axis=0)))

    # pprint(graph_data)

    # fig, ax = plt.subplots()

    names = {
        0: ("max flow", "flow"),
        1: ("augmenting pahts", "augmenting"),
        2: ("time", "time")
    }

    for i in range(3):
        xs = np.arange(1, 16 + 1, dtype=int)
        ys = np.array([a[0][i] for a in graph_data])
        print(xs, ys)
        plt.locator_params(axis='x', nbins=16)
        plt.plot(xs, ys, marker='o', markersize=4)
        # ax.plot(xs, ys, label=t)

        label, filename = names[i]
        plt.xlabel("bit length")
        plt.ylabel(label)
        plt.title(label)
        plt.savefig(f"out/plots/flow-{filename}.png")
        plt.show()

        plt.locator_params(axis='x', nbins=16)
        plt.semilogy(xs, ys, marker='o', markersize=4)
        # ax.plot(xs, ys, label=t)

        label, filename = names[i]
        plt.xlabel("bit length")
        plt.ylabel(label)
        plt.title(label)
        plt.savefig(f"out/plots/flow-{filename}-logy.png")
        plt.show()


if __name__ == '__main__':
    # main_flow()
    main_match()
