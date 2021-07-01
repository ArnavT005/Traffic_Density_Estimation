import pandas as pd
import matplotlib.pyplot as plt
x1 = ["M3_reduceResolParam", "M3vsBase_moveParam", "M3vsBase_queueParam"]
x2 = ['66x156','82x195','165x390','165x779','329x390','329x779','395x935']
DENSITY_FILEs = [a+b+".csv" for a in x1 for b in x2]
UTILITY_FILEs = ["UtilityVsRuntime.csv"]

plt.rcParams.update({'font.size': 20})

def adjustx(x):
    if x == 66:
        return -2
    if x == 329:
        return 0
    return -1

def adjusty(x):
    if x == 66:
        return -0.5
    return 0.3

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
    ax = df.plot.scatter(y='Runtime', x='Queue Utility')
    plt.ylabel('Runtime (in seconds)')
    plt.xlabel('Queue Utility (in utils)')
    df.apply(lambda r: ax.annotate("("+r['ParameterX'].astype(
        str)[:-2]+","+r['ParameterY'].astype(str)[:-2]+")", (r['Queue Utility']+adjustx(r['ParameterX']), r['Runtime']+adjusty(r['ParameterX'])), fontsize=12), axis=1)
    plt.xlim(0, 100)
    plt.ylim(12, 30)
    plt.xticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
    plt.grid(b=True, which='major', color='#666666', linestyle='-')
    plt.minorticks_on()
    plt.grid(b=True, which='minor', color='#999999',
             linestyle='-', alpha=0.2)
    fig1 = plt.gcf()
    fig1.set_size_inches(15, 10)
    plt.axhline(y=26.80, color='r', linestyle='dashed')
    ax.text(1, 26.9, 'Baseline Runtime', style='italic', fontsize=15,
            bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
    plt.savefig(INPUT_FILE[:-4]+"Queue.jpg")
    ax = df.plot.scatter(y='Runtime', x='Moving Utility')
    plt.ylabel('Runtime (in seconds)')
    plt.xlabel('Moving Utility (in utils)')
    df.apply(lambda r: ax.annotate("("+r['ParameterX'].astype(
        str)[:-2]+","+r['ParameterY'].astype(str)[:-2]+")", (r['Moving Utility']+adjustx(r['ParameterX']), r['Runtime']+adjusty(r['ParameterX'])), fontsize=12), axis=1)
    plt.xlim(0, 100)
    plt.ylim(12, 30)
    plt.xticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
    plt.grid(b=True, which='major', color='#666666', linestyle='-')
    plt.minorticks_on()
    plt.grid(b=True, which='minor', color='#999999',
                 linestyle='-', alpha=0.2)
    fig1 = plt.gcf()
    fig1.set_size_inches(15, 10)
    plt.axhline(y=26.8, color='r', linestyle='dashed')
    ax.text(1, 26.9, 'Baseline Runtime', style='italic', fontsize=15,
            bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
    plt.savefig(INPUT_FILE[:-4]+"Moving.jpg")
