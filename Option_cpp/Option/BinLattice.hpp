//
//  BinLattice.hpp
//  Option
//
//  Created by Mason's Mac on 16/10/7.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

//Binary Lattice Pricing Tree

#ifndef BinLattice_hpp
#define BinLattice_hpp

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

template <typename Type>
class BinLattice {
    int N;
    vector<vector<Type>> Lattice;
public:
    void SetN(int N){
        this->N = N;
        Lattice.resize(N+1);
        for (int i = 0; i < N+1; i++) {
            Lattice[i].resize(i+1);
        }
    };
    void SetNode(int N, int i, Type X){
        Lattice[N][i] = X;
    };
    Type GetNode(int N, int i){
        return Lattice[N][i];
    };
    void Display(){
        cout << setiosflags(ios::fixed) << setprecision(3);
        for(int n=0; n<=N; n++)
        {  for(int i=0; i<=n; i++)
            cout << setw(7) << GetNode(n,i) ;
            cout << endl;
        }
        cout << endl;

    };
};

#endif /* BinLattice_hpp */
