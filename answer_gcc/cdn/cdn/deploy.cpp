#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>


using namespace std;

int n;                          //消费节点总数
list<int> bw;
int need;                       //消费节点总需求
//生成热度列表
list<int> getTlist() {
    list<int> r;
    return r;
}
//生成总带宽列表
list<int> getbw() {
    list<int> r;
    return r;
}
//判断带宽是否满足
bool isValid(int k) {
    bool r;
    return r;
}
//获取最大流及花费
pair<int, int> Push_Relable1() {
    pair<int, int> r;
    return r;
}
int getinitial() {
    int i;
    for (i=1; i<n; i++) {
        auto tmp = isValid(i);
        if(tmp) {
            //现假定若前k个点无法跑出满足条件的最大流，则任意k个点的组合不满足需求（待商讨）。
            if(Push_Relable1().first==need) break;
        }
    }
    return i;
}
struct valueof_swapN {
    vector<int> S;
    pair<int, int> swap;                //查找时不应有顺序因素影响
};
struct valueof_insertN {
    vector<int> S;
    int add;
};
vector<valueof_swapN> getswapN() {
    vector<valueof_swapN> r;
    return r;
}
bool isallin(vector<valueof_swapN> N, vector<pair<int, int>> T) {
    return true;
}
//step4
void dosomething() {
    
}
void updateT_swap(vector<pair<int, int>>& T, valueof_swapN value) {
    
}
void Tabu_search() {
    int A;                          //渴望水平
    vector<int> S_best;             //历史最优解
    vector<int> S;                  //保存候选解中的最优解
    int cost;                       //保存候选解中最小花费
    vector<valueof_insertN> insert;           //同下
    vector<valueof_swapN> swapN;             //存储swap候选解及得到候选解所做操作
    vector<pair<int, int>> T;               //存储禁忌列表
    int r = getinitial();                   //此处为初始化A
    if(r==n) return;        //输出原始解
    swapN = getswapN();
    for (int k=r; k<n; k++) {                       //insert操作的循环
        while (1) {                                 //swap操作的循环
            cost=INT_MAX;
            swapN = getswapN();
            valueof_swapN tmp;
            if(isallin(swapN, T)) {                 //所有候选解都被禁。
                break;
            }
            
            for(auto it:swapN) {
                auto tmpcost=(Push_Relable1()).second;
                if(cost<tmpcost) {
                    cost=tmpcost;
                    S=it.S;
                    tmp = it;
                }
            }
            //循环跳出之后，则S为候选解中最优解，cost为其花费。
            if(cost<A) {
                S_best = S;
                A = cost;
            }
            else {
                dosomething();
            }
            updateT_swap(T, tmp);
        }
    }
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    int consumerNum = 0;
    char *c;
    int spaceCount = 0;
    
    c = topo[0];
    
    while (*c != '\0' && *c != '\n' && *c != '\r')
    {
        if (*c == ' ')
        {
            c++;
            spaceCount++;
            continue;
        }
        if (spaceCount == 2)
        {
            consumerNum = *c - '0' + consumerNum * 10;
        }
        c++;
    }
    
    string res;
    char a[20];
    sprintf(a, "%d\n\n",consumerNum);
    res = a;
    int netnode, need;
    
    for (int i = 1; i < consumerNum+1; i++)
    {
        c = topo[line_num-i];
        netnode = need = spaceCount = 0;
        while (*c != '\0' && *c != '\n' && *c != '\r')
        {
            if (*c == ' ')
            {
                c++;
                spaceCount++;
                continue;
            }
            if (spaceCount == 1)
            {
                netnode = *c - '0' + netnode * 10;
            }
            else if (spaceCount == 2)
            {
                need = *c - '0' + need * 10;
            }
            c++;
        }
        sprintf(a, "%d %d %d",netnode,consumerNum-i,need);
        res += a;
        if (i != consumerNum)
        {
            res += "\n";
        }
    }
    
    char * topo_file = (char *)res.c_str();
    write_result(topo_file, filename);
}
