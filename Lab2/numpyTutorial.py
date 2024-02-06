
import matplotlib
import matplotlib.pyplot as plt
import scipy as scp
import numpy as np
import cv2
#Secion 3.2
print(np.ones((3,4)))
print(np.zeros((4,3)))
A = np.array([[1,2],[1,2],[1,2]])
x = np.array([[1,2,3],[1,2,3]])
B = np.matmul(A,x)
print(B)
V = np.array([[3,1],[1,2]])
print(np.linalg.eig(V))
