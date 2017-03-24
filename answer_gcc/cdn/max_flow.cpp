//
//  main.cpp
//  IMG_MaxFlow
//
//  Created by 张宁 on 2017/3/23.
//  Copyright © 2017年 张宁. All rights reserved.
//


// The push-relable algorithm code due to CLRS chapter 26
#include<iostream>
#include<list>
using namespace std;
const int N = 100;
int n; // vertex number
int e[N]; // residual flow of the vertex
int h[N]; // height of the vertex
int c[N][N]; // capacity of the edge
int f[N][N]; // flow of the edge
list<int> ev; // excess flow vertex
list<int> edge[N]; // edge link list
bool flag[N]; // lable whether the vertex is in the flow list

inline void Push(int u, int v) // push flow from edge (u, v)
{
    int df = min(e[u], c[u][v] - f[u][v]);
    f[u][v] += df;
    f[v][u] = -f[u][v];
    e[u] -= df;
    e[v] += df;
}

void Relable(int u) // re-lable heght of vertex u
{
    h[u] = n * 2 - 1;
    for(list<int>::iterator iter = edge[u].begin(); iter != edge[u].end(); iter++)
    {
        if(c[u][*iter] > f[u][*iter] && h[*iter] < h[u])
            h[u] = h[*iter];
    }
    h[u]++;
}

void Discharge(int u) // discharge the residual flow of vertex u
{
    list<int>::iterator iter = edge[u].begin();
    while(e[u] > 0)
    {
        if(iter == edge[u].end())
        {
            Relable(u);
            iter = edge[u].begin();
        }
        if(h[u] == h[*iter] + 1 && c[u][*iter] > f[u][*iter])
        {
            Push(u, *iter);
            if(e[*iter] > 0 && !flag[*iter])
                ev.push_back(*iter);
        }
        ++iter;
    }
}

void Init_PreFlow()
{
    ev.clear();
    h[0] = n;
    e[0] = 0;
    memset(flag, 0, sizeof(flag));
    memset(f, 0 , sizeof(f));
    flag[0] = flag[n-1] = true;
    for (int u = 1; u < n; u++)
    {
        f[0][u] = c[0][u];
        f[u][0] = -f[0][u];
        e[u] = c[0][u];
        if(e[u] > 0 && !flag[u])
        {
            ev.push_back(u);
            flag[u] = true;
        }
    }
    
    // construct link list
    for(int u = 0; u < n; u++)
        for(int v = u + 1; v < n; v++)
        {
            if(c[u][v] > 0 || c[v][u] > 0)
            {
                edge[u].push_back(v);
                edge[v].push_back(u);
            }
        }
}

int Push_Relable()
{
    Init_PreFlow();
    while(!ev.empty())
    {
        int u = ev.front();
        Discharge(u);
        ev.pop_front();
        flag[u] = false;
    }
    for (int i=0; i<10; i++) {
        cout<<e[i]<<endl;
    }
    return e[n-1];
}

int main()
{
    int m, u, v, w;
    cout<<"start"<<endl;
    while(scanf("%d%d", &m, &n) != EOF)
    {
        memset(c, 0, sizeof(c));
        for(int i = 0; i < m; i++)
        {
            cout<<"输入边信息"<<endl;
            scanf("%d%d%d", &u, &v, &w);
            c[u][v] = w;
        }
        printf("Max Flow is %d\n", Push_Relable());
    }
    return 0;
}
