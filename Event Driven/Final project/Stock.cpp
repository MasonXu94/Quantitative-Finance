//
//  Stock.cpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/11.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#include "Stock.hpp"

void Stock::setMonth(string Month){
    if (Month.size()==1) {
        Month = "0"+Month;
    }
    this->Month=Month;
 }

void Stock::setDay(string Day){
    if (Day.size()==1) {
        this->Day = "0"+Day;
    }
    else this->Day = Day;
}

void Stock::printPrice(){
    vector<double> tmp = getPrice();
    for (int i=0; i<tmp.size(); ++i) {
        cout << tmp[i] << " ";
    }
    cout << endl;
}

void Index::setMonth(vector<string> Month){
    this->MonthVec = Month;
    for (int i=0; i<MonthVec.size(); ++i) {
        cout << MonthVec[i]<< " ";
    }
    cout << "success"<< endl;
}

void Index::setDay(vector<string> Day){
    this->DayVec=Day;
    for (int i=0; i<DayVec.size(); ++i) {
        cout << DayVec[i]<< " ";
    }
    cout << "success"<< endl;
}


