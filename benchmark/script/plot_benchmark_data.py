import matplotlib.pyplot as plt
from collections import defaultdict
import os
import pandas


# Modified function to return throughput grouped by thread count
def grouped_throughput(filepath):
    df = pandas.read_csv(filepath, names=["threads", "time", "cpu"], header=None)
    df["throughput"] = 1_000_000 * df["threads"] / df["time"]
    grouped = df.groupby("threads")["throughput"].mean().to_dict()
    return grouped


def collect_all_grouped_throughputs(main_folder):
    version_data = defaultdict(dict)  # version_name -> {thread_count -> mean_throughput}
    
    for root, dirs, files in os.walk(main_folder):
        version = os.path.basename(root)
        for file in files:
            if file.endswith("result.csv"):
                full_path = os.path.join(root, file)
                grouped = grouped_throughput(full_path)
                for threads, tput in grouped.items():
                    version_data[version][threads] = tput
    return version_data

def plot_throughput_per_thread(version_data):
    plt.figure(figsize=(8, 5))
    
    for version, thread_data in version_data.items():
        threads = sorted(thread_data.keys())
        throughputs = [thread_data[t] for t in threads]
        plt.plot(threads, throughputs, marker='o', label=version)

    plt.xlabel("Number of Threads")
    plt.ylabel("Mean Throughput (log/ms)")
    plt.title("Mean Throughput per thread")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("throughput_plot.png")

if __name__ == "__main__":
    main_folder = "D:/Iteration2 - Copy"
    version_data = collect_all_grouped_throughputs(main_folder)
    plot_throughput_per_thread(version_data)