//
//  Option.hpp
//  Option
//
//  Created by Mason's Mac on 16/10/7.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//European option and American Option

#ifndef Option_hpp
#define Option_hpp

#include <stdio.h>
#include "BinModel.hpp"
#include "BinLattice.hpp"

class Option{
private:
    int N;
public:
    virtual double Payoff(double z)=0;
    void SetN(int N);
    int GetN();
};

class EurOption: virtual public Option{
public:
    
    double PriceByCRR(BinModel Model);
    //Euroption Replicate Strategy
    double PriceByCRR(BinModel Model, BinLattice<double> &StockTree, BinLattice<double> &MoneyTree,BinLattice<double> &PriceTree);
};

class AmOption: virtual public Option{
public:
    double PriceByShell(BinModel Model);
    double PriceByShell1(BinModel Model, BinLattice<double> &PriceTree, BinLattice<bool> &StopTree);
    //老师方法
    double PriceByShell0(BinModel Model);
    double PriceByShell2(BinModel Model, BinLattice<double> &PriceTree, BinLattice<bool> &StopTree);

};
#endif /* Option_hpp */
