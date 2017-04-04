//
//  main.cpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/9.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include "vector"
#include "curl/curl.h"
#include "Stock.hpp"
#include "Value.hpp"
#include "Stock_Manage.hpp"

using namespace std;

int main()
{
    map<string, Stock> stock_table_total;
    vector<Stock> stock_table;
    //int r;
    Stock_Manage stock;
    stock.Download("eps2.csv");
    
    
    stock_table_total = stock.getStock_Table();
    stock_table = stock.getStock_Name();
    
    int T=61;int N=120;
    vector<string> Stocks;
    map<string,Price> Pm;
    map<string,Price> my_map;
    for(int i; i<stock_table.size();++i)
    {
        Stocks.push_back(stock_table[i].getTicker());
    }
    for(map<string,Stock>::const_iterator it = stock_table_total.begin();
        it != stock_table_total.end(); ++it)
    {
        Stock tmp = it->second;
        Pm.insert(pair<string, Price>(it->first, tmp.getIndexPrice()));
        //Pm = tmp.getIndexPrice();//这个函数有点问题！
        my_map.insert(pair<string, Price>(it->first, tmp.getPrice()));
        
    }
    
    
    /*
    for(int i; i<Pm.size();++i)
    {
        cout<<Pm[i]<< " ";
    }*/
    cout<< endl;
    /*
    for(int i; i<stock_table.size();++i)
    {
        cout<<Stocks[i]<< " ";
    }
    */
    
    Value Object;
    Object.Set(N,T);
    
    
    
    ofstream fout;
    fout.open("RRRRR.txt");
    
    Object.DerivR(Stocks,my_map,Pm);
    Object.DerivAAR();
    Object.DerivCAAR();
    
    cout<< endl;
    Object.Display(fout);
    fout.close();
    
    cout<<"1.couttxt"<<endl<<"2.plot"<<endl<<"3.exit"<<endl;
    while (true)
    {
        int o;
        cin>>o;
        if (o==1)
        {
            //cout<< "ahahahaahah"<<endl;
            for(int n=-29;n<(T-30);n++)
            {
                cout<<n<<" ";
                for(int i=0;i<3;i++)
                    
                    cout<<Object.GetCAAR(i,n+29)<<" ";
                cout<<endl;
            }
            //cout<< "ahahahaahah"<<endl;
        }
        //cout<< "ahahahaahah"<<endl;
        cin>>o;
        
        if (o==2)
        {
            //cout<< "ahahahaahah"<<endl;
            FILE* pipe = popen("/usr/local/bin/gnuplot", "w");
            //gnuplot储存的地址
            fprintf(pipe, "set xlabel 'time'\n");
            fprintf(pipe, "set ylabel 'ACCR'\n");
            fprintf(pipe, "set xrange[-29:30]\n");
            fprintf(pipe, "set title 'The figure'\n");
            fprintf(pipe, "plot '/Users/mason/Desktop/Event Driven/Final project/RRRRR.txt' using 1:2 with lines  title 'Group1','/Users/mason/Desktop/Final project/Final project/RRRRR.txt' using 1:3 with lines title 'Group2','/Users/mason/Desktop/Final project/Final project/RRRRR.txt' using 1:4 with lines title 'Group3'\n");
            //这个取决于txt储存地址
            fflush(pipe);
            fprintf(pipe, "pause -1/n");
            //pclose(pipe);
            //return 0;
        }
        /*cin>>o;
        if (o==3) {
            return 0;
        }
        */
        //return 0;
    }
}



