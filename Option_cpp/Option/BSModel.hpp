//
//  BSModel.hpp
//  Option
//
//  Created by Mason's Mac on 16/10/17.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Black Scholes Model to estimate element in Binary Pricing Model 

#ifndef BSModel_hpp
#define BSModel_hpp

#include <stdio.h>
#include "BinModel.hpp"
class BSModel{
private:
    double S0;
    double r;
    double sigma;
public:
    BSModel(double S0, double r, double sigma):S0(S0),r(r),sigma(sigma){};
    BinModel ApproxBinModel(double t);
};
#endif /* BSModel_hpp */
