#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <queue>
#include <iostream>
#include<list>
#include<map>
#include<vector>
#include<algorithm>
#include<cstring>
#include<fstream>
using namespace std;

struct path {
    vector<int> p;        //存储路径
    int f;              //存储当前路径分配的流量
};

struct initial {
    int s;
    int cost;
};
struct maxflow_and_cost {
    int maxflow=0;
    int cost=0;
};
struct Solution_and_cost {
    vector<int> solution;
    pair<int, int> swap;
    int cost;
};
struct valueofOp {
    vector<int> solution;
    pair<int, int> swap;                //查找时不应有顺序因素影响
};
struct H_tral1 {
    int vextec;
    int Hierarchy;
};
const int N = 900;

typedef pair<int, int> PAIR;
void process_data(const char * const filename,const char * const resultfile);
int cmp(const PAIR &x, const PAIR &y);
void change_c(vector<int>solution);
void getsortbyBwandE();
void getTlist();
initial getinitial();
inline void Push(int u, int v); // push flow from edge (u, v)
void Relable(int u); // re-lable heght of vertex u
void Discharge(int u);// discharge the residual flow of vertex u
void Init_PreFlow();
maxflow_and_cost Push_Relable();
void dosomething(vector<int>& p, int f1);
valueofOp dosomething2(vector<Solution_and_cost> nerbou, vector<pair<int, int>> tabuList);
list<path> getpath();
vector<int>Tabu_search(initial r);
void writeResult(char * result_file,vector<int>solution);
void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
void get_H_tral();
void getsortByH_E();


#endif
