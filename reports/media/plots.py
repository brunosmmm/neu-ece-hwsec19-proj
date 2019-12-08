"""Generate plots for results."""
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy

TEST_NAMES = [
    "SW/ECB",
    "SW/CBC",
    "MMIO/ECB (auto)",
    "MMIO/ECB (single)",
    "MMIO/CBC",
    "RoCC/ECB",
    "RoCC/CBC",
    "Hybrid/EBC",
]

ROCKET_RESULTS = "../../evaluation/rocket.results"
BOOM_RESULTS = "../../evaluation/boom.results"


def parse_results(data):
    """Parse results."""
    return [(int(d[0]), int(d[1])) for d in [d.split(",") for d in data]]


if __name__ == "__main__":
    plt.style.use("bmh")

    # load results
    with open(ROCKET_RESULTS, "r") as rocketdata:
        rocket = parse_results(rocketdata.readlines())

    with open(BOOM_RESULTS, "r") as boomdata:
        boom = parse_results(boomdata.readlines())

    if len(boom) != len(rocket) or len(boom) < len(TEST_NAMES):
        print("ERROR: incorrect result formatting")
        exit(1)

    # get slowest (more cycles) for normalization purposes
    rocket_cycles = [data[0] for data in rocket]
    rocket_insts = [data[1] for data in rocket]
    boom_cycles = [data[0] for data in boom]
    boom_insts = [data[1] for data in boom]
    slowest_ebc, _ = rocket[0]
    slowest_cbc, _ = rocket[1]

    # horizontal bar plots
    # plot 1 is ECB mode cycle count normalized to Rocket
    fig, ax = plt.subplots()
    ecb_benchmarks = [
        "2-way\nBOOM",
        "MMIO\n(auto)",
        "MMIO\n(single)",
        "RoCC",
        "Hybrid\n(batch)",
    ]
    ecb_cycles = [
        boom_cycles[0],
        rocket_cycles[2],
        rocket_cycles[3],
        rocket_cycles[5],
        rocket_cycles[7],
    ]
    ecb_speedup = [slowest_ebc / cycles for cycles in ecb_cycles]

    ecb_data_sorted = sorted(zip(ecb_speedup, ecb_benchmarks))
    ecb_sorted_names = [x for _, x in ecb_data_sorted]
    ecb_sorted_speedup = [x for x, _ in ecb_data_sorted]

    y_pos = numpy.arange(len(ecb_benchmarks))
    ax.barh(
        y_pos,
        ecb_sorted_speedup,
        align="center",
        color="steelblue",
        height=0.5,
        edgecolor="black",
    )
    ax.set_yticks(y_pos)
    ax.set_xticks(numpy.arange(0, 6.5, 0.5))
    ax.set_yticklabels(ecb_sorted_names)
    ax.set_xlabel("Speedup")
    ax.set_title(
        "Speedup compared to pure software in-order implementation\n(ECB mode)"
    )
    ax.grid(axis="y", b=False)

    plt.savefig("ecb_speedup.pdf")

    fig, ax = plt.subplots()
    cbc_benchmarks = ["2-way\nBOOM", "MMIO\n(single)", "RoCC"]
    cbc_cycles = [boom_cycles[1], rocket_cycles[4], rocket_cycles[6]]
    cbc_speedup = [slowest_cbc / cycles for cycles in cbc_cycles]

    cbc_data_sorted = sorted(zip(cbc_speedup, cbc_benchmarks))
    cbc_sorted_names = [x for _, x in cbc_data_sorted]
    cbc_sorted_speedup = [x for x, _ in cbc_data_sorted]

    y_pos = numpy.arange(len(cbc_benchmarks))
    ax.barh(
        y_pos,
        cbc_sorted_speedup,
        align="center",
        color="steelblue",
        height=0.5,
        edgecolor="black",
    )
    ax.set_yticks(y_pos)
    ax.set_yticklabels(cbc_sorted_names)
    ax.set_xlabel("Speedup")
    ax.set_title(
        "Speedup compared to pure software in-order implementation\n(CBC mode)"
    )
    ax.grid(axis="y", b=False)

    plt.savefig("cbc_speedup.pdf")

    # scatter plot
    fig, ax = plt.subplots()

    for cycle, inst, name in zip(rocket_cycles, rocket_insts, TEST_NAMES):
        ax.plot(cycle, inst, "o", label=name)
    ax.xaxis.set_major_formatter(ticker.EngFormatter())
    ax.yaxis.set_major_formatter(ticker.EngFormatter())
    ax.legend()
    ax.set_xlabel("Cycles")
    ax.set_ylabel("Instructions retired")
    ax.set_title(
        "Instructions retired vs Cycles for benchmarks on\nin-order core"
    )
    plt.savefig("scatter_results.pdf")
