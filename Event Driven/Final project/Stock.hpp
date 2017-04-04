//
//  Stock.hpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/11.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#ifndef Stock_hpp
#define Stock_hpp

#include <stdio.h>
#include "iostream"
#include "vector"
#include <string>
#include <map>
using namespace std;


class Asset{
    string Ticker;
    vector<double> Price;
public:
    Asset(string Ticker):Ticker(Ticker){};
    vector<double> getPrice(){return Price;}
    string getTicker(){return Ticker;}
    void setPrice(vector<double> Price){this->Price=Price;}
};

class Stock:public Asset{
    //string Ticker;
    string Month;
    string Day;
    string Group;
    int size;
    vector<double> IndexPrice;
    //vector<double> Price;
public:
    Stock(string Ticker, string Group):Asset(Ticker),Group(Group){}
    //string getTicker(){return Ticker;}
    string getMonth(){return Month;}
    string getDay(){return Day;}
    string getGroup(){return Group;}
    int getSize(){return size;}
    vector<double> getIndexPrice(){return IndexPrice;};
    //vector<double> getPrice(){return Price;}
    //void setPrice(vector<double> Price){this->Price=Price;}
    void setMonth(string Month);
    void setDay(string Day);
    void setSize(){size = getPrice().size();};
    void setIndexPrice(double IndexPrice){this->IndexPrice.push_back(IndexPrice);};
    void print(){cout << getTicker() << ": " << Month << " " << Day << endl;}
    void printPrice();
};

class Index:public Asset{
    vector<string> MonthVec;
    vector<string> DayVec;
    int size;
public:
    Index(string Ticker): Asset(Ticker){};
    vector<string> getMonth(){return MonthVec;}
    vector<string> getDay(){return DayVec;}
    int getSize(){return size;}
    void setMonth(vector<string> Month);
    void setDay(vector<string> Day);
    void setSize(){size = MonthVec.size();};
};




#endif /* Stock_hpp */
