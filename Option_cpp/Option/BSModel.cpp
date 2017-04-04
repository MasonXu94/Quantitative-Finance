//
//  BSModel.cpp
//  Option
//
//  Created by Mason's Mac on 16/10/17.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Black Scholes Model to estimate element in Binary Pricing Model

#include "BSModel.hpp"
#include "cmath"
#include "iostream"
using namespace std;

BinModel BSModel::ApproxBinModel(double t){
    
    double U = exp((r-sigma*sigma/2)*t+sigma*sqrt(t))-1;
    double D = exp((r-sigma*sigma/2)*t-sigma*sqrt(t))-1;
    double R = exp(r*t)-1;
    BinModel Approx(S0, U, D, R);
    return Approx;
}