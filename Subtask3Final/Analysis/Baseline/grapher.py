import pandas as pd
import matplotlib.pyplot as plt
DENSITY_FILEs = ["Baseline.csv"]

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