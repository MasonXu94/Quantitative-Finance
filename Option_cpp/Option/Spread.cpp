//
//  Spread.cpp
//  ForthClass
//
//  Created by Mason's Mac on 16/9/25.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#include "Spread.hpp"
#include <iostream>
using namespace std;

int BullSpread:: GetInputData(){
    cout << "Enter Bull Spread data:" << endl;
   	int N;
   	cout << "Enter steps to expiry N: "; cin >> N;
   	SetN(N);
   	cout << "Enter parameter K1:      "; cin >> K1;
   	cout << "Enter parameter K2:      "; cin >> K2;
   	cout << endl;
   	return 0;
}

double BullSpread:: Payoff(double z){
    if (z <= K1) {
        return 0;
    }
    else if (z > K1 && z < K2) {
        return z-K1;
    }
    else{
        return K2-K1;
    }
}

int BearSpread:: GetInputData(){
    cout << "Enter Bear Spread data:" << endl;
   	int N;
   	cout << "Enter steps to expiry N: "; cin >> N;
   	SetN(N);
   	cout << "Enter parameter K1:      "; cin >> K1;
   	cout << "Enter parameter K2:      "; cin >> K2;
   	cout << endl;
   	return 0;
}

double BearSpread:: Payoff(double z){
    if (z <= K1) {
        return K2-K1;
    }
    else if (z > K1 && z < K2) {
        return K2-z;
    }
    else{
        return 0;
    }
}

int StrangleSpread:: GetInputData(){
    cout << "Enter Strangle Spread data:" << endl;
   	int N;
   	cout << "Enter steps to expiry N: "; cin >> N;
   	SetN(N);
   	cout << "Enter parameter K1:      "; cin >> K1;
   	cout << "Enter parameter K2:      "; cin >> K2;
   	cout << endl;
   	return 0;
}

double StrangleSpread:: Payoff(double z){
    if (z < K1) {
        return K1-z;
    }
    else if (z > K2){
        return z-K2;
    }
    else {
        return 0;
    }
}

int ButterflySpread:: GetInputData(){
    cout << "Enter Butterfly Spread data:" << endl;
   	int N;
   	cout << "Enter steps to expiry N: "; cin >> N;
   	SetN(N);
   	cout << "Enter parameter K1:      "; cin >> K1;
   	cout << "Enter parameter K2:      "; cin >> K2;
   	cout << endl;
   	return 0;
}

double ButterflySpread:: Payoff(double z){
    double K = (K1 + K2) / 2;
    if (z > K1 && z < K) {
        return z - K1;
    }
    if (z < K2 && z > K) {
        return K2 - z;
    }
    else {
        return 0;
    }
}





