//
//  BinModel.cpp
//  Option
//
//  Created by Mason's Mac on 16/10/6.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//


//Binary Pricing Model
#include "BinModel.hpp"
#include "math.h"
#include <iostream>
using namespace std;

double BinModel::RiskNeutProb(){
    return (R-D)/(U-D);
}

double BinModel:: S(int N, int i){
    return S0*pow(1+U,i)*pow(1+D,N-i);
}

int BinModel:: GetInputData(){
    cout << "S0 = ";
    cin >> S0;
    cout << "U = ";
    cin >> U;
    cout << "D = ";
    cin >> D;
    cout << "R = ";
    cin >> R;
    if (S0<=0.0 || U<=-1.0 || D<=-1.0 || U<=D || R<=-1.0)
    {
        cout << "Illegal data ranges" << endl;
        cout << "Terminating program" << endl;
        return 1;
    }
    if(D>=R||U<=R){
        cout << "Arbitrage exists" << endl;
        cout << "Terminating program" << endl;
        return 1;
    }
    else {
        cout << "Input data checked" << endl;
        cout << "There is no arbitrage" << endl << endl;
        return 0;
    }
}

/*
int BinModel:: GetInputDataBS(){
    double r;
    double V;
    int N;
    double T;
    cout << "S0 = ";
    cin >> S0;
    cout << "Volitility = ";
    cin >> V;
    cout << "r = ";
    cin >> r;
    cout << "T = ";
    cin >> T;
    cout << "N = ";
    cin >> N;
    
    U = exp((r-V*V/2)*T/N+V*sqrt(T/N))-1;
    D = exp((r-V*V/2)*T/N-V*sqrt(T/N))-1;
    R = exp(r*T/N)-1;
    
    if (S0<=0.0 || U<=-1.0 || D<=-1.0 || U<=D || R<=-1.0)
    {
        cout << "Illegal data ranges" << endl;
        cout << "Terminating program" << endl;
        return 1;
    }
    if(D>=R||U<=R){
        cout << "Arbitrage exists" << endl;
        cout << "Terminating program" << endl;
        return 1;
    }
    else {
        cout << "Input data checked" << endl;
        cout << "There is no arbitrage" << endl << endl;
        return 0;
    }
}

*/


