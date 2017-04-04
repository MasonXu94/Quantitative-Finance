//
//  main.cpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/9.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//
//  main.cpp
//  scurl
//
//  Created by Yuehan on 11/1/16.
//  Copyright © 2016 Yuehan. All rights reserved.
//
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "vector"
#include "curl/curl.h"
#include <map>
using namespace std;


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
    /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
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

string MonthMinus(string a){
    string res;
    if (a == "1") res = "0";
    else if (a == "01") res = "00";
    else if (a == "02") res = "01";
    else if (a == "03") res = "02";
    else if (a == "04") res = "03";
    else if (a == "05") res = "04";
    else if (a == "06") res = "05";
    else if (a == "07") res = "06";
    else if (a == "08") res = "07";
    else if (a == "09") res = "08";
    else if (a == "10") res = "09";
    else if (a == "11") res = "10";
    else if (a == "12") res = "11";
    return res;
}



int main()
{
    
    map<string, vector<string>> stock_inf;  //Store the
    map<string, vector<double>> stock_price;
    vector<string> Name;
    vector<string> Month;
    vector<string> Day;
    vector<string> Group;
    
    ifstream file("eps.csv"); // declare file stream:
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
            vector<string> value(4);
            value[0] = group;
            value[1] = y;
            if (m.size()==1) {
                value[2] = "0"+m;
            }
            else value[2] = m;
            if (d.size()==1) {
                value[3] = "0"+d;
            }
            else value[3] = d;
            stock_inf.insert(pair<string, vector<string>>(key, value));
            //cout << value[2]<<' '<<value[3]<<endl;
        }
        else {title=1;}
    }
    
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
        const char* url0 = "http://real-chart.finance.yahoo.com/table.csv?s=AAPL&d=010&e=31&f=2016&g=d&a=04&b=01&c=2016&ignore=.csv";       //SP500代码暂时不知道
        curl_easy_setopt(curl, CURLOPT_URL, url0);
        curl_easy_setopt(curl, CURLOPT_ENCODING, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
        res = curl_easy_perform(curl);
        if ( res != CURLE_OK)
        {
            cout<< "WRONG!!!!"<<endl;
            system("pause");
        }
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
        /*for (int i = 0; i<spx_mm.size(); ++i) {
            cout<< spx_mm[i]<< " "<< spx_dd[i]<< " ";
        }
        cout<< endl;
         */
        free(data.memory);
        
        int count =0;
        int count_flag =0;
        for (map<string,vector<string> >::const_iterator it = stock_inf.begin(); it!=stock_inf.end(); ++it) {
            vector<string> date;
            int flag=0;
            for(int i = 0; i<spx_mm.size();++i){
                //cout<<spx_mm[i]<< ' '<<it->second[2]<< ' '<<spx_dd[i]<<' '<<it->second[3]<<endl;
                if(spx_mm[i]==it->second[2]&&spx_dd[i]==it->second[3]){
                    cout<<"i= "<<i<<endl;
                    date.push_back(MonthMinus(spx_mm[i+30]));
                    date.push_back(spx_dd[i+30]);
                    date.push_back(MonthMinus(spx_mm[i-30]));
                    date.push_back(spx_dd[i-30]);
                    flag=1;
                    ++count_flag;
                    break;
                }
            }
            
            data.memory = NULL;
            data.size=0;
            const char* corp = it->first.c_str();
            
            char url[150] =  "http://real-chart.finance.yahoo.com/table.csv?s=" ;
            char rest[51] = "&d=08&e=30&f=2016&g=d&a=05&b=01&c=2016&ignore=.csv";
            if (flag == 1) {
                //cout<<rest[4]<<' '<<date[0][0]<<endl;
                rest[3] = date[0][0];
                rest[4] = date[0][1];
                rest[8] = date[1][0];
                rest[9] = date[1][1];
                rest[24] = date[2][0];
                rest[25] = date[2][1];
                rest[29] = date[3][0];
                rest[30] = date[3][1];
            }
            else{
                return 1;
            }
            
            //cout<<rest<<endl;
            cout<< corp;
            strcat(url,corp);
            strcat(url,rest);
            cout << " " << url<<endl;;
            cout<< count << endl;
            //const char * url2 ="http://real-chart.finance.yahoo.com/table.csv?s=AAPL&d=8&e=30&f=2016&g=d&a=8&b=1&c=2016&ignore=.csv";
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
            //cout << "lines=" << lines << endl;
            //reverse function could be improve
            reverse(tmp.begin(),tmp.end());
            tmp.pop_back();
            cout<< "size = " << tmp.size() <<endl;
            /*for (vector<double>::iterator it=tmp.begin(); it!=tmp.end(); ++it)
                cout << ' ' << *it;*/
            cout<< endl<< endl;
            stock_price.insert(pair<string, vector<double>>(corp, tmp));
            ++count;
            
            data.size=0;
            free(data.memory);
        }
        cout<<count_flag<<endl;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    for(map<string,vector<double> >::const_iterator it = stock_price.begin();
            it != stock_price.end(); ++it)
    {
        cout << it->first << ": "<< it->second.size() << endl;
        
        for (vector<double>::const_iterator it0 = it->second.begin(); it0 != it->second.end();++it0) {
            cout << *it0 << " ";
        }
        cout << endl;
    }
    return 0;
}


