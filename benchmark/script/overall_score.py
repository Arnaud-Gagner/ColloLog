# TODO: move all .csv and .txt in same folder
# TODO: parse all result files
import os
import pandas
from dataclasses import dataclass

@dataclass
class VersionScore:
    name = ""
    drate = 0.0
    tput = 0.0
    ltency = 0.0

def drop_rate(filepath):
    f = open(filepath)
    content = int(f.read())
    f.close()
    return 1 - content/5000000

def throughput(filepath):
    print(filepath)
    df = pandas.read_csv(filepath, names=["col1", "col2", "col3"], header=None)
    return (df['col2'] / df['col1']).mean()

def latency(filepath):
    df = pandas.read_csv(filepath, names=["col1", "col2", "col3"], header=None)
    return df['col3'].mean()

def individual_scores(main_folder):
    scores = []
    for root, dirs, files in os.walk(main_folder):
        this = VersionScore()
        this.name = os.path.basename(root)
        for file in files:
            if file.endswith("drop_rate.txt"):
                this.drate = drop_rate(os.path.join(root, file))
            if file.endswith("result.csv"):
                this.tput = throughput(os.path.join(root, file))
                this.ltency = latency(os.path.join(root, file))
        if (this.drate > 0):
            scores.append(this)
    return scores
                    
def ranked_scores(scores):
    if not scores:
        return scores

    # normalize throughput
    throughputs = [s.tput for s in scores]
    min_throughput = min(throughputs)
    max_throughput = max(throughputs)

    if max_throughput == min_throughput:
        for s in scores:
            s.tput = 1.0
    else:
        for s in scores:
            s.tput = 1 - (s.tput - min_throughput) / (max_throughput - min_throughput)

    # normalize lantency
    latencies = [s.ltency for s in scores]
    min_latency = min(latencies)
    max_latency = max(latencies)

    if max_latency == min_latency:
        for s in scores:
            s.ltency = 1.0
    else:
        for s in scores:
            s.ltency = 1 - (s.ltency - min_latency) / (max_latency - min_latency)

    return scores

def display_ranking(ranked_scores):
        for i, s in enumerate(ranked_scores, 1):
            print(f"Name: {s.name}")
            print(f"\tDrop Rate: {s.drate}")
            print(f"\tThroughput: {s.tput}")
            print(f"\tLatency: {s.ltency}")
            print(f"\tOverall: {s.drate + 1.5*s.tput + 2*s.ltency}")

if __name__ == "__main__":
    main_folder = "D:/Iteration2"
    scores = []
    scores = individual_scores(main_folder)
    scores = ranked_scores(scores)
    display_ranking(scores)
