//
//  Value.cpp
//  Final project
//
//  Created by Mason's Mac on 2016/12/12.
//  Copyright © 2016年 Mason's Mac. All rights reserved.
//

#include "Value.hpp"

void Value::DerivR(const vector<string>& Stock,const map<string,Price>& my_map,map<string, vector<double>>& Pm)
{
    SetName(Stock);
    for (int i=0; i<N; i++)
    {
        auto it= my_map.find(Stock[i]);
        auto it0 = Pm.find(Stock[i]);
        if (it != my_map.end())
            for (int t=0; t<T;t++)
            {
                double r = it->second[t+1]/it->second[t]-1;
                double r0 = it0->second[t+1]/it0->second[t]-1;
                SetR(i,t,r-r0);
            }
    }
}

void Value::DerivAAR()
{
    for (int t=0; t<T; t++)
    {
        double m=0;double n=0;double l=0;
        for (int i=0; i<N/3; i++)
        {
            m=m+GetR(i, t);
        }
        for (int i=N/3;i<2*N/3;i++)
        {
            n=n+GetR(i, t);
        }
        for (int i=2*N/3;i<N;i++)
        {
            l=l+GetR(i, t);
        }
        SetAAR(0,t,m*3/N);
        SetAAR(1,t,n*3/N);
        SetAAR(2,t,l*3/N);
    }
}
void Value::DerivCAAR()
{
    double m=0, n=0, l=0;
    for (int t=0;t<T;t++)
    {
        m=m+GetAAR(0, t);
        n=n+GetAAR(1, t);
        l=l+GetAAR(2, t);
        SetCAAR(0, t, m);
        SetCAAR(1, t, n);
        SetCAAR(2, t, l);
    }
}
