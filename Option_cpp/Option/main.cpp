//
//  main.cpp
//  Option
//
//  Created by Mason's Mac on 16/10/6.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//
#include "BSModel.hpp"
#include "CallPut.hpp"
#include "DoubDigitOpt.hpp"
#include "Spread.hpp"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Option Pricing" << endl;
    cout << "Which option and method would you like to choose: " << endl;
    
    BinModel Model;
    Model.GetInputData();
    /*
    double S0 = 100;
    double sigma = 0.435;
    double r = 0.01;
    BSModel BS(S0, r, sigma);
    Model = BS.ApproxBinModel(0.00001);       //Give U,D,R by BSModel
    //if (Model.GetInputData()==1) return 1;    //Directly give U, D, R
    cout<<"P= "<< Model.RiskNeutProb()<<endl;
    */
    //ofstream fout;
    //fout.open("Results.txt");

    
    /*
    BinLattice<double> CallPriceTree;
    BinLattice<bool> CallStopTree;
    Call Option1;
    Option1.GetInputData();
    cout << "European call option price = "  << Option1.PriceByCRR(Model) << endl << endl;
    cout << "American call option price = "  << Option1.PriceByShell(Model) << endl << endl;
    cout<<"American call option price1 = "<< Option1.PriceByShell1(Model,CallPriceTree,CallStopTree) << endl;
    CallPriceTree.Display();
    CallStopTree.Display();
    //Other implement of PricebySnell
    //cout << "American call option price0 = "  << Option1.PriceByShell0(Model) << endl << endl;
    //cout << "American call option price1 = "  << Option1.PriceByShell2(Model,PriceTree,StopTree) << endl;
    */
    /*
    BinLattice<double> PutPriceTree;
    BinLattice<bool> PutStopTree;
    Put Option2;
    Option2.GetInputData();
    cout << "European put option price = "  << Option2.PriceByCRR(Model) << endl << endl;
    cout << "American put option price = "  << Option2.PriceByShell(Model) << endl << endl;
    cout << "American put option price0 = "  << Option2.PriceByShell0(Model) << endl << endl;
    cout << "American put option price1 = " << Option2.PriceByShell1(Model,PutPriceTree,PutStopTree) << endl;
    PutPriceTree.Display();
    PutStopTree.Display();
    //老师写法
    //cout << "American put option price0 = "  << Option2.PriceByShell0(Model) << endl << endl;
    //cout << "American put option price1 = "  << Option2.PriceByShell2(Model,PriceTree,StopTree) << endl;
    

    
 
    //各种期权组合
    /*
    BinLattice<double> PriceTree;
    BinLattice<bool> StopTree;
    DoubDigitOpt DoubleDigit;
    DoubleDigit.GetInputData();
    cout << "European double-digital option price = "  << DoubleDigit.PriceByCRR(Model)  << endl << endl;
    cout << "American double-digital option price = "  << DoubleDigit.PriceByShell(Model) << endl << endl;
    cout << "American double-digital option option price1 = " << DoubleDigit.PriceByShell1(Model,PriceTree,StopTree) << endl;
    PriceTree.Display();
    StopTree.Display();
    
    BullSpread BullSpread;
    BullSpread.GetInputData();
    cout << "European Bull Spread option price = "  << BullSpread.PriceByCRR(Model)  << endl << endl;
    cout << "American Bull Spread option price = "  << BullSpread.PriceByShell(Model) << endl << endl;
    cout << "American Bull option option price1 = " << BullSpread.PriceByShell1(Model,PriceTree,StopTree) << endl;
    PriceTree.Display();
    StopTree.Display();
    
    BearSpread BearSpread;
    BearSpread.GetInputData();
    cout << "European Bear Spread option price = "  << BearSpread.PriceByCRR(Model)  << endl << endl;
    cout << "American Bear Spread option price = "  << BearSpread.PriceByShell(Model) << endl << endl;
    cout << "American Bear option option price1 = " << BearSpread.PriceByShell1(Model,PriceTree,StopTree) << endl;
    PriceTree.Display();
    StopTree.Display();
    
    StrangleSpread Strangle;
    Strangle.GetInputData();
    cout << "European Strangle Spread option price = "  << Strangle.PriceByCRR(Model)  << endl << endl;
    cout << "American Strangle Spread option price = "  << Strangle.PriceByShell(Model) << endl << endl;
    cout << "American Strangle option option price1 = " << Strangle.PriceByShell1(Model,PriceTree,StopTree) << endl;
    PriceTree.Display();
    StopTree.Display();
    
    ButterflySpread Butterfly;
    Butterfly.GetInputData();
    cout << "European Butterfly Spread option price = "  << Butterfly.PriceByCRR(Model)  << endl << endl;
    cout << "American Butterfly Spread option price = "  << Butterfly.PriceByShell(Model) << endl << endl;
    cout << "American Butterfly option option price1 = " << Butterfly.PriceByShell1(Model,PriceTree,StopTree) << endl;
    PriceTree.Display();
    StopTree.Display();
    */
    
    
    //复制策略
    
     Call Option1;
     Option1.GetInputData();
     BinLattice<double> PriceTree;
     BinLattice<double> XTree;
     BinLattice<double> YTree;
     
     cout << "European call prices by PriceByCRR:" << Option1.PriceByCRR(Model) << endl << endl;
     
     cout << "European call prices by PriceByCRR(Show Replicating Process):"
     << Option1.PriceByCRR(Model,XTree,YTree,PriceTree)
     << endl << endl;
     PriceTree.Display();
     cout << "Stock positions in replicating strategy:" << endl << endl;
     XTree.Display();
     cout << "Money market account positions in replicating strategy:" << endl << endl;
     YTree.Display();
    

    
    return 0;
}
/*
cout << "S0 = " << Model.GetS0() << endl
<< "U = " << Model.GetU() << endl
<< "D = " << Model.GetD() << endl
<< "R = " << Model.GetR() << endl;

cout << "N = " << Option1.GetN() << endl
<< "K = " << Option1.GetK() << endl;
*/


