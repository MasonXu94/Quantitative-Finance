/*导入股票日均收益和沪深三百*/
data Return;
	set IV.GP_QUOTE;
	keep stock_id dt rtn status;
run;
data Market;
	set IV.Mkt;
run;

/*数据合并*/
proc sql;
	create table total as 
	select a.*, b.mkt_return
	from Return a left join Market b
	on a.dt = b.dt;
quit;

proc sort data = total;
	by dt stock_id ;
run;

/*数据处理*//*月频处理+控制样本区间为2008.1.1-2016.1.31*/
data total;
	set total;
	if mkt_return= . then delete;
	mkt_re = mkt_return/100;
	if dt < 20080101 then delete;
	drop mkt_return;
	rename stock_id =id; 
	if status=0 then delete;
	rtn_e=rtn-0.04/365;
	mkt_re_e = mkt_re-0.04/365;
	dt0=mdy(mod(int(dt/100),100),mod(dt,100),int(dt/10000));
	format dt0 yymmddn.;
	monthday=month(dt0);
	month=abs(intck('month',dt0,'01jan2008'd));
	/*by id;
	if first.id then delete;     剔除股票上市*/
run;

/*用市场模型对每天所有股票截面回归得到残差*/
proc printto print='c:\auto.lst' log='c:\auto.log' new;
run;
proc reg data=total;
	model rtn_e=mkt_re_e/r;
	by dt;
	output out=reg r=residuals;
run;
proc printto print=print log=log;
run;

/*取残差*/
data e;
	set reg;
	keep id dt0 rtn mkt_re residuals month;
	rename residuals = e;
	rename mkt_re = rtnm;
	drop dt;
run;
proc sort data=e;
	by id month;
run;

/*求每家公司每月的特制波动率IV*/
proc summary data=e nway missing;
	class id month;
	var e;
	output out=iv_mth std=iv;
run;

/*求SH300和每家公司每月的收益率*/
data rtn_mth;
	set e;
	by id month;
	retain rtn_m_c 1;
	rtn_m_c = rtn_m_c * (1+rtn);
	if first.id then rtn_m_c = 1+rtn;
	if first.month then rtn_m_c = 1+rtn;
	if first.id then n=1;
	if first.month then n=1;
	if last.id or last.month then i=1;
run;
proc sql;
	create table rtn_mth0 as 
	select * from rtn_mth where i=1;
quit;

/*合并特质波动率和收益率*/
proc sql;
	create table result as 
	select a.* , b._freq_, b.iv
	from rtn_mth0 a left join iv_mth b
	on a.id = b.id and a.month = b.month;
quit;

/*数据处理：去除月数据不足10天构建的IV*/
data result0;
	set result;
	drop n;
	if iv=. then delete;
	if _FREQ_ < 10 then delete;
run;

/*IV滞后一期对应rtn*/
proc sort data = result0;
	by id month;
run;
data result0;
	set result0;
	by id;
	iv_next = lag(iv);
	if first.id then delete;
run;

/*对IV分N组*/
proc sort data = result0;
	by month iv_next;
run;
proc rank data=result0 out=iv_g groups=100;
	var iv_next;
	ranks gp;
	by month;
run;
data iv_g;
	set iv_g;
	gp = gp+1;
	label gp=gp;
run;

/*选股特质波动率最小组*/
proc sql;
	create table port as 
	select * from iv_g where gp = 1
	order by month;
quit;

/*去掉每月首个交易日无法买入的股票（涨停）*/
proc sql;
	create table fail as
	select * from rtn_mth where n=1 and (rtn >0.099 or rtn = 0);
quit;
proc sql;
	create table port0 as
	select a.* , b.n
	from port a left join fail b
	on a.id = b.id and a.month = b.month;
quit;
proc sql;
	create table port1 as
	select * from port0 where n=.;
quit;

/*求组合收益率*/
proc summary data=port1 nway missing;
	class month;
	var rtn_m_c;
	output out=return_p mean=rtn_ave;
run;

data return_p;
	set return_p;
	label rtn_ave=rtn_ave;
	retain rtn_c 1;
	rtn_c = rtn_c * rtn_ave;	
run;

/*HS300市场回报的月度处理和累计收益率*/
data return_m;
	set Market;
	if dt<20080201 then delete;
	if Mkt_Return = . then delete;
	retain mktrtn_c 1;
	mktrtn_c = mktrtn_c * (1+mkt_return/100);
run;
data return_m;
	set return_m;
	dt0=mdy(mod(int(dt/100),100),mod(dt,100),int(dt/10000));
	format dt0 yymmddn.;
	monthday=month(dt0);
	month=abs(intck('month',dt0,'01jan2008'd));
run;
data return_m;
	set return_m;
	by month;
	if last.month then i=1;
run;
proc sql;
	create table return_m0 as 
	select * from return_m where i=1;
quit;

/*合并比较公司累计收益和市场收益*/
proc sql;
	create table exre_test as
	select b.dt, a.*, b.mktrtn_c
	from return_p a left join return_m0 b
	on a.month = b.month;
quit;

/*观察不同组之间的累计收益差距*/
proc sql;
	create table fail as
	select * from rtn_mth where n=1 and rtn >0.099;
quit;
proc sql;
	create table iv_g1 as
	select a.* , b.n
	from iv_g a left join fail b
	on a.id = b.id and a.month = b.month;
quit;
proc sql;
	create table iv_g2 as
	select * from iv_g1 where n=.;
quit;
proc summary data=iv_g2 nway missing;
	class month gp;
	var rtn_m_c;
	output out=compare sum=rtn_s mean=rtn_ave;
run;
proc sort data=compare;
	by gp month;
run;
data compare0;
	set compare;
	by gp;
	retain rtn_c 1;
	rtn_c = rtn_c*rtn_ave;
	if first.gp then rtn_c = rtn_ave;
	if last.gp then i=1;
run;
proc sql;
	create table compare_g as 
	select * from compare0 where i=1;
quit;

/*匹配hs300成份股*/
proc sort data = iv_g;
	by id month;
run;
proc sql;
	create table port_m as 
	select * from iv_g where gp = 1
	order by month;
quit;
proc sort data = port_m;
	by id month;
run;
data port300;
	set CIV.hs300port;
	n=1;
run;
proc sql;
	create table port_match as
	select a.*,b.n
	from port_m a left join port300 b
	on a.id = b.id
	order by month;
quit;
proc sql;
	create table match as
	select id, month from port_match where n = 1;
quit;
proc sql;
	create table match_sum as select distinct
		month,count(id) as cc
	from match
	group by month
	order by month;
quit;
proc sql;
	create table match_sum0 as
	select a.*,b._freq_
	from match_sum a left join exre_test b
	on a.month = b.month
	order by month;
quit;
data match_sum0;
	set match_sum0;
	percent = cc/_freq_;
run;
