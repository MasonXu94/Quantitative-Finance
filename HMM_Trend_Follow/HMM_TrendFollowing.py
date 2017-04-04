from hmmlearn.hmm import GaussianHMM
import quandl
import numpy as np
from matplotlib import cm, pyplot as plt
import matplotlib.dates as dates
import pandas as pd
import datetime
import seaborn as sns
quandl.ApiConfig.api_key = "4iCU9PM6Ubz4x-9xYr-1"
import pykalman as km
import tushare as ts



beginDate = '2000-01-01'
endDate = '2016-12-31'
num_states = 5
#n个隐藏状态

spy = quandl.get("GOOG/NYSE_SPY", start_date=beginDate,
                  end_date=endDate, collapse = "daily", authtoken="4iCU9PM6Ubz4x-9xYr-1")

# Fill missing Nan in high/low price
spy = spy.fillna(method="backfill")
spy.to_csv("spy.csv")
#print(df.head())

##############################################
####### Performance Measure Calculation ######
##############################################
def performance_measure(rtn, rtn_m, date,freq):
    """
    :param rtn: rtn of portfolio
    :param rtn_m: rtn of market
    :param date: date list
    :param freq: 252=daily, 12=month, 1=year
    :return final_pl: final value of portfolio
    :return batting_average: winning probability
    :return sharp_ratio: excess return adjusted by risk of portfolio over standard deviation
    :return info_ratio: excess return adjusted by benchmark return over hedged standard deviation
    :return max_drawdown: max drop percentage during investment periods
    """
    pl = np.array(rtn.cumprod())
    pl_mkt = np.array(rtn_m.cumprod())

    plt.figure(figsize=(10, 5))
    plt.plot(date, pl, label='Backtest result', lw=2)
    plt.plot(date, pl_mkt, label='Market trend', lw=2)
    plt.ylabel("Value")
    plt.xlabel("Date")
    plt.title("Backtest Result")
    plt.legend()
    plt.show()

    # Calculate final_pl, sharp_ratio, info_ratio
    final_pl = pl[-1]
    sharp_ratio = (rtn.mean() - 1) / rtn.std() * np.sqrt(freq)
    sharp_ratio_spy = (rtn_m.mean() - 1) / rtn_m.std() * np.sqrt(freq)
    info_ratio = (rtn.mean() - rtn_m.mean()) * np.sqrt(freq) / (rtn - rtn_m).std()

    # Calculate batting_average
    w_l = (rtn - rtn_m) > 0
    l_l = (rtn - rtn_m) < 0
    win = 0
    lose = 0
    for i in w_l:
        if i == True:
            win += 1
    for j in l_l:
        if j == True:
            lose +=1
    batting_average = win / (win+lose)

    # Calculate MaxDrawdown
    high = 1
    max_drawdown = 0
    for x in pl:
        if x > high:
            high = x
        if max_drawdown < high/x - 1:
            max_drawdown = high/x - 1
    print("Final P&L: ",final_pl)
    print("Sharp Ratio: ",sharp_ratio)
    print("SPY Sharp Ration: ", sharp_ratio_spy)
    print("Info Ratio: ",info_ratio)
    print("Batting Average: ",batting_average)
    print("Max Drawdown: ",max_drawdown)



    return final_pl, batting_average, sharp_ratio, info_ratio, max_drawdown


##############################################
##############################################
##############################################

close = spy['Close'][:-1]
date_list = pd.to_datetime(close.index[5:])
log_rtn = (np.log(np.array(close[1:])) - np.log(np.array(close[:-1])))[4:]
close_idx = close[5:]

close_next = spy['Close'][1:]
date_list_next = pd.to_datetime(close_next.index[5:])
log_rtn_next = (np.log(np.array(close_next[1:])) - np.log(np.array(close_next[:-1])))[4:]
close_idx_next = close_next[5:]

high = spy['High'][5:-1]
low = spy['Low'][5:-1]
volume = spy['Volume'][5:-1]
log_rtn5 = np.log(np.array(close[5:])) - np.log(np.array(close[:-5]))
diff_hl = np.log(np.array(high))-np.log(np.array(low))

observe = [log_rtn5, volume, diff_hl]
print("test")

colors = cm.rainbow(np.linspace(0, 1, num_states))

def get_hidden_states_hmm(num_states, observe,date_list,close_idx):

    X = np.column_stack(observe)
    hmm = GaussianHMM(n_components = num_states, covariance_type='full', n_iter = 5000, algorithm='viterbi').fit(X)
    hidden_states = hmm.predict(X)
    trans_prob = hmm.transmat_[hidden_states[-1]]


    sns.set_style('white')
    # show everyday's state

    plt.figure(figsize = (15, 8))
    for i in range(hmm.n_components):
        state = (hidden_states == i)
        plt.plot(date_list[state],close_idx[state],'.', c= colors[i], label = 'hidden_state %d'%i,lw = 1)#closeidx
        plt.legend()
        plt.grid(1)
    #plt.show()

    return hidden_states


# in-sample
hidden_states = get_hidden_states_hmm(num_states, observe,date_list,close_idx)
data = pd.DataFrame({'Date': date_list[:], 'log_rtn': log_rtn[:], 'H_states': hidden_states[:]}).set_index('Date')

state_final_pl = np.zeros(num_states)
plt.figure(figsize = (10,6))
for i in range(num_states):
    state = ( data.H_states == i)
    idx = np.append(0,state)[1:]
    data['state %d_return'%i] = data.log_rtn.multiply(state,axis=0)
    state_final_pl[i] = np.exp(data['state %d_return' % i].cumsum())[-1]
    plt.plot(np.exp(data['state %d_return' %i].cumsum()),c= colors[i],label='hidden_state %d'%i)
    plt.legend()
    plt.grid(1)
plt.show()
print(state_final_pl)


theta = 0.2
# strategy: buy when bull market, state final pl > theta; short when bear market, state final pl < -theta
buy = np.array([False for x in range(len(hidden_states))])
sell = np.array([False for x in range(len(hidden_states))])
for i in range(num_states):
    if state_final_pl[i] >= 1+theta:
        buy += hidden_states == i
    elif state_final_pl[i] <= 1-theta:
        sell += hidden_states == i

buy = np.append(0,buy[:-1])
sell = np.append(0,sell[:-1])

data['backtest_return'] = data.log_rtn.multiply(buy,axis = 0) \
                            - data.log_rtn.multiply(sell,axis = 0)
data['hedged_return'] = data['backtest_return'] - data['log_rtn']
rtn = np.exp(data['backtest_return'])
rtn_m = np.exp(data['log_rtn'])


performance_measure(rtn,rtn_m,list(data.index),252)


# plot final insample P&L
plt.figure(figsize = (10,6))
plt.plot_date(date_list, np.exp(data['log_rtn'].cumsum()), '-', label='spy return')
plt.plot_date(date_list, np.exp(data['backtest_return'].cumsum()) / np.exp(data['log_rtn'].cumsum()), '-', label='hedging return')
plt.plot_date(date_list, np.exp(data['backtest_return'].cumsum()), '-', label='backtest return')
plt.legend()
plt.grid(1)
plt.show()



"""
# out-sample test1 - simple way
num_predict = 252
sample_size = 2520
num_states = 5
theta = 0.05

high = spy['High'][5:-1]
low = spy['Low'][5:-1]
volume = spy['Volume'][5:-1]
log_rtn5 = np.log(np.array(close[5:])) - np.log(np.array(close[:-5]))
diff_hl = np.log(np.array(high))-np.log(np.array(low))

close_next = spy['Close'][1:]
date_list_next = pd.to_datetime(close_next.index[5:])
log_rtn_next = (np.log(np.array(close_next[1:])) - np.log(np.array(close_next[:-1])))[4:]
close_idx_next = close_next[5:]
rtn_list = []
pos_list = []

for i in range(num_predict,0,-1):
    #print(len(log_rtn5[-i-sample_size:-i]),len(volume[-i-sample_size:-i]),len(diff_hl[-i-sample_size:-i]),len(date_list_next[-i-sample_size:-i]),len(close_idx_next[-i-sample_size:-i]))
    observe = [log_rtn5[-sample_size-i:-i], volume[-sample_size-i:-i]]    #, diff_hl[-1000:-i]
    hidden_states = get_hidden_states_hmm(num_states,observe,date_list_next[-sample_size-i:-i],close_idx_next[-sample_size-i:-i])
    last_state = hidden_states[-1]
    #print(len(hidden_states))
    #print(log_rtn_next[-i])
    data = pd.DataFrame({'Date': date_list_next[-sample_size-i:-i-1], 'log_rtn_next': log_rtn_next[-sample_size-i:-i-1], 'H_states': hidden_states[:-1]}).set_index('Date')
    state_final_pl = np.zeros(num_states)
    #plt.figure(figsize = (10,6))
    print(last_state)
    for j in range(num_states):
        state = (data.H_states == j)
        idx = np.append(0,state)[1:]
        data['state %d_return'%j] = data.log_rtn_next.multiply(idx,axis=0)
        #print(data.head(3))
        state_final_pl[j] = np.exp(data['state %d_return' % j].cumsum())[-1]
        #plt.plot(np.exp(data['state %d_return' %j].cumsum()),label='latent_state %d'%j)
        #plt.legend()
        #plt.grid(1)
    #plt.show()

    if state_final_pl[last_state] >= 1 + theta:
        position = 1
    elif state_final_pl[last_state] <= 1- theta:
        position = -1
    else:
        position = 0
    pos_list.append(position)
    print(log_rtn_next[-i])
    rtn_list.append(position * log_rtn_next[-i])

print(pos_list)
print(rtn_list)
rtn = np.exp(np.array(rtn_list))
rtn_m = np.exp(log_rtn_next[-num_predict:])
pl_table = pd.DataFrame({"rtn_backtest": rtn, "rtn_market":rtn_m, "Date": date_list_next[-num_predict:], "position": pos_list})
print(pl_table.head())
pl_table.to_csv("res_tmp.csv")
performance_measure(pl_table["rtn_backtest"], pl_table["rtn_market"], pl_table["Date"],252)
"""
