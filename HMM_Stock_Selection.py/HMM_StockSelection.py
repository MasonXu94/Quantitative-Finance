"""
Create by Tianyu in 2017.3.3
Apply Hidden Marcov Model on stock selection
"""
from hmmlearn.hmm import GaussianHMM
import quandl
import threading
import numpy as np
import pandas as pd
from time import ctime
from matplotlib import cm, pyplot as plt
quandl.ApiConfig.api_key = "4iCU9PM6Ubz4x-9xYr-1"

# Basic setting:
transaction_cost = 0.001
frequency = "monthly"

##############################################
##### Macro Variable Hidden States Func ######
##############################################
def macro_states_hmm(n_states, date_list, spy_close,spy_vol,spy_hl, cpi, indpro, vix, num_predict):
    """
    :param n_states: Number of hidden states based on assumption
    :param date_list: list of date
    :param spy_close: SPY market performance measure
    :param cpi: CPI inflation measure
    :param indpro: INDPRO industrial production level measure
    :param vix: VIX market volatility measure
    :param num_predict: Back-test or predict period length
    :return: state_table: Macro hidden states table
    :return hidden_state_last: Last or newest hidden state
    :return predict_month: Next month date
    """
    n = n_states
    predict_month = date_list[-num_predict]
    date_list = date_list[12:-num_predict]
    spy_log = (np.log(np.array(spy_close[1:])) - np.log(np.array(spy_close[:-1])))[11:-num_predict]
    spy_vol = spy_vol[12:-num_predict]
    spy_hl = spy_hl[12:-num_predict]
    cpi_d = ((np.array(cpi[12:]) - np.array(cpi[:-12])) / np.array(cpi[:-12]))[:-num_predict]
    indpro_d = ((np.array(indpro[1:]) - np.array(indpro[:-1])) / np.array(indpro[:-1]))[11:-num_predict]
    vix_ = np.array(vix)[12:-num_predict]

    macro = np.column_stack([spy_log,spy_vol,vix_,indpro_d,cpi_d])#spy_hl,
    hmm = GaussianHMM(n_components=n, covariance_type='full', n_iter=5000, algorithm='viterbi').fit(macro)
    hidden_states_macro = hmm.predict(macro)



    ### Separate Model: HMM prediction for macro variables
    hmm_spy = GaussianHMM(n_components=n, covariance_type='full', n_iter=5000, algorithm='viterbi').fit(
        np.column_stack([spy_log,spy_vol,spy_hl]))
    hidden_states_spy = hmm_spy.predict(np.column_stack([spy_log,spy_vol,spy_hl]))

    hmm_cpi = GaussianHMM(n_components=n, covariance_type='diag', n_iter=5000, algorithm='viterbi').fit(
        np.column_stack([cpi_d]))
    hidden_states_cpi = hmm_cpi.predict(np.column_stack([cpi_d]))

    hmm_indpro = GaussianHMM(n_components=n, covariance_type='diag', n_iter=5000, algorithm='viterbi').fit(
        np.column_stack([indpro_d]))
    hidden_states_indpro = hmm_indpro.predict(np.column_stack([indpro_d]))

    hmm_vix = GaussianHMM(n_components=n, covariance_type='diag', n_iter=5000, algorithm='viterbi').fit(
        np.column_stack([vix_]))
    hidden_states_vix = hmm_vix.predict(np.column_stack([vix_]))

    # Plot hidden state
    #plot_states(hmm, hidden_states_macro, date_list, spy_close[12:-num_predict], "MACRO")
    """
    ### plot
    plot_states(hmm_spy, hidden_states_spy, date_list, spy_close[12:-num_predict],"SPY")
    plot_states(hmm_cpi, hidden_states_cpi, date_list, cpi[12:-num_predict], "CPI")
    plot_states(hmm_indpro, hidden_states_indpro, date_list, indpro[12:-num_predict],"IPI")
    plot_states(hmm_vix, hidden_states_vix, date_list, vix[12:-num_predict],"VIX")
    """

    # Aggregate four macro states
    hidden_state = []
    for i in range(len(hidden_states_cpi)):
        hidden_state.append(str(hidden_states_cpi[i]) + str(hidden_states_indpro[i]) + str(hidden_states_spy[i]) + str(
            hidden_states_vix[i]))
    print("date_list: ", len(date_list), "h_s:", len(hidden_state))

    """
    hidden_state = []
    for i in range(len(hidden_states_macro)):
        hidden_state.append(str(hidden_states_macro[i]))
    print("date_list: ", len(date_list), "h_s:", len(hidden_state))
    """
    state_table = pd.DataFrame({"Y_month": get_year_month(date_list), "H_states": hidden_state})
    hidden_state_last = hidden_state[-1]
    print(state_table.head())
    print(hidden_state_last)
    return state_table, hidden_state_last, predict_month


##############################################
### Construct Portfolio & Calculate Return ###
##############################################
def portfolio_return(stock_table, state_table, hidden_state_last, predict_month):
    """
    :param stock_table: Monthly data of stocks in SP500
    :param state_table: Macro hidden states table
    :param hidden_state_last: Last or newest hidden state
    :param predict_month: Next month date
    :return: date: The Next month date
    :return: port_rtn: The return of selected portfolio
    """
    # Return of stocks in the following month
    rtn_next_table = stock_table[stock_table["Date"] == predict_month]

    # Merge stock table and hidden states table
    stock_table = pd.merge(stock_table, state_table, on='Y_month', sort=False)
    stock_table = stock_table.sort_values(["H_states", "Code", "Y_month"])
    stock_table["Cum_rtn"] = stock_table.groupby(['H_states', 'Code'])['Log_rtn'].agg(np.cumsum)

    # Compare and Rank stocks in each state-combinations
    table = pd.DataFrame(stock_table.groupby(['H_states', 'Code'])['Cum_rtn'].last())
    ### Alternative: table = stock_table0.groupby(['H_states','Code'])['Log_rtn'].sum()
    table["index"] = table.index
    index = np.array(table["index"])
    h_states_id = []
    code_id = []
    for x in table["index"]:
        h_states_id.append(x[0])
        code_id.append(x[1])
    table["H_states"], table["Code"] = h_states_id, code_id
    table = table.sort_values(["H_states", "Cum_rtn"], ascending=False)

    # Select top 50 stocks with best performance in the historical period
    port_state = table.groupby(["H_states"]).head(50)
    # num_state = int(len(port_state) / 50)
    port_pool = pd.DataFrame()
    port = []
    for i in range(len(port_state)):
        if i == 0:
            state = port_state["H_states"][i]
        if state != port_state["H_states"][i]:
            # print(str(state))
            # print(len(np.array(port)))
            port_pool[str(state)] = np.array(port)
            state = state = port_state["H_states"][i]
            port = [port_state["Code"][i]]
        elif i == len(port_state) - 1:
            port.append(port_state["Code"][i])
            port_pool[str(state)] = np.array(port)
        else:
            port.append(port_state["Code"][i])

    # Get Target portfolio
    target_stock = pd.DataFrame({"Code": port_pool[str(hidden_state_last)]})

    port_pool.to_csv("state_port.csv")

    # Next month stock return
    target_stock = pd.merge(target_stock, rtn_next_table, on='Code', sort=False)
    print(target_stock.head())
    port_rtn = target_stock["Return"].mean()
    date = target_stock["Date"][0]
    return date, port_rtn


##############################################
####### Performance Measure Calculation ######
##############################################
def performance_measure(rtn, rtn_m, date, freq):
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


    # Calculate final_pl, sharp_ratio, info_ratio
    final_pl = pl[-1]
    sharp_ratio = (rtn.mean() - 1) / rtn.std() * np.sqrt(freq)
    sharp_ratio_spy = (rtn_m.mean() - 1) / rtn_m.std() * np.sqrt(freq)
    info_ratio = (rtn.mean() - rtn_m.mean()) * np.sqrt(freq) / (rtn - rtn_m).std()

    # Calculate batting_average
    w_l_hedg = (rtn-rtn_m)>=0
    #print(w_l)
    win = 0
    win_m = 0
    win_hedg = 0
    pl_w = 0
    pl_l = 0
    pl_m_w = 0
    pl_m_l = 0
    for i in range(len(w_l_hedg)):
        if w_l_hedg[i] == True:
            win_hedg += 1
    batting_average = win_hedg/w_l_hedg.count()

    for i in range(len(rtn)):
        if rtn[i] >=1:
            win += 1
            pl_w += rtn[i]-1
        else:
            pl_l += rtn[i]-1
    hit_ratio = win / rtn.count()

    for i in range(len(rtn_m)):
        if rtn_m[i] >=1:
            win_m += 1
            pl_m_w +=rtn_m[i]-1
        else:
            pl_m_l += rtn_m[i]-1
    hit_ratio_m = win_m/rtn_m.count()
    # Calculate MaxDrawdown
    high = 1
    max_drawdown = 0
    for x in pl:
        if x > high:
            high = x
        if max_drawdown < high/x - 1:
            max_drawdown = high/x - 1

    print("Final P&L: ",final_pl)
    print("Final P&L of SPY: ", pl_mkt[-1])
    print("Sharp Ratio: ",sharp_ratio)
    print("SPY Sharp Ration: ", sharp_ratio_spy)
    print("Info Ratio: ",info_ratio)
    print("Batting Average: ",batting_average)
    print("Hit Ratio: ", hit_ratio)
    print("Hit Ratio of SPY: ", hit_ratio_m)
    print("Max Drawdown: ",max_drawdown)
    print("Expected Win: ", pl_w / win)
    print("Expected Loss: ", pl_l / (len(rtn)-win))
    print("Expected Win of SPY: ", pl_m_w / win_m)
    print("Expected Loss of SPY: ", pl_m_l / (len(rtn_m) - win_m))

    plt.figure(figsize=(10, 5))
    plt.plot(date, pl, label='Backtest Return', lw=2)
    plt.plot(date, pl_mkt, label='Market Return', lw=2)
    plt.plot(date, pl-pl_mkt, label='Hedged Return', lw=2)
    plt.ylabel("Value")
    plt.xlabel("Date")
    plt.title("Backtest Result")
    plt.legend()
    plt.show()

    return final_pl, batting_average, sharp_ratio, info_ratio, max_drawdown


##############################################
############# Download Data Func##############
##############################################
### Download Macro Variable Data
def get_macro_data(start_date,end_date,freq):
    spy = quandl.get("GOOG/NYSE_SPY", start_date=start_date, end_date=end_date, collapse=freq)
    cpi = quandl.get("RATEINF/CPI_USA", start_date=start_date, end_date=end_date, collapse=freq)
    indpro = quandl.get("FRED/INDPRO", start_date=start_date, end_date=end_date, collapse=freq)

    vix = quandl.get("CBOE/VIX", start_date=start_date, end_date=end_date, collapse=freq)
    spy.to_csv("spy.csv")
    cpi.to_csv("cpi.csv")
    indpro.to_csv("indpro.csv")
    vix.to_csv("vix.csv")
    return spy, cpi, indpro, vix
"""
### Get Macro variable data
spy = pd.read_csv("spy.csv")
cpi = pd.read_csv("cpi.csv")
indpro = pd.read_csv("indpro.csv")
vix = pd.read_csv("vix.csv")
"""

### Get sp500 stock data
def get_stock_data():
    stock_table = pd.read_csv("SP500_stock_table.csv")
    stock_table.columns = ["Date", "Close", "Vol", "Code", "Month"]
    stock_table = stock_table[["Code", "Month", "Date", "Close", "Vol"]]
    return stock_table
### Or get stock data from quandl
"""
### Download stock data
start = ctime()
ticker = pd.read_csv("SP500.csv")
print(ticker["free_code"][0])
stock_list = []
for stock_name in ticker["free_code"]:
    stock = quandl.get(stock_name, start_date=start_date, end_date=end_date,collapse=freq)
    stock = stock[['Adj. Close', "Adj. Volume"]]
    stock["Code"] = [stock_name for x in range(len(stock))]
    print(stock_name)
    stock_list.append(stock)
stock_table = pd.concat(stock_list)
stock_table.to_csv('SP500_stock_table.csv')
end = ctime()
print(start)
print(end)
"""


##############################################
############# Data Process Func ##############
##############################################
### Calculate year_month
def get_year_month(date):
    date = pd.to_datetime(date)
    size = len(date)
    y_m = np.zeros(size)
    for i in range(size):
        y_m[i] = str(date[i].year) + str(date[i].month).zfill(2)
    return y_m

### Stock data process
def stock_data_process(stock_table):
    # Add year-month
    stock_table.Date = pd.to_datetime(stock_table.Date)

    stock_table["Y_month"] = get_year_month(stock_table["Date"])

    stock_table['Log_rtn'] = np.log(stock_table.groupby(['Code'])['Close'].diff() /
                                    stock_table.groupby(['Code'])['Close'].shift(periods=1) + 1)
    stock_table['Return'] = stock_table.groupby(['Code'])['Close'].diff() / \
                            stock_table.groupby(['Code'])['Close'].shift(periods=1) + 1
    return stock_table

### Plot_states func
def plot_states(hmm, hidden_states, date_list, y, title):
    print(len(y), len(date_list))
    for i in range(hmm.n_components):
        state = (hidden_states == i)
        plt.plot(date_list[state], y[state], '.', label='state %d' % i, lw=1)
        plt.title ("Hidden states of %s" %title)
        plt.legend()
        plt.grid(1)
    plt.show()
    return 0


##############################################
################# Test Func ##################
##############################################
def test(num_predict, n_states, start_date, end_date, freq):
    """
    :param num_predict: Back-test or predict period length
    :param n_states: Number of hidden states based on assumption
    :param start_date: Beginning date of data
    :param end_date: Ending date of data
    :param freq: Data frequency
    :return: res: Final table with 2 columns of portfolio return and benchmark return
    """
    print("test")
    # Get data
    stock_table = get_stock_data()
    spy, cpi, indpro, vix = get_macro_data(start_date,end_date,freq)
    # Data process
    stock_table = stock_data_process(stock_table)
    spy_close = spy["Close"][:]
    spy_vol = spy["Volume"][:]
    spy_hl = np.log(np.array(spy["High"]))-np.log(np.array(spy["Low"]))
    vix = vix["VIX Close"]

    # Store datetime
    date_list = pd.to_datetime(spy_close.index)

    # Get back-test result by loop every month
    month_list = []
    port_rtn_list = []
    hidden_state = []
    for i in range(num_predict, 0, -1):
        #print(i)
        state_table, hidden_state_last, predict_month = macro_states_hmm(n_states, date_list, spy_close,
                                                                         spy_vol,spy_hl,cpi, indpro,vix, i)
        date, port_rtn = portfolio_return(stock_table, state_table, hidden_state_last, predict_month)
        hidden_state.append(hidden_state_last)
        month_list.append(date)
        port_rtn_list.append(port_rtn)
    res = pd.DataFrame({"Date": month_list, "Rtn_portfolio": port_rtn_list, "H_states": hidden_state})
    spy_rtn = (np.array(spy_close[1:]) / np.array(spy_close[:-1]))[-num_predict:]
    res["Rtn_SPY"] = spy_rtn
    return res


##############################################
##############################################
##############################################
### Set basic config
start_date = "2004-01-01"
end_date = "2016-12-31"
freq = "monthly"
n_states = 3
num_predict = 36

### Test
res = test(num_predict, n_states,start_date,end_date,freq)
print(res)
final_pl, batting_average, sharp_ratio, info_ratio, max_drawdown \
    = performance_measure(res["Rtn_portfolio"], res["Rtn_SPY"],res["Date"],12)
performance = pd.DataFrame({"final_pl": [final_pl], "batting_average":[batting_average],
                            "sharp_ratio": [sharp_ratio], "info_ratio": [info_ratio],
                            "max_drawdown": [max_drawdown]})
res.to_csv("result_%s.csv" %n_states)
performance.to_csv("performance_%s.csv" %n_states)
