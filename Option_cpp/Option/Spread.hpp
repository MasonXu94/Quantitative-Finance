//
//  Spread.hpp
//  ForthClass
//
//  Created by Mason's Mac on 16/9/25.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Bull, Bear, Strangle, Butterfly Spread Option

#ifndef Spread_hpp
#define Spread_hpp

#include <stdio.h>
#include "Option.hpp"

class BullSpread: public EurOption,public AmOption  //
{
    double K1;
    double K2;
public:
    int GetInputData();
    double Payoff(double z);
};

class BearSpread: public EurOption,public AmOption
{
    double K1;
    double K2;
public:
    int GetInputData();
    double Payoff(double z);
};

class StrangleSpread: public EurOption,public AmOption
{
    double K1;
    double K2;
public:
    int GetInputData();
    double Payoff(double z);
};

class ButterflySpread: public EurOption,public AmOption
{
    double K1;
    double K2;
public:
    int GetInputData();
    double Payoff(double z);
};

#endif /* Spread_hpp */
