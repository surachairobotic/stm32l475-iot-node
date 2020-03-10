import random
from datetime import datetime
import math
import matplotlib.pyplot as plt

def main():
  random.seed(datetime.now())
  t0 = datetime.now()
  
  a = [-63,-63,-63,-55,-64,-69,-60,-63,-62,-52]
  b = [-63,-53,-52,-54,-63,-54,-53,-55,-62,-53,-54,-54,-53,-54,-53,-63,-59,-52,-54,-56,-58,-55,-60,-60]
  c = [-59,-58,-59,-61,-59,-77,-58,-58,-58,-71,-58,-59]
  d = [-63,-67,-67,-65,-63,-68,-65,-68,-64,-60,-63,-60,-63,-60]
  e = [-73,-62,-63,-68,-68,-67,-63,-74,-72,-70,-62,-66,-71,-66,-79,-69,-68,-64,-62,-67,-63]
  
  xa = []
  for i in range(len(a)):
    xa.append(0.5)

  xb = []
  for i in range(len(b)):
    xb.append(1.0)

  xc = []
  for i in range(len(c)):
    xc.append(2.0)

  xd = []
  for i in range(len(d)):
    xd.append(3.0)

  xe = []
  for i in range(len(e)):
    xe.append(4.0)
  
  plt.plot(a, xa, '.', b, xb, '.', c, xc, '.', e, xe, '.', linewidth=0.5)
  plt.show()


if __name__ == '__main__':
  main()
