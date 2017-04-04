//
//  DoubDigitOpt.hpp
//  ForthClass
//
//  Created by Mason's Mac on 16/9/25.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Double Digit Option

#ifndef DoubDigitOpt_hpp
#define DoubDigitOpt_hpp

#include <stdio.h>
#include "Option.hpp"

class DoubDigitOpt: public EurOption,public AmOption
{
private:
    double K1; //parameter 1
    double K2; //parameter 2
public:
    int GetInputData();
    double Payoff(double z);
};

#endif /* DoubDigitOpt_hpp */
