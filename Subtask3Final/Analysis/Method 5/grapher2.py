import pandas as pd
import matplotlib.pyplot as plt
p = [str(x) for x in range(9,17)]
DENSITY_FILEs = ["M5vsBase_queueParam"+x+".csv" for x in p]
UTILITY_FILEs = []

plt.rcParams.update({'font.size': 20})

def apply(x):
    if x == 4:
        return 1
    if x == 5:
        return 2
    if x == 6:
        return 1
    if x == 7:
        return 2
    if x == 8:
        return 3
    return 1

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
        ax = df.plot.scatter(y='Queue Utility', x='Parameter')
        plt.ylabel('[Queue/Moving] Utility (in utils)')
        plt.xlabel('Number of pthreads')
        #df.apply(lambda r: ax.annotate(r['Parameter'].astype(
        #    str)[:-2], (r['Queue Utility']-apply(r['Parameter']), r['Runtime']),fontsize = 15), axis=1)
        plt.xlim(0, 17)
        plt.ylim(0, 110)
        plt.xticks([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17])
        plt.grid(b=True, which='major', color='#666666', linestyle='-')
        plt.minorticks_on()
        plt.grid(b=True, which='minor', color='#999999',
                 linestyle='-', alpha=0.2)
        fig1 = plt.gcf()
        fig1.set_size_inches(15, 10)
        #plt.axhline(y=27.295, color='r', linestyle='dashed')
        #ax.text(1, 27.6, 'Baseline Runtime', style='italic', fontsize=15, bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
        plt.savefig(INPUT_FILE[:-4]+"QueueParam.jpg")
        ax = df.plot.scatter(y='Runtime', x='Moving Utility')
        plt.ylabel('Runtime (in seconds)')
        plt.xlabel('No. of pthreads')
        #df.apply(lambda r: ax.annotate(r['Parameter'].astype(
        #    str)[:-2], (r['Moving Utility']-apply(r['Parameter']), r['Runtime']),fontsize = 15), axis=1)
        plt.xlim(0, 17)
        plt.ylim(0, 110)
        plt.xticks([0,1,2,3,4,5,6,7,8,9])
        plt.grid(b=True, which='major', color='#666666', linestyle='-')
        plt.minorticks_on()
        plt.grid(b=True, which='minor', color='#999999',
                 linestyle='-', alpha=0.2)
        fig1 = plt.gcf()
        fig1.set_size_inches(15, 10)
        plt.axhline(y=27.295, color='r', linestyle='dashed')
        ax.text(1, 27.6, 'Baseline Runtime', style='italic', fontsize=15, bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
        plt.savefig(INPUT_FILE[:-4]+"MovingParam.jpg")
    if len(df.columns) == 5:
        ax = df.plot.scatter(y='Runtime', x='Queue Utility')
        plt.ylabel('Runtime (in seconds)')
        plt.xlabel('Queue Utility (in utils)')
        df.apply(lambda r: ax.annotate("("+r['ParameterX'].astype(
            str)[:-2]+","+r['ParameterY'].astype(str)[:-2]+")", (r['Queue Utility']-0.8, r['Runtime']+0.25),fontsize = 15), axis=1)
        plt.xlim(0, 100)
        plt.ylim(12, 30)
        plt.xticks([0,10,20,30,40,50,60,70,80,90,100])
        plt.grid(b=True, which='major', color='#666666', linestyle='-')
        plt.minorticks_on()
        plt.grid(b=True, which='minor', color='#999999',
                 linestyle='-', alpha=0.2)
        fig1 = plt.gcf()
        fig1.set_size_inches(15, 10)
        plt.axhline(y=27.295, color='r', linestyle='dashed')
        ax.text(1, 27.6, 'Baseline Runtime', style='italic', fontsize=15, bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
        plt.savefig(INPUT_FILE[:-4]+"Queue.jpg")
        ax = df.plot.scatter(y='Runtime', x='Moving Utility')
        plt.ylabel('Runtime (in seconds)')
        plt.xlabel('Moving Utility (in utils)')
        df.apply(lambda r: ax.annotate("("+r['ParameterX'].astype(str)[:-2]+","+r['ParameterY'].astype(
            str)[:-2]+")", (r['Moving Utility'] - 0.8, r['Runtime'] + 0.25),fontsize = 15), axis=1)
        plt.xlim(0, 100)
        plt.ylim(12, 30)
        plt.xticks([0,10,20,30,40,50,60,70,80,90,100])
        plt.grid(b=True, which='major', color='#666666', linestyle='-')
        plt.minorticks_on()
        plt.grid(b=True, which='minor', color='#999999',
                 linestyle='-', alpha=0.2)
        fig1 = plt.gcf()
        fig1.set_size_inches(15, 10)
        plt.axhline(y=27.295, color='r', linestyle='dashed')
        ax.text(1, 27.6, 'Baseline Runtime', style='italic', fontsize=15, bbox={'facecolor': 'red', 'alpha': 0.2, 'pad': 3})
        plt.savefig(INPUT_FILE[:-4]+"Moving.jpg")
