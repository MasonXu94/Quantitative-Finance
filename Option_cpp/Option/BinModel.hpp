//
//  BinModel.hpp
//  Option
//
//  Created by Mason's Mac on 16/10/6.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Binary Pricing Model
#ifndef BinModel_hpp
#define BinModel_hpp

#include <stdio.h>

class BinModel{
private:
    double S0;
    double U;
    double D;
    double R;
public:
    BinModel(): S0(0.0),U(0.0),D(0.0),R(0.0){};
    BinModel(double S0,double U, double D, double R): S0(S0),U(U),D(D),R(R){};
    double RiskNeutProb();
    double S(int N, int i);
    double GetR(){return R;};
    double GetS0(){return S0;};
    double GetU(){return U;};
    double GetD(){return D;};
    int GetInputData();
    //int GetInputDataBS();
};

#endif /* BinModel_hpp */
