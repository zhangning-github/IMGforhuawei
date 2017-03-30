#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
//#include <queue>
//#include <iostream>
//#include<algorithm>

using namespace std;
double alpha=0.5; //热度列表需用到的参数
double beta=0.5;  //热度列表需用到的参数
extern map<int,int> node_consumer; //记录consumer和与其相连的点（<node,consumer>）
int tabuList_len=3;                      //禁忌列表大小，初始化为3
int Nerbour_len;                    //存储候选解集的大小
extern int consumerNum;            //消费节点总数
extern vector<int> bw;            //存储每个节点的总带宽
extern int need;                     //消费节点总需求
extern vector<double> meanBw;  //存储每个节点的平均带宽
extern vector<double> meanPrice;//存储每个节点边的平均租用费
extern vector<int> sortbyBw;
extern int c[N][N];
vector<int> Tlist;                //热度列表

int cmp(const PAIR &x, const PAIR &y)
{
    return x.second > y.second;
}
int cmp1(const Solution_and_cost &x, const Solution_and_cost &y) {
    return x.cost<y.cost;
}
//生成热度列表
void getTlist() {
    vector<int> r;
    vector<PAIR>pair_vec;
    for(int i=1;i<meanBw.size();i++){
        pair_vec.push_back(make_pair(i,meanBw[i]*alpha+meanPrice[i]*beta));
    }
    sort(pair_vec.begin(), pair_vec.end(), cmp);
    for (vector<PAIR>::iterator curr = pair_vec.begin(); curr != pair_vec.end(); ++curr)
    {
        Tlist.push_back(curr->first);
    }
}

//判断带宽是否满足
bool isValid(int k) {
    int sum=0;
    for(int i=0;i<k;i++){
        sum+=bw[Tlist[i]];
    }
    if(sum>=need)
        return true;
    else
        return false;
}
bool isValid2(int k) {   //初始时只以带宽为标准来选点
    int sum=0;
    for(int i=0;i<k;i++){
        sum+=sortbyBw[i];
    }
    if(sum>=need)
        return true;
    else
        return false;
}

initial getinitial() {
    initial r;
    int i;
    for (i=1; i<consumerNum; i++) {
        if(isValid2(i)) {
            //现假定若前k个点无法跑出满足条件的最大流，则任意k个点的组合不满足需求（待商讨）。
            c[0][sortbyBw[i-1]]=10000;
            if(Push_Relable().maxflow==need)///&&&&&&&&&&需要修改
                break;
        }
    }
    r.s = i;
    if(i!=consumerNum)//i==consumerNum不需要再运行最大流，因为最优解是直接挂载
    {
        r.cost = Push_Relable().cost;
    }
    return r;
}

//找到由于swap操作产生的候选解集,候选解集的大小为solution的长度，候选集为通过solution中的每个元素
//与Tlist(热度列表)中不在solution中且热度最高的点交换得到。
vector<valueofOp> getswapN(vector<int> solution) {
    vector<valueofOp> r;
    int depot_wait_exchange=0;           //存储Tlist(热度列表)中不在solution中且热度最高的点
    Nerbour_len = (int)solution.size();
    //此处未考虑Tlist中已无点可交换的情况
    for(auto value:Tlist) {
        if(find(solution.begin(), solution.end(), value)==solution.end()) {
            depot_wait_exchange = value;
            break;
        }
    }
    valueofOp tmp;
    pair<int, int> swap;
    for (int i=0; i<Nerbour_len; i++) {
        tmp.solution = solution;
        tmp.solution[i] = depot_wait_exchange;
        
        swap.first = solution[i];
        swap.second = depot_wait_exchange;
        tmp.swap = swap;
        
        r.push_back(tmp);
    }
    
    return r;
}
//找到由于insert操作产生的候选解集
vector<valueofOp> getinsertN(vector<int> solution) {
    vector<valueofOp> r;
    //候选解的大小
    Nerbour_len = (int)min(solution.size(), Tlist.size()-solution.size());
    
    valueofOp tmp;
    pair<int, int> swap;
    int count=0;
    for (int i=0; i<(int)Tlist.size()&&count<Nerbour_len; i++) {
        if(find(solution.begin(), solution.end(), Tlist[i])!=solution.end()) {
            tmp.solution = solution;
            tmp.solution.push_back(Tlist[i]);
            
            swap.first = -1;
            swap.second = Tlist[i];
            tmp.swap = swap;
            
            r.push_back(tmp);
            count++;
        }
    }
    
    return r;
}
//查看候选解集中的元素是否都在禁忌列表
bool isallin(vector<valueofOp> N, vector<pair<int, int>> tubuList) {
    int tmp;
    for(auto it:N) {
        if(it.swap.first!=-1) {
            tmp = it.swap.first;
            it.swap.first = it.swap.second;
            it.swap.second = tmp;
        }
        if(find(tubuList.begin(), tubuList.end(), it.swap)==tubuList.end())
            return false;
    }
    return true;
}
//step4
valueofOp dosomething2(vector<Solution_and_cost> nerbou) {
    valueofOp r;
    int tmp;
//    for(auto it:nerbou) {
//        if(it.swap.first!=-1) {
//            tmp = it.swap.first;
//            it.swap.first = it.swap.second;
//            it.swap.second = tmp;
//        }
//        if(find(Tlist.begin(), Tlist.end(), it.swap)!=Tlist.end()) continue;
//        r.solution = it.solution;
//        r.swap = it.swap;
//        break;
//    }
    return r;
}
//更新禁忌列表
void updateT(vector<pair<int, int>>& tabuList, valueofOp value) {
    if(tabuList.size()>=tabuList_len) {
        tabuList.erase(tabuList.begin());
    }
    tabuList.push_back(value.swap);
}
void Tabu_search() {
    //生成Tlist列表
    getTlist();
    //上述两步在cdn.cpp处理数据时完成，此处为逻辑完整性，列出，最终需删除
    
    int A;                          //渴望水平
    vector<int> bestSolu;             //历史最优解
    vector<int> curBestSolu;                  //保存候选解中的最优解
    int mincost;                       //保存候选解中最小花费
    vector<valueofOp> op;             //存储swap候选解及得到候选解所做操作
    //vector<valueof_insertN> insertN;         //存储inserN候选解
    vector<pair<int, int>> tabuList;               //存储禁忌列表
    auto r = getinitial();
    A = r.cost;
    if(r.s==consumerNum) return;        //输出原始解
    vector<Solution_and_cost> nerbou;           //存储候选解及其花费（按花费从高到低排序）
    
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
            Solution_and_cost scost_tmp;
            for(auto it:op) {
                auto tmpcost=(Push_Relable()).cost;
                scost_tmp.solution = it.solution;
                scost_tmp.cost = tmpcost;
                scost_tmp.swap = it.swap;
                nerbou.push_back(scost_tmp);
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
                updateT(tabuList, tmp);
            }
            else {
                sort(nerbou.begin(), nerbou.end(), cmp1);
                updateT(tabuList, dosomething2(nerbou));
            }
            nerbou.clear();
        }
        //上组swap操作结束
        
        //对于每一种insert求出其候选解的情况
        op = getinsertN(curBestSolu);
        if(isallin(op, tabuList)) break;           //如果所有insert的候选解都在T中，则停止整个算法
        mincost = INT_MAX;
        valueofOp tmp_insert;
        Solution_and_cost scost_tmp;
        for(auto valueof_insertN:op) {
            auto tmpcost = (Push_Relable()).cost;
            scost_tmp.solution = valueof_insertN.solution;
            scost_tmp.cost = tmpcost;
            scost_tmp.swap = valueof_insertN.swap;
            nerbou.push_back(scost_tmp);
            if(mincost<tmpcost) {
                mincost = tmpcost;
                curBestSolu = valueof_insertN.solution;
                tmp_insert = valueof_insertN;
            }
        }
        if(mincost<A) {
            bestSolu = curBestSolu;
            A = mincost;
            updateT(tabuList, tmp_insert);
        }
        else {
            sort(nerbou.begin(), nerbou.end(), cmp1);
            updateT(tabuList, dosomething2(nerbou));
        }
        
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
