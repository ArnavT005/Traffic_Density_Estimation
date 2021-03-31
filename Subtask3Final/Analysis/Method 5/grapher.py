import pandas as pd
import matplotlib.pyplot as plt

x1 = ['M5_temporalSplitParam', 'M5vsBase_moveParam', 'M5vsBase_queueParam']
x2 = [str(x) for x in range(1, 17)]
DENSITY_FILEs = [a+b+".csv" for a in x1 for b in x2]
UTILITY_FILEs = ["UtilityVsRuntime.csv"]

plt.rcParams.update({'font.size': 20})

for INPUT_FILE in DENSITY_FILEs:
    df = pd.read_csv(INPUT_FILE)
    df.plot(x='Time')
    plt.ylabel('Density')
    plt.xlabel('Time (in seconds)')
    plt.grid(b=True, which='major', color='#666666', linestyle='-')
    plt.minorticks_on()
    plt.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)
    fig1 = plt.gcf()
    fig1.set_size_inches(20, 10)
    plt.savefig(INPUT_FILE[:-4]+".jpg")

for INPUT_FILE in UTILITY_FILEs:
    df = pd.read_csv(INPUT_FILE)
    ax = df.plot.scatter(y='Queue Utility', x='Parameter')
    plt.ylabel('[Queue/Moving] Utility (in utils)')
    plt.xlabel('Number of pthreads')
    plt.xlim(0, 17)
    plt.ylim(0, 110)
    plt.xticks([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17])
    plt.grid(b=True, which='major', color='#666666', linestyle='-')
    plt.minorticks_on()
    plt.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)
    fig1 = plt.gcf()
    fig1.set_size_inches(15, 10)
    plt.savefig("UtilityVsPthread.jpg")

    ax = df.plot.scatter(y='Runtime', x='Parameter')
    plt.ylabel('Runtime (in seconds)')
    plt.xlabel('No. of pthreads')
    plt.xlim(0, 17)
    plt.ylim(12, 34)
    plt.xticks([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17])
    plt.grid(b=True, which='major', color='#666666', linestyle='-')
    plt.minorticks_on()
    plt.grid(b=True, which='minor', color='#999999', linestyle='-', alpha=0.2)
    fig1 = plt.gcf()
    fig1.set_size_inches(15, 10)
    plt.axhline(y=27.295, color='r', linestyle='dashed')
    ax.text(1, 27.6, 'Baseline Runtime', style='italic', fontsize=15, bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
    plt.savefig("RuntimeVsPthread.jpg")