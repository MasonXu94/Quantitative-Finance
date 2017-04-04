//
//  Value.hpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/12.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#ifndef Value_hpp
#define Value_hpp

#include <stdio.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include "vector"
#include "curl/curl.h"
#include "Stock.hpp"

using namespace std;
typedef vector<double> Price;
typedef vector<vector<double>> Matrix;

class Value
{
private:
    Matrix R;
    Matrix AAR;
    Matrix CAAR;
    Matrix Pm;
    vector<string> Name;
    Price Rm;
    int N;
    int T;
public:
    void Set(int N_,int T_)
    {   N=N_;
        T=T_;
        Rm.resize(T+1);
        AAR.resize(3);
        CAAR.resize(3);
        R.resize(N);
        for (int n=0;n<N;n++)
            R[n].resize(T);
        for (int i=0;i<3;i++)
        {
            AAR[i].resize(T);
            CAAR[i].resize(T);
        }
    }
    void SetR(int n, int t, double x){R[n][t]=x;}
    double GetR(int n, int t){return R[n][t];}
    void SetAAR(int n, int t, double x){AAR[n][t]=x;}
    double GetAAR(int n, int t){return AAR[n][t];}
    void SetCAAR(int n, int t, double x){CAAR[n][t]=x;}
    double GetCAAR(int n,int t){return CAAR[n][t];}
    void SetName(const vector<string> &Name){this->Name = Name;}
    const vector<string>& GetName(){ return Name;}
    void DerivR(const vector<string>& Stock,const map<string,Price>& my_map,map<string, vector<double>>& Pm);
    void DerivAAR();
    void DerivCAAR();
    void Display(ofstream & foutput)
    {
        for(int n=-29;n<(T-30);n++)
        {
            foutput<<n<<" ";
            for(int i=0;i<3;i++)
                foutput<<GetCAAR(i,n+29)<<" ";
            foutput<<endl;
        }
    }
    
};


#endif /* Value_hpp */
