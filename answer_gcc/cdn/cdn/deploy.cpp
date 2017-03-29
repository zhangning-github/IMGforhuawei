#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>


using namespace std;
extern int consumerNum;            //消费节点总数
extern vector<int> bw;            //存储每个节点的总带宽
extern int need;                     //消费节点总需求
list<int> Tlist;                    //热度列表
struct initial {
    int s;
    int cost;
};
//生成热度列表
list<int> getTlist() {
    list<int> r;
    return r;
}
//生成总带宽列表
vector<int> getbw() {
    vector<int> r;
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
initial getinitial() {
    initial r;
    int i;
    for (i=1; i<consumerNum; i++) {
        auto tmp = isValid(i);
        if(tmp) {
            //现假定若前k个点无法跑出满足条件的最大流，则任意k个点的组合不满足需求（待商讨）。
            if(Push_Relable1().first==need) break;
        }
    }
    r.s = i;
    r.cost = (Push_Relable1()).second;
    return r;
}
struct valueofOp {
    vector<int> solution;
    pair<int, int> swap;                //查找时不应有顺序因素影响
};
vector<valueofOp> getswapN(vector<int> solution) {
    vector<valueofOp> r;
    return r;
}
vector<valueofOp> getinsertN(vector<int> solution) {
    vector<valueofOp> r;
    return r;
}
bool isallin(vector<valueofOp> N, vector<pair<int, int>> tubuList) {
    return true;
}
//step4
void dosomething() {
    
}
void updateT(vector<pair<int, int>>& tabuList, valueofOp value) {
    
}
void Tabu_search() {
//生成Tlist列表
    getTlist();
//生成bw（带宽）列表
    getbw();
//上述两步在cdn.cpp处理数据时完成，此处为逻辑完整性，列出，最终需删除
    
    int A;                          //渴望水平
    vector<int> bestSolu;             //历史最优解
    vector<int> curBestSolu;                  //保存候选解中的最优解
    int mincost;                       //保存候选解中最小花费
    vector<valueofOp> op;             //存储swap候选解及得到候选解所做操作
    //vector<valueof_insertN> insertN;         //存储inserN候选解
    vector<pair<int, int>> tabuList;               //存储禁忌列表
    auto r = getinitial();                   //此处未初始化A
    A = r.cost;
    if(r.s==consumerNum) return;        //输出原始解

    for (int i=0; i<r.s; i++) {
        curBestSolu.push_back(bw[i]);
    }
    
    for (int k=r.s; k<consumerNum; k++) {                       //insert操作的循环
        
//针对每一种insert最优情况，做一组swap操作
        mincost=INT_MAX;
        op = getswapN(curBestSolu);
        valueofOp tmp;

//swap中有不在禁忌列表中的情况
        if(!isallin(op, tabuList)) {
            //选次优解待定
            
            for(auto it:op) {
                auto tmpcost=(Push_Relable1()).second;
                if(mincost<tmpcost) {
                    mincost=tmpcost;
                    curBestSolu=it.solution;
                    tmp = it;
                }
            }
            
            //循环跳出之后，则S为候选解中最优解，cost为其花费。
            if(mincost<A) {
               bestSolu = curBestSolu;
                A = mincost;
            }
            else {
                dosomething();
            }
            updateT(tabuList, tmp);
        }
//上组swap操作结束
        
//对于每一种insert求出其候选解的情况
        op = getinsertN(curBestSolu);
        if(isallin(op, tabuList)) break;           //如果所有insert的候选解都在T中，则停止整个算法
        mincost = INT_MAX;
        valueofOp tmp_insert;
        for(auto valueof_insertN:op) {
            auto tmpcost = (Push_Relable1()).second;
            if(mincost<tmpcost) {
                mincost = tmpcost;
                curBestSolu = valueof_insertN.solution;
                tmp_insert = valueof_insertN;
            }
        }
        if(mincost<A) {
            bestSolu = curBestSolu;
            A = mincost;
        }
        else {
            dosomething();
        }
        updateT(tabuList, tmp_insert);
//求insert候选解结束
        
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
