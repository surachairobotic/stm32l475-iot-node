import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier
data_in = pd.read_csv('sync_data_in.txt', header = None)
data_out = pd.read_csv('sync_data_out.txt', header = None)
x_train_in = data_in.to_numpy()
x_train_out = data_out.to_numpy()
test_in_raw = pd.read_csv('sync_data_in.txt', header = None)
test_out_raw = pd.read_csv('sync_data_out.txt', header = None)
test_in = data_in.to_numpy()
test_out = data_out.to_numpy()
# y_train = np.zeros(15)
y_train0 = np.zeros(len(x_train_in))
y_train1 = np.ones(len(x_train_out))
y_train = np.append(y_train0,y_train1)
x_train = np.concatenate((x_train_in,x_train_out))
knn = KNeighborsClassifier(n_neighbors=1)#n_neighbors=pick nearest
knn.fit(x_train,y_train)
Y_pred=[]
for i in range(len(test_in)):
    y_pred=(knn.predict(test_in[[i]]))
    Y_pred.append(y_pred[0])

for i in range(len(test_out)):
    y_pred=knn.predict(test_out[[i]])
    Y_pred.append(y_pred[0])

y_test_in=np.zeros(len(test_in))
y_test_out=np.ones(len(test_out))
y_test = np.append(y_test_in,y_test_out)
print(Y_pred)
print ('accuracy = {:.2f}'.format(np.mean(Y_pred==y_test)))