#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
#include<list>
#include<map>
#include<vector>
using namespace std;

struct path {
    vector<int> p;        //存储路径
    int f;              //存储当前路径分配的流量
};

struct initial {
    int s;
    int cost;
};
const int N = 900;
typedef pair<int, int> PAIR;
//void process_data(char * topo[MAX_EDGE_NUM], int line_num);
void process_data(const char * const filename,const char * const resultfile);
int cmp(const PAIR &x, const PAIR &y);
void getTlist();
initial getinitial();
inline void Push(int u, int v); // push flow from edge (u, v)
void Relable(int u); // re-lable heght of vertex u
void Discharge(int u);// discharge the residual flow of vertex u
void Init_PreFlow();
int Push_Relable();
void dosomething(vector<int>& p, int f1);
list<path> getpath();
void Tabu_search();
void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);



#endif
