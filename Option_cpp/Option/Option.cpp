//
//  Option.cpp
//  Option
//
//  Created by Mason's Mac on 16/10/7.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#include "Option.hpp"
#include <vector>
#include "cmath"
using namespace std;

void Option::SetN(int N){
    this->N = N;
}

int Option::GetN(){
    return this->N;
}

double EurOption::PriceByCRR(BinModel Model){
    double q = Model.RiskNeutProb();
    int N = GetN();
    vector<double> Price(N+1);
    for (int i=0; i<=N; i++)
        Price[i]=Payoff(Model.S(N,i));
    for (int n=N-1; n>=0; n--)
    {
        for (int i=0; i<=n; i++)
            Price[i]=(q*Price[i+1]+(1-q)*Price[i])/(1+Model.GetR());
    }
    return Price[0];
}


double EurOption::PriceByCRR(BinModel Model, BinLattice<double> &StockTree, BinLattice<double> &MoneyTree, BinLattice<double> &PriceTree){
    double q = Model.RiskNeutProb();
    int N = GetN();
    StockTree.SetN(N-1);
    MoneyTree.SetN(N-1);
    PriceTree.SetN(N);
    for (int i=0;i<=N; i++) {
        PriceTree.SetNode(N, i, Payoff(Model.S(N, i)));
    }
    for (int n=N-1; n>=0; n--)
    {  for (int i=0; i<=n; i++){
            PriceTree.SetNode(n, i,(q*PriceTree.GetNode(n+1,i+1) +(1-q)*PriceTree.GetNode(n+1,i))/(1+Model.GetR()));
        }
    }
    for (int n=N-1; n>=0; n--)
    {
        for (int i=0; i<=n; i++){
            StockTree.SetNode(n, i, (PriceTree.GetNode(n+1, i+1)-PriceTree.GetNode(n+1,i))/(Model.S(n+1,i+1)-Model.S(n+1,i)));
            MoneyTree.SetNode(n, i, (PriceTree.GetNode(n+1, i)-StockTree.GetNode(n,i)*Model.S(n+1,i))/pow(1+Model.GetR(),n+1));
        }
    }
    return PriceTree.GetNode(0, 0);
}


double AmOption::PriceByShell(BinModel Model){
    double q = Model.RiskNeutProb();
    int N = GetN();
    vector<double> Price(N+1);
    for (int i=0; i<=N; i++)
        Price[i]=Payoff(Model.S(N,i));
    for (int n=N-1; n>=0; n--)
    {
        for (int i=0; i<=n; i++)
            Price[i]=max((q*Price[i+1]+(1-q)*Price[i])/(1+Model.GetR()),Payoff(Model.S(n, i)));
    }
    return Price[0];
}

double AmOption::PriceByShell0(BinModel Model){
    double q=Model.RiskNeutProb();
    int N=GetN();
    vector<double> Price(N+1);
    double ContVal;
    for (int i=0; i<=N; i++)
    {
        Price[i]=Payoff(Model.S(N,i));
    }
    for (int n=N-1; n>=0; n--)
    {
        for (int i=0; i<=n; i++)
        {
            ContVal=(q*Price[i+1]+(1-q)*Price[i])/(1+Model.GetR());
            Price[i]=Payoff(Model.S(n,i));
            if (ContVal>Price[i]) Price[i]=ContVal;
        }
    }
    return Price[0];
}

double AmOption::PriceByShell1(BinModel Model, BinLattice<double> &PriceTree, BinLattice<bool> &StopTree){
    int n = GetN();
    double q = Model.RiskNeutProb();
    PriceTree.SetN(n);
    StopTree.SetN(n);
    for (int i=0; i<n+1; i++) {
        PriceTree.SetNode(n, i, Payoff(Model.S(n,i)));
        StopTree.SetNode(n,i,1);
    }
    for (int j =n-1; j>=0; j--) {
        for (int i=0; i<j+1; i++) {
            double S = (q*PriceTree.GetNode(j+1, i+1)+(1-q)*PriceTree.GetNode(j+1, i))/(1+Model.GetR());
            PriceTree.SetNode(j, i, max(Payoff(Model.S(j,i)),S));
            StopTree.SetNode(j, i, 1);
            if (S>Payoff( Model.S(j,i))||PriceTree.GetNode(j, i)==0.0) {
                StopTree.SetNode(j, i, 0);
            }
        }
    }
    return PriceTree.GetNode(0, 0);
}

double AmOption::PriceByShell2(BinModel Model,  BinLattice<double>& PriceTree,  BinLattice<bool>& StoppingTree)
{ double q=Model.RiskNeutProb();
    int N=GetN();
    PriceTree.SetN(N);
    StoppingTree.SetN(N);
    double ContVal;
    for (int i=0; i<=N; i++)
    { PriceTree.SetNode(N,i,Payoff(Model.S(N,i)));
        StoppingTree.SetNode(N,i,1);
    }
    for (int n=N-1; n>=0; n--)
    {  for (int i=0; i<=n; i++)
    {  ContVal=(q*PriceTree.GetNode(n+1,i+1) +(1-q)*PriceTree.GetNode(n+1,i))/(1+Model.GetR());
        PriceTree.SetNode(n,i,Payoff(Model.S(n,i)));
        StoppingTree.SetNode(n,i,1);
        if (ContVal>PriceTree.GetNode(n,i))
        {  PriceTree.SetNode(n,i,ContVal);
            StoppingTree.SetNode(n,i,0);
        }
        else if (PriceTree.GetNode(n,i)==0.0)
        {  StoppingTree.SetNode(n,i,0);  }
    }
    }
    return PriceTree.GetNode(0,0);
}




