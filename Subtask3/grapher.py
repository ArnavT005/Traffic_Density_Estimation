DENSITY_FILEs = ["sample.csv"]
UTILITY_FILEs = ["sample.csv"]
import matplotlib.pyplot as plt
import pandas as pd

for INPUT_FILE in DENSITY_FILEs:
    df = pd.read_csv(INPUT_FILE)
    df.plot(x = 'Time')
    plt.savefig(INPUT_FILE[:-4]+".jpg")

for INPUT_FILE in UTILITY_FILEs:
    df = pd.read_csv(INPUT_FILE)
    ax = df.plot.scatter(y = 'RunTime', x = 'QueueUtility')
    df.apply(lambda r: ax.annotate(r['Parameter'].astype(str), (r.QueueUtility + 0.1, r.RunTime + 0.1)), axis=1)
    plt.savefig(INPUT_FILE[:-4]+"Queue.jpg")
    df.plot.scatter(y = 'RunTime', x = 'MovingUtility')
    df.apply(lambda r: ax.annotate(r['Parameter'].astype(str), (r.QueueUtility + 0.1, r.RunTime + 0.1)), axis=1)
    plt.savefig(INPUT_FILE[:-4]+"Moving.jpg")