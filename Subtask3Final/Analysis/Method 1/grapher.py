import pandas as pd
import matplotlib.pyplot as plt
x1 = ["M1_subSampleParam", "M1vsBase_moveParam", "M1vsBase_queueParam"]
x2 = ['2','3','5','7','9','11','13','15']
DENSITY_FILEs = [a + b + ".csv" for a in x1 for b in x2]
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
    ax = df.plot.scatter(y='Runtime', x='Queue Utility')
    plt.ylabel('Runtime (in seconds)')
    plt.xlabel('Queue Utility (in utils)')
    df.apply(lambda r: ax.annotate(r['Parameter'].astype(
        str)[:-2], (r['Queue Utility']-0.8, r['Runtime']+0.25),fontsize = 18), axis=1)
    plt.xlim(0, 100)
    plt.ylim(12, 35)
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
    df.apply(lambda r: ax.annotate(r['Parameter'].astype(str)[
             :-2], (r['Moving Utility']-0.8, r['Runtime']+0.25),fontsize = 18), axis=1)
    plt.xlim(0, 100)
    plt.ylim(12, 35)
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