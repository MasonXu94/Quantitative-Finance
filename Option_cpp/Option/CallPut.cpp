//
//  CallPut.cpp
//  Option
//
//  Created by Mason's Mac on 16/10/7.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#include "CallPut.hpp"
#include <iostream>
using namespace std;

double Call:: Payoff(double z){
    return z>K?(z-K):0.0;
}

int Call::GetInputData()
{
    cout << "Enter call option data:" << endl;
    int N;
    cout << "Enter steps to expiry N: "; cin >> N;
    SetN(N);
    cout << "Enter strike price K:    "; cin >> K;
    cout << endl;
    return 0;
}

double Put::Payoff(double z){
    return z<K?(K-z):0.0;
}

int Put::GetInputData()
{
    cout << "Enter Put option data:" << endl;
    int N;
    cout << "Enter steps to expiry N: "; cin >> N;
    SetN(N);
    cout << "Enter strike price K:    "; cin >> K;
    cout << endl;
    return 0;
}
