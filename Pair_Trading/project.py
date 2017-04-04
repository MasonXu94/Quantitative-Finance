# -*- coding: utf-8 -*-
"""
Created on Mon Oct 17 16:55:14 2016

@author: renguanqi
"""
import pandas as pd
import numpy as np
import statsmodels.formula.api as sm
from statsmodels.tsa.stattools import adfuller
import matplotlib.pyplot as plt
import pykalman as pk
from pandas_datareader.data import DataReader
import quandl
quandl.ApiConfig.api_key = "4iCU9PM6Ubz4x-9xYr-1"

stock_list=['BAC','BK','BBT','C','CMA','FITB','HBAN','JPM','KEY','MTB','PNC','STI','USB','WFC','ZION']

EWA=DataReader('EWA','yahoo',start="20110101",end="20160924")
EWC=DataReader('EWC','yahoo',start="20110101",end="20160924")

df=pd.DataFrame()
df['ewa']=EWA['Adj Close']["2011":"2015"]
df['ewc']=EWC['Adj Close']["2011":"2015"]

model=sm.ols("ewa~ewc",data=df).fit()

adfuller(model.resid)

def adf_pvalue(sticker1,sticker2):
    y=DataReader(sticker1,'yahoo',start="20110101",end="20160924")
    x=DataReader(sticker2,'yahoo',start="20110101",end="20160924")

    df=pd.DataFrame()
    df['y']=y['Adj Close']["2011":"2015"]
    df['x']=x['Adj Close']["2011":"2015"]
    
    model=sm.ols('y~x',data=df).fit()
    
    return adfuller(model.resid)[1]
#%%
p_values=np.zeros([len(stock_list),len(stock_list)])    
for i in range(len(stock_list)):
    for j in range(len(stock_list)):
        if (i!=j):
            p_values[i][j]=adf_pvalue(stock_list[i],stock_list[j])
        else:
            p_values[i][j]=1
#%%
column_labels = stock_list
row_labels = stock_list
data = 1-p_values
fig, ax = plt.subplots()
heatmap = ax.pcolor(data, cmap=plt.cm.Blues)

# put the major ticks at the middle of each cell
ax.set_xticks(np.arange(data.shape[0])+0.5, minor=False)
ax.set_yticks(np.arange(data.shape[1])+0.5, minor=False)

# want a more natural, table-like display
#ax.invert_yaxis()
#ax.xaxis.tick_top()

ax.set_xticklabels(row_labels, minor=False)
ax.set_yticklabels(column_labels, minor=False)
plt.show()
        
#BBT, USB
#%%
ticker_a='BBT'
ticker_b='USB'

ticker_a='C'
ticker_b='JPM'

adf_pvalue(ticker_a,ticker_b)

stock_a=DataReader(ticker_a,'yahoo',start="20060101",end="20160924")
stock_b=DataReader(ticker_b,'yahoo',start="20060101",end="20160924")

df=pd.DataFrame()
df[ticker_a]=stock_a['Adj Close']["2011":"2015"]
df[ticker_b]=stock_b['Adj Close']["2011":"2015"]

model=sm.ols(ticker_a+'~'+ticker_b,data=df).fit()

resid=model.resid
#resid.plot()

ret_combined=np.log((resid-resid.shift(1))/(df[ticker_a]+df[ticker_b])+1)

stdev=resid.std()
position=pd.Series(index=df.index)
position[0]=0.
position[1]=0.

open_threshold=2.
close_threshold=0.
for i in range(2,len(resid)):
    position[i]=position[i-1]
    if (resid[i-2]>open_threshold*stdev and resid[i-1]<=open_threshold*stdev):
        position[i]=-1.
    if (resid[i-1]<close_threshold*stdev and position[i-1]==-1. ):
        position[i]=0.
    if (resid[i-2]<-open_threshold*stdev and resid[i-1]>=-open_threshold*stdev):
        position[i]=1.
    if (resid[i-1]>-close_threshold*stdev and position[i-1]==1.):
        position[i]=0.

ret=ret_combined*position
cum_ret=np.exp(ret.cumsum())
sharpe=ret.mean()/ret.std()*np.sqrt(252)
#%% Out of sample
df=pd.DataFrame()
df[ticker_a]=stock_a['Adj Close']["2016"]
df[ticker_b]=stock_b['Adj Close']["2016"]

resid=df[ticker_a]-model.params[1]*df[ticker_b]
resid=resid-resid.mean()

ret_combined=np.log((resid-resid.shift(1))/(df[ticker_a]+df[ticker_b])+1)

position=pd.Series(index=df.index)
position[0]=0.
position[1]=0.

open_threshold=2.
close_threshold=0.
for i in range(2,len(resid)):
    position[i]=position[i-1]
    if (resid[i-2]>open_threshold*stdev and resid[i-1]<=open_threshold*stdev):
        position[i]=-1.
    if (resid[i-1]<close_threshold*stdev and position[i-1]==-1. ):
        position[i]=0.
    if (resid[i-2]<-open_threshold*stdev and resid[i-1]>=-open_threshold*stdev):
        position[i]=1.
    if (resid[i-1]>-close_threshold*stdev and position[i-1]==1.):
        position[i]=0.

ret=ret_combined*position
cum_ret=np.exp(ret.cumsum())
sharpe=ret.mean()/ret.std()*np.sqrt(252)


#%%Kalman Filter
df=pd.DataFrame()
df[ticker_a]=stock_a['Adj Close']["2012":"2016"]
df[ticker_b]=stock_b['Adj Close']["2012":"2016"]

x=np.vstack([df[ticker_b],np.ones(df[ticker_b].shape)]).T[:, np.newaxis]
y=np.array(df[ticker_a])
delta=0.0001
yhat=np.zeros(len(df[ticker_a]))
e=np.zeros(len(df[ticker_a]))
Q=np.zeros(len(df[ticker_a]))
P=np.zeros((2,2))
beta=np.zeros((len(x),2))
#beta[0]=[.7,df[ticker_a][0]-.7*df[ticker_b][0]]
Vw=delta/(1-delta)*np.eye(2)
Ve=0.001
R=np.zeros((2,2))
for t in range(len(y)):
    if (t>0):
        beta[t]=beta[t-1]
        R=P+Vw
    yhat[t]=x[t].dot(beta[t])
    Q[t]=(x[t].dot(R)*x[t]).sum()+Ve
    e[t]=y[t]-yhat[t]
    K=(x[t].dot(R.T)/Q[t])
    beta[t]+=K[0]*e[t]
    P=R-K.T.dot(x[t]).dot(R)
    
q=pd.DataFrame(np.sqrt(Q),index=df.index)
e=pd.DataFrame(e,index=df.index)

df=df[2:]

df2=q
df2['e']=e
df2.columns=['q','e']
df2=df2.ix[2:]

#df2.plot()

beta=pd.DataFrame(beta[2:],index=df2.index)
beta.columns=['beta','intercept']
#beta.plot()

weight=beta[['intercept','beta']].shift(1)
weight.columns=[ticker_a,ticker_b]
weight[ticker_a]=1.
weight[ticker_b]=-weight[ticker_b]
weight=weight*df
weight[ticker_a]=weight[ticker_a]/abs(weight).sum(axis=1)
weight[ticker_b]=1-weight[ticker_a]

position=pd.Series(index=df.index)
position[0]=0.
position[1]=0.

open_threshold=2.0
close_threshold=2.0

resid=df2.e
stdev=df2.q

open_date=[]
close_date=[]
for i in range(1,len(df2)):
    position[i]=position[i-1]
    if (df2.e[i-1]<close_threshold*df2.q[i-1] and position[i-1]==-1. ):
        position[i]=0.
        close_date.append(i)    
    if (df2.e[i-1]>-close_threshold*df2.q[i-1] and position[i-1]==1.):
        position[i]=0.
        close_date.append(i)
    #if (position[i-1]!=0):
    #    position[i]=0.
    #    close_date.append(i)
    if (df2.e[i-1]>open_threshold*df2.q[i-1] and position[i-1]!=-1):
        position[i]=-1.
        open_date.append(i)
    if (df2.e[i-1]<-open_threshold*df2.q[i-1] and position[i-1]!=1):
        position[i]=1.
        open_date.append(i)

open_date=np.array(open_date)
close_date=np.array(close_date)
#holding_period=close_date-open_date

ret_stock=(df/df.shift(1)-1).ix[2:]
ret=np.log((ret_stock*weight).sum(axis=1)+1)*position
cum_ret=np.exp(ret.cumsum())
cum_ret.plot()
sharpe=ret.mean()/ret.std()*np.sqrt(252)

resid=beta[['intercept','beta']]
resid['beta']=df[ticker_a]-beta['beta'].shift(1)*df[ticker_b]
resid.columns=['kf','real']
resid['upper']=df2['q']*open_threshold+resid['kf']
resid['lower']=df2['q']*(-open_threshold)+resid['kf']
#resid["20150201":"2016"].plot()
ax=resid.plot()
ymin,ymax=ax.get_ylim()
ax.vlines(x=df2.index[open_date],ymin=ymin,ymax=ymax,color='r')
ax.vlines(x=df2.index[close_date],ymin=ymin,ymax=ymax,color='b')

