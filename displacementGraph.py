#Displacement graph heat map

import scipy
from scipy import interpolate
import numpy as np

#IMPORTANT
import matplotlib
matplotlib.use('agg')

import matplotlib.pyplot as plt
import pandas as pd

#command line arguments:
import sys

nameOfFile = "datForPython.csv"
if (len(sys.argv) > 1):
	nameOfFile = sys.argv[1]
outfileName = "fig.pdf"
if (len(sys.argv) > 2):
	outfileName = sys.argv[2]


df = pd.read_csv(str(nameOfFile))

def listDF(string):
    return df[string][:]

#Function mostly from https://stackoverflow.com/questions/39034797/heatmap-for-nonuniformly-spaced-data
def nonuniform_imshow(x, y, z, aspect=1, cmap=plt.cm.CMRmap):
  # Create regular grid
  xi, yi = np.linspace(xMin, xMax, 100), np.linspace(yMin, yMax, 100)
  xi, yi = np.meshgrid(xi, yi)

  # Interpolate missing data
  rbf = scipy.interpolate.Rbf(x, y, z, function='linear')
  zi = rbf(xi, yi)

  _, ax = plt.subplots(figsize=(6, 6))
  
  hm = ax.imshow(zi, interpolation='nearest', cmap=cmap,
                 extent=[xMin, xMax, yMin, yMax], origin = 'lower')
  ax.autoscale(False)
  ax.scatter(x, y, c='purple', alpha = 0.75)
  ax.set_aspect(aspect)
  
  return hm

origA = listDF('origA')
origB = listDF('origB')
defA = listDF('defA')
defB = listDF('defB')
disp = listDF('disp')
xMin, xMax = 0, df['ssA'][0]
yMin, yMax = 0, df['ssB'][0]

#create regular grid
x, y, z = origA, origB, disp
dx, dy = defA, defB
x_, y_, z_ = [], [], []
dx_, dy_ = [], []

i = 0
while(i < len(x)):
    if (x[i] > xMin - 0.1 and x[i] < xMax + 0.1):
        if (y[i] > yMin - 0.1 and y[i] < yMax + 0.1):
            x_.append(x[i])
            y_.append(y[i])
            z_.append(z[i])
    i = i + 1
i = 0
while(i < len(dx)):
    if (dx[i] > xMin - 0.1 and dx[i] < xMax + 0.1):
        if (dy[i] > yMin - 0.1 and dy[i] < yMax + 0.1):
            dx_.append(x[i])
            dy_.append(y[i])
    i = i + 1
    

heatmap = nonuniform_imshow(x_, y_, z_)
heatmap.set_clim([0, 1])
plt.colorbar(heatmap, shrink = 0.63)
plt.xlabel(r'Zig-Zag Position ($\mathring{\rm A}$)')
plt.ylabel(r'Armchair Position($\mathring{\rm A}$)')
plt.tight_layout()
plt.savefig(str(outfileName), dpi=95)


#plt.plot(origA, origB)
#plt.imshow(arr, cmap='viridis')
#plt.colorbar()
