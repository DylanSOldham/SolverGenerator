import pandas as pd
from matplotlib import pyplot as plt
import os

os.system('./modgen && make && ./solver > out.csv')

plt.rcParams["figure.figsize"] = [7.00, 3.50]
plt.rcParams["figure.autolayout"] = True

df = pd.read_csv("out.csv")

df.plot(x=df.columns[0])
plt.show()