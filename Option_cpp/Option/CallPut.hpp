//
//  CallPut.hpp
//  Option
//
//  Created by Mason's Mac on 16/10/7.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#ifndef CallPut_hpp
#define CallPut_hpp

#include <stdio.h>
#include "Option.hpp"

class Call: public EurOption, public AmOption{
    double K;
public:
    double GetK(){return K;};
    double Payoff(double z);
    int GetInputData();
};

class Put:public EurOption, public AmOption{
    double K;
public:
    double GetK(){return K;};
    double Payoff(double z);
    int GetInputData();
};

#endif /* CallPut_hpp */
