//
//  Stock_Manage.hpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/12.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#ifndef Stock_Manage_hpp
#define Stock_Manage_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <pthread.h>
#include <vector>
#include <curl/curl.h>
#include <Stock.hpp>
using namespace std;

struct MemoryStruct {
    char *memory;
    size_t size;
};


class Stock_Manage{
    struct MemoryStruct data;
    map<string, vector<double>> stock_table_1;
    map<string, vector<double>> stock_table_2;
    map<string, vector<double>> stock_table_3;
    map<string, Stock> stock_table_total;
    vector<Stock> stock_table;
    Index SPY;
    string filename;
public:
    Stock_Manage():SPY("SPY"){data.memory=NULL;data.size=0;};
    string MonthMinus(string Month);
    void getCompanyData(ifstream& file);
    void printStock_Table();
    void print(map<string,Stock> stock_table_total);
    void GetIndex(stringstream &sData0);
    int Download(string filename);
    const vector<Stock>& getStock_Name(){return stock_table;}
    const map<string, Stock>& getStock_Table(){return stock_table_total;}
};


#endif /* Stock_Manage_hpp */
