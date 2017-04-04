
###################################################################
### Asian Call Option PDE using Monte Carlo and Variate Control ###
###################################################################

import numpy as np
from scipy.stats import norm

# Basic settings
T=1
r=0.01
S0=110
K=100
sigma=0.2

m=12
n=1000000


# Generate sample paths for Y(Arithmetic Average of S), X(Geometric Average of S)
C_Ariave = np.ones(n)
C_Geoave = np.ones(n)

for i in range(n):
    S_ = S0
    S_Sum = 0
    S_Geosum = 1
    for j in range(m):
        S_ = S_*pow(np.e,(r-0.5*(sigma**2))*T/m + sigma*np.sqrt(T/m)*np.random.normal(0,1))
        S_Sum = S_Sum + S_
        S_Geosum = S_Geosum * S_
    C_Ariave[i] = pow(np.e,-r*T)*max(S_Sum/m-K,0)
    C_Geoave[i] = pow(np.e,-r*T)*max(pow(S_Geosum, 1/m)-K,0)


# Compute Monte Carlo Simulation Result
MeanMC = np.mean(C_Ariave)
VarMC = np.cov(C_Ariave,C_Geoave)[0,0]/n
ErrMC = np.sqrt(VarMC)


# Compute covariance and correlation coefficient of Y and X
Cov = np.cov(C_Ariave,C_Geoave)[0,1]
corr = np.sqrt(np.cov(C_Ariave,C_Geoave)[0,1]**2/np.cov(C_Ariave,C_Geoave)[0,0]/np.cov(C_Ariave,C_Geoave)[1,1])


# Apply Variate Control Method: Y_ = Y_ - b * (X_-E[X])
# where b is optimal and E[X] is BS solution of X
# See more details about formula in Lecture Note 6
b = Cov/np.var(C_Geoave)
T_l = np.linspace(1/m,m/m,m)
i_l = np.linspace(1,2*m-1,m)
T_ave = T_l.mean()

sigma_ave = np.sqrt(sigma**2/(m**2*T_ave)*(i_l*T_l[::-1]).sum())
delta = 0.5*(sigma**2-sigma_ave**2)
d = (np.log(S0/K)+(r-delta+0.5*(sigma_ave**2))*T_ave)/(sigma_ave*(T_ave**0.5))
EX_BS = np.exp(-r*(T-T_ave)-delta*T_ave)*S0*norm.cdf(d) - np.exp(-r*T)*K*norm.cdf(d-sigma_ave*(T_ave**0.5))


# Compute Monte Carlo Simulation with variate control Result
MeanVC = MeanMC - b * (C_Geoave.mean()-EX_BS)
VarVC = (np.cov(C_Ariave,C_Geoave)[0,0] - Cov**2/np.cov(C_Ariave,C_Geoave)[1,1])/n #(VarMC - 2*b*Cov+b**2*np.var(C_Geoave))
print(VarMC*n/(n-1),np.cov(C_Ariave,C_Geoave)[0,0])
ErrVC = np.sqrt(VarVC)



print("n: ",n)
print("b: ",b)
print("Cov: ",Cov)
print("corr: ", corr)
print("MeanMC: ", MeanMC)
print("MeanVC: ",MeanVC)
print("VarMC: ",VarMC)
print("VarVC: ",VarVC)
print("ErrMC: ",ErrMC)
print("ErrVC: ",ErrVC)
print("Geoave: ",C_Geoave.mean())
print("GeoBS: ",EX_BS)


