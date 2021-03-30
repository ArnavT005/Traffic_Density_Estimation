import pandas as pd
import matplotlib.pyplot as plt
p = ['66x156','82x195','165x390','165x779','329x390','329x779','395x935']
DENSITY_FILEs = []
UTILITY_FILEs = ["UtilityVsRuntime.csv"]

plt.rcParams.update({'font.size': 20})

def adjust(x):
    if x == 66:
        return -2
    return -1

def adjust2(x):
    if x == 66:
        return -0.5
    return 0.3

for INPUT_FILE in DENSITY_FILEs:
    df = pd.read_csv(INPUT_FILE)
    #df.plot(x = 'Time', alpha = 0.8)
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
    if len(df.columns) == 4:
        ax = df.plot.scatter(y='Runtime', x='Queue Utility')
        plt.ylabel('Runtime (in seconds)')
        plt.xlabel('Queue Utility (in utils)')
        df.apply(lambda r: ax.annotate(r['Parameter'].astype(
            str)[:-2], (r['Queue Utility']-0.8, r['Runtime']+0.25), fontsize=18), axis=1)
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
        df.apply(lambda r: ax.annotate(r['Parameter'].astype(str)[
                 :-2], (r['Moving Utility']-0.8, r['Runtime']+0.25), fontsize=18), axis=1)
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
        plt.savefig(INPUT_FILE[:-4]+"Moving.jpg")
    if len(df.columns) == 5:
        ax = df.plot.scatter(y='Runtime', x='Queue Utility')
        plt.ylabel('Runtime (in seconds)')
        plt.xlabel('Queue Utility (in utils)')
        df.apply(lambda r: ax.annotate("("+r['ParameterX'].astype(
            str)[:-2]+","+r['ParameterY'].astype(str)[:-2]+")", (r['Queue Utility']+adjust(r['ParameterX']), r['Runtime']+adjust2(r['ParameterX'])), fontsize=12), axis=1)
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
            str)[:-2]+","+r['ParameterY'].astype(str)[:-2]+")", (r['Moving Utility']+adjust(r['ParameterX']), r['Runtime']+adjust2(r['ParameterX'])), fontsize=12), axis=1)
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
