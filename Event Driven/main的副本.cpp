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
//#include "Stock_Manage.hpp"
using namespace std;

void print(map<string,Stock> stock_table_total){
    for(map<string,Stock>::const_iterator it = stock_table_total.begin();
        it != stock_table_total.end(); ++it)
    {
        Stock a = it->second;
        cout << it->first << ": "<< a.getGroup() << endl;
        a.printPrice();
    }
}

void print(map<string,vector<double>> stock_table_total){
    for(map<string,vector<double>>::const_iterator it = stock_table_total.begin();
        it != stock_table_total.end(); ++it)
    {
        cout << it->first << endl;
        for (int i = 0; i<it->second.size(); ++i) {
            cout<< it->second[i] <<" ";
        }
        cout << endl;
    }
}

string MonthMinus(string Month){
    string res;
    if (Month == "1") res = "0";
    else if (Month == "01") res = "00";
    else if (Month == "02") res = "01";
    else if (Month == "03") res = "02";
    else if (Month == "04") res = "03";
    else if (Month == "05") res = "04";
    else if (Month == "06") res = "05";
    else if (Month == "07") res = "06";
    else if (Month == "08") res = "07";
    else if (Month == "09") res = "08";
    else if (Month == "10") res = "09";
    else if (Month == "11") res = "10";
    else if (Month == "12") res = "11";
    return res;
}


int write_data(void *ptr, int size, int nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

void *myrealloc(void *ptr, size_t size)
{

    if(ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}


int write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
    
    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

void getCompanyData(ifstream& file, vector<Stock>& stock_table){
    string fline;
    int title = 0;
    while (getline(file, fline)){
        //cout<< fline << endl;
        string key;
        string y;
        string m;
        string d;
        string group;
        int i1 = fline.find_first_of(' ');
        int i2 = fline.find_first_of(',');
        int i3 = fline.find_first_of('/');
        int i4 = fline.find_last_of('/');
        int i5 = fline.find_last_of(',');
        key = fline.substr(0,i1);
        m = fline.substr(i2+1, i3-i2-1);
        d = fline.substr(i3+1, i4-i3-1);
        y = fline.substr(i4+1, i5-i4-1);
        group = fline.substr(fline.find_last_of(',')+1);
        if (title == 1) {
            Stock stock(key,group);
            stock.setMonth(m);
            stock.setDay(d);
            stock_table.push_back(stock);
            //cout << value[2]<<' '<<value[3]<<endl;
        }
        else {title=1;}
    }

}

void GetIndex(Index& SPY, const struct MemoryStruct& data){
    stringstream sData0;
    sData0.str(data.memory);
    string mm;
    string dd;
    double price;
    vector<string> spx_mm;
    vector<string> spx_dd;
    vector<double> spx_price;
    for (string line; getline(sData0, line); ) {
        
        //cout << line<<endl;
        mm = line.substr(5,2);
        dd = line.substr(8,2);
        price = strtod(line.substr(0,line.find_last_of(',')).c_str(), NULL);
        spx_mm.push_back(mm);
        spx_dd.push_back(dd);
        spx_price.push_back(price);
        //cout<< mm<< " "<< dd<< endl;
    }
    //reverse function could be improve
    reverse(spx_mm.begin(),spx_mm.end());
    spx_mm.pop_back();
    reverse(spx_dd.begin(),spx_dd.end());
    spx_dd.pop_back();
    reverse(spx_price.begin(),spx_price.end());
    spx_price.pop_back();
    SPY.setMonth(spx_mm);
    SPY.setDay(spx_dd);
    SPY.setPrice(spx_price);
}





int main()
{
    map<string, Stock> stock_table_1;
    map<string, Stock> stock_table_2;
    map<string, Stock> stock_table_3;
    map<string, Stock> stock_table_total;
    vector<Stock> stock_table;
    
    Index SPY("SPY");
    ifstream file("eps.csv"); // declare file stream:
    getCompanyData(file, stock_table);
    file.close();
    
    struct MemoryStruct data;
    data.memory=NULL;
    data.size = 0;
    //create pointer of curl operation
    CURL *curl;
    curl_global_init(CURL_GLOBAL_ALL);
    //indiactor that can be used to judge if opening url success or not
    CURLcode res;
    
    //init
    curl=curl_easy_init();
    
    if(curl)
    {
        data.memory = NULL;
        data.size=0;
        const char* url0 = "http://real-chart.finance.yahoo.com/table.csv?s=SPY&d=010&e=31&f=2016&g=d&a=04&b=01&c=2016&ignore=.csv";
        curl_easy_setopt(curl, CURLOPT_URL, url0);
        curl_easy_setopt(curl, CURLOPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
        res = curl_easy_perform(curl);
        if ( res != CURLE_OK)
        {
            cout<< "WRONG!!!!"<<endl;
            return 1;
        }
        GetIndex(SPY, data);
        free(data.memory);
        
        for (int j = 0; j < stock_table.size(); ++j) {
            data.memory = NULL;
            data.size=0;
            const char* corp = stock_table[j].getTicker().c_str();
            vector<string> date;
            int flag=0;
            int index = 0;
            for(int i = 0; i<SPY.getSize();++i){
                if(SPY.getMonth()[i]==stock_table[j].getMonth()&&SPY.getDay()[i]==stock_table[j].getDay()){
                    index = i;
                    date.push_back(MonthMinus(SPY.getMonth()[i+30]));
                    date.push_back(SPY.getDay()[i+30]);
                    date.push_back(MonthMinus(SPY.getMonth()[i-30]));
                    date.push_back(SPY.getDay()[i-30]);
                    flag=1;
                    break;
                }
            }
            //cout << index << " ";
            for (int i=index-30; i<index+31; ++i) {
                stock_table[j].setIndexPrice(SPY.getPrice()[i]);
            }
            
            char url[150] =  "http://real-chart.finance.yahoo.com/table.csv?s=" ;
            char rest[51] = "&d=08&e=30&f=2016&g=d&a=05&b=01&c=2016&ignore=.csv";
            if (flag == 1) {
                rest[3] = date[0][0];
                rest[4] = date[0][1];
                rest[8] = date[1][0];
                rest[9] = date[1][1];
                rest[24] = date[2][0];
                rest[25] = date[2][1];
                rest[29] = date[3][0];
                rest[30] = date[3][1];
            }
            
            //cout<< corp;
            strcat(url,corp);
            strcat(url,rest);
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_ENCODING, "");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
            res = curl_easy_perform(curl);
            if ( res != CURLE_OK)
            {
                cout<< "WRONG!!!!"<<endl;
                return 1;
            }
            cout<< corp <<" data from Yahoo completed!" << endl;
            stringstream sData;
            sData.str(data.memory);
            string sValue;
            double dValue = 0;
            vector<double> tmp;
            int lines=0;
            for (string line; getline(sData, line); ) {
                // before  is the day
                sValue = line.substr(line.find_last_of(',')+1);
                // string to double
                dValue = strtod (sValue.c_str(), NULL);
                //cout << sValue << "   " << dValue << endl;
                tmp.push_back(dValue);
                ++lines;
            }
            
            //reverse function could be improve
            reverse(tmp.begin(),tmp.end());
            tmp.pop_back();
            stock_table[j].setPrice(tmp);
            stock_table_total.insert(pair<string, Stock>(corp, stock_table[j]));
            
            //cout << "j=" << j << endl;
            /*
            if(j<40){
                stock_table_1.insert(pair<string, Stock>(corp, stock_table[j]));
            }
            else if(j>=40&&j<80){
                stock_table_2.insert(pair<string, Stock>(corp, stock_table[j]));
            }
            else if(j>=80){
                stock_table_3.insert(pair<string, Stock>(corp, stock_table[j]));
            }
             */
            data.size=0;
            free(data.memory);
        }
        
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    
    
    print(stock_table_total);
    //print(stock_table_1);
    //print(stock_table_2);
    //print(stock_table_3);
    /*
    
    int T=61;int N=120;
    vector<string> Stocks;
    vector<double> Pm;
    map<string,Price> my_map;
    for(map<string,Stock>::const_iterator it = stock_table_total.begin();
        it != stock_table_total.end(); ++it)
    {
        Stock tmp = it->second;
        Stocks.push_back(it->first);
        Pm = tmp.getIndexPrice();
        my_map.insert(pair<string, Price>(it->first, tmp.getPrice()));
        
    }
    Value Object;
    Object.Set(N,T);

    //Pm[0]=0.1;Pm[1]=1.1;Pm[2]=0.2;
    
    ofstream fout;
    fout.open("RRRRR.txt");
    
    Object.DerivR(Stocks,my_map);
    Object.DerivAAR(Pm);
    Object.DerivCAAR();
    for(int i=0; i <N;++i){
        for (int j=0; j<T; ++j) {
            
            cout << Object.GetR(i,j)<< " ";
        }
        cout << endl;
    }
    for(int i=0; i <N;++i){
        for (int j=0; j<T; ++j) {
            
            cout << Object.GetAAR(i,j)<< " ";
        }
        cout << endl;
    }
    for(int i=0; i <N;++i){
        for (int j=0; j<T; ++j) {
            
            cout << Object.GetCAAR(i,j)<< " ";
        }
        cout << endl;
    }
    for(int n=-29;n<(T-30);n++)
    {
        cout<<n<<" ";
        for(int i=0;i<3;i++)
            
            cout<<Object.GetCAAR(i,n+29)<<" ";
        cout<<endl;
    }
    
    Object.Display(fout);
    fout.close();
    */
    
    return 0;
}



