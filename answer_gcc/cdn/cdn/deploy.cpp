#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include<limits>
//#include <queue>
//#include <iostream>
//#include<algorithm>

using namespace std;
double alpha=0.1; //热度列表需用到的参数
double beta=0.1;  //热度列表需用到的参数
extern int serverPrice;//服务器单价
extern vector<int> occurrence;//出现次数
extern map<int,int> node_consumer; //记录consumer和与其相连的点（<node,consumer>）
int tabuList_len=20;                      //禁忌列表大小，初始化为3
int Nerbour_len;                    //存储候选解集的大小
extern int n;
extern int consumerNum;            //消费节点总数
extern vector<int> bw;            //存储每个节点的总带宽
extern vector<int> sortbyBw;//将节点id按bw由大到小排序
extern int need;                     //消费节点总需求
extern vector<double> meanBw;  //存储每个节点的平均带宽
extern vector<double> meanPrice;//存储每个节点边的平均租用费
extern vector<int> sortbyBwandE;
extern vector<int>sortByH_E;// 以层次和边数来排序
extern int f[N][N];

extern int c[N][N];
vector<int> Tlist;                //热度列表
extern vector<int> selected;     //被选为挂载服务器的节点

vector<H_tral1> Hierarchy_traversal;    //存储层次遍历结果

vector<int>bad;  //存储那些drop之后不满足要求的点


int cmp(const PAIR &x, const PAIR &y)
{
    return x.second > y.second;
}
int cmp1(const Solution_and_cost &x, const Solution_and_cost &y) {
    return x.cost<y.cost;
}

int cmp3(const hot_v &x, const hot_v &y) {
    return x.hot<y.hot;
}
//void change_c(vector<int>solution){
//    for (int i=0; i<N; i++) {
//        c[0][i]=0;
//    }
//    for(auto j:solution) {
//        c[0][j]=10000;
//    }
//}
void getsortByH_E(){
    vector<PAIR>pair_vec;
    
    for(int i=1;i<Hierarchy_traversal.size();i++){
        pair_vec.push_back(make_pair(Hierarchy_traversal[i].vextec,10/(Hierarchy_traversal[i].Hierarchy-1)+0*occurrence[Hierarchy_traversal[i].vextec]+meanBw[Hierarchy_traversal[i].vextec]*alpha+meanPrice[Hierarchy_traversal[i].vextec]*beta));
    }
    sort(pair_vec.begin(), pair_vec.end(), cmp);
    for (vector<PAIR>::iterator curr = pair_vec.begin(); curr != pair_vec.end(); ++curr)
    {
        sortByH_E.push_back(curr->first);
    }
}
void getsortbyBwandE(){
    vector<PAIR>pair_vec;
    for(int i=1;i<bw.size();i++){
        pair_vec.push_back(make_pair(i,bw[i]+occurrence[i]*10));
    }
    sort(pair_vec.begin(), pair_vec.end(), cmp);
    for (vector<PAIR>::iterator curr = pair_vec.begin(); curr != pair_vec.end(); ++curr)
    {
        sortbyBwandE.push_back(curr->first);
    }
}


void get_H_tral() {
    queue<H_tral1> H_tral;
    queue<H_tral1> tmp;
    //标记顶点是否已经加入，防止，重复加入
    vector<int> flag(10001,0);
    int count = 1;
    H_tral1 Htmp;
    Htmp.Hierarchy = count;
    Htmp.vextec = n-1;
    //将超级汇点加入
    tmp.push(Htmp);
    Hierarchy_traversal.push_back(Htmp);
    flag[n-1]=1;
    while (!tmp.empty()) {
        int size = (int)tmp.size();
        count++;
        for (int j=0; j<size; j++) {
            H_tral1 t = tmp.front();
            for (int i=0; i<n; i++) {
                if(c[i][t.vextec]!=0&&flag[i]==0) {
                    Htmp.Hierarchy = count;
                    Htmp.vextec = i;
                    tmp.push(Htmp);
                    Hierarchy_traversal.push_back(Htmp);
                    flag[i]=1;
                }
            }
            tmp.pop();
        }
        
    }
    
}

//生成热度列表
//void getTlist() {
//    vector<int> r;
//    vector<PAIR>pair_vec;
//    for(int i=1;i<meanBw.size();i++){
//        pair_vec.push_back(make_pair(i,meanBw[i]*alpha+meanPrice[i]*beta));
//    }
//    sort(pair_vec.begin(), pair_vec.end(), cmp);
//    for (vector<PAIR>::iterator curr = pair_vec.begin(); curr != pair_vec.end(); ++curr)
//    {
//        Tlist.push_back(curr->first);
//    }
//}

//判断带宽是否满足
//bool isValid(int k) {
//    int sum=0;
//    for(int i=0;i<k;i++){
//        sum+=bw[Tlist[i]];
//    }
//    if(sum>=need)
//        return true;
//    else
//        return false;
//}
//bool isValid2(int k) {   //初始时只以带宽为标准来选点
//    int sum=0;
//    for(int i=0;i<k;i++){
//        sum+=sortbyBw[i];
//    }
//    if(sum>=need)
//        return true;
//    else
//        return false;
//}


void sortbyflow_edge(vector<int>& solution) {
    vector<hot_v> tmp;
    hot_v tmphot_v;
    int hot=0;
    for(auto value:solution) {
        for (int i=0; i<n; i++) {
            if(f[value][i]>0) hot+=f[value][i];
        }
        tmphot_v.hot = hot;
        tmphot_v.vertex = value;
        tmp.push_back(tmphot_v);
    }
    sort(tmp.begin(), tmp.end(), cmp3);
    for (int i=0; i<solution.size(); i++) {
        solution[i]=tmp[i].vertex;
    }
}

void writeResult(char * result_file,vector<int>solution)
{
    //change_c(solution);
    list<path> pa=getpath();
    ofstream out;
    out.open(result_file);
    if(!out)
    {
        cout<<"Error opening output stream!"<<endl;
        return ;
    }
    out<<pa.size()<<"\n";
    out<<"\n";
    for (auto it = pa.begin(); it!=pa.end(); it++) {
        it->p.pop_back();
        auto it2 = it->p.begin();
        it2++;
        for (; it2!=it->p.end(); it2++) {
            
            out<<*it2-1<<" ";
            
        }
        out<<node_consumer.at(it->p.back())<<" ";
        out<<it->f<<"\n";
    }
    
}
initial getinitial(){
    initial r;
    PAIR cur;
    
    init_graph(selected);
    cur=mcmf(0,n-1);
    cout<<"MAX Flow"<<cur.first<<"cost="<<cur.second<<endl;
    r.s = (int)selected.size();
    r.cost=cur.second;
    return r;
}

//initial getinitial() {
//    getsortbyBwandE();
//    initial r;
//    int i;
//    int j=0;
//    int index=1;
//    int sum=0;
//    int cur_cost=0;
//    for (i=1; i<consumerNum; i++) {
//
//        //现假定若前k个点无法跑出满足条件的最大流，则任意k个点的组合不满足需求（待商讨）。
//
//        while(j<n)
//        {
//            int k=0;
//            for(;k<i-1;k++)
//            {
//                if (c[selected[k]][sortbyBwandE[j]]!=0)
//                    break;
//            }
//            if(k==i-1)
//            {
//                c[0][sortbyBwandE[j]]=10000;
//                selected.push_back(sortbyBwandE[j]);
//                j++;
//                break;
//            }
//            j++;
//        }
//
//        if(j>=n)
//        {
//            while(true)
//            {
//                int k=0;
//                for(;k<selected.size();k++){
//                    if(selected[k]==sortbyBwandE[index]){
//                        break;
//                    }
//                }
//                if(k==selected.size()){
//                    c[0][sortbyBwandE[j]]=10000;
//                    selected.push_back(sortbyBwandE[j]);
//                    index++;
//                    break;
//                }
//                index++;
//            }
//        }
//        sum+=bw[selected.back()];
//        if(sum>=need)
//        {
//            cout<<i<<endl;
//            maxflow_and_cost cur=Push_Relable();
//            cur_cost=cur.cost;
//            cout<<"bw sum="<<sum<<","<<"need="<<need<<endl;
//            cout<<"MAX Flow"<<cur.maxflow<<endl;
//            if(cur.maxflow==need)
//                break;
//        }
//    }
//
//    r.s = i;
//    r.cost=cur_cost;
//    //    if(i!=consumerNum)//i==consumerNum不需要再运行最大流，因为最优解是直接挂载
//    //    {
//    //        list<path> pa=getpath();
//    //        ofstream out;
//    //        out.open(result_file);
//    //        if(!out)
//    //        {
//    //            cout<<"Error opening output stream!"<<endl;
//    //            return r;
//    //        }
//    //        out<<pa.size()<<"\n";
//    //        out<<"\n";
//    //        for (auto it = pa.begin(); it!=pa.end(); it++) {
//    //            it->p.pop_back();
//    //            auto it2 = it->p.begin();
//    //            it2++;
//    //            for (; it2!=it->p.end(); it2++) {
//    //
//    //                out<<*it2-1<<" ";
//    //
//    //            }
//    //            out<<node_consumer.at(it->p.back())<<" ";
//    //            out<<it->f<<"\n";
//    //        }
//    //
//    //
//    //    }
//    return r;
//}

//找到由于swap操作产生的候选解集,候选解集的大小为solution的长度，候选集为通过solution中的每个元素
//与Tlist(热度列表)中不在solution中且热度最高的点交换得到。
vector<valueofOp> getswapN(vector<int> solution) {
    vector<valueofOp> r;
    int depot_wait_exchange=0;           //存储Tlist(热度列表)中不在solution中且热度最高的点
    Nerbour_len = 10;
    //此处未考虑Tlist中已无点可交换的情况
    for(auto value:sortByH_E) {
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


vector<valueofOp> getdropN(vector<int> solution) {
    sortbyflow_edge(solution);
    vector<valueofOp> r;
    Nerbour_len = 10;                           //假定drop候选集的大小为10
    valueofOp tmp;
    pair<int, int> drop;
    int count = 0;
    for(int i=0;i<solution.size();i++){
        if(find(bad.begin(), bad.end(), solution[i])==bad.end())
        {
            tmp.solution = solution;
            auto it = tmp.solution.begin();
            tmp.solution.erase(it+i);
            
            drop.first=-2;
            drop.second=*(it+i);
            tmp.swap = drop;
            
            r.push_back(tmp);
            count++;
            if(count>=min((int)solution.size(),Nerbour_len))
                break;
            
        }
    }
    //    for(int i=0; count<Nerbour_len&&i<solution.size();i++,count++) {
    //        tmp.solution = solution;
    //        auto it = tmp.solution.begin();
    //        tmp.solution.erase(it+i);
    //
    //        drop.first=-2;
    //        drop.second=*(it+i);
    //        tmp.swap = drop;
    //
    //        r.push_back(tmp);
    //    }
    return r;
}

//int drop_num=2;                      //每次要drop掉的depot数目
////每次drop两个depot。
//vector<valueofOp> getdropN(vector<int> solution) {
//    sortbyflow_edge(solution);
//    vector<valueofOp> r;
//    Nerbour_len = min(10, (int)solution.size()/2);                  //假定drop候选集的大小为10
//    valueofOp tmp;
//    pair<int, int> drop;
//    int count = 0;
//    for(int i=0; count<Nerbour_len&&i<solution.size();i++,count++) {
//        tmp.solution = solution;
//        auto it = tmp.solution.begin();
//        //每次drop掉前drop_num个元素
//        for (int j=0; j<drop_num; j++) {
//            tmp.solution.erase(it+i+j);
//        }
//        r.push_back(tmp);
//    }
//    return r;
//}
//找到由于insert操作产生的候选解集
vector<valueofOp> getinsertN(vector<int> solution) {
    vector<valueofOp> r;
    //候选解的大小
    Nerbour_len = (int)min(solution.size(), sortByH_E.size()-solution.size());
    
    valueofOp tmp;
    pair<int, int> swap;
    int count=0;
    for (int i=0; i<(int)sortByH_E.size()&&count<Nerbour_len; i++) {
        if(find(solution.begin(), solution.end(), sortByH_E[i])!=solution.end()) {
            tmp.solution = solution;
            tmp.solution.push_back(sortByH_E[i]);
            
            swap.first = -1;
            swap.second = sortByH_E[i];
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
        if(it.swap.first!=-2) {
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
valueofOp dosomething2(vector<Solution_and_cost> nerbou, vector<pair<int, int>> tabuList) {
    valueofOp r;
    int tmp;
    for(auto it:nerbou) {
        if(it.swap.first!=-2) {
            tmp = it.swap.first;
            it.swap.first = it.swap.second;
            it.swap.second = tmp;
        }
        if(find(tabuList.begin(), tabuList.end(), it.swap)!=tabuList.end()) continue;
        r.solution = it.solution;
        r.swap = it.swap;
        break;
    }
    return r;
}
//更新禁忌列表
void updateT(vector<pair<int, int>>& tabuList, valueofOp value) {
    if(tabuList.size()>=tabuList_len) {
        tabuList.erase(tabuList.begin());
    }
    tabuList.push_back(value.swap);
}

void updateBad(valueofOp value){
    if(bad.size()>=tabuList_len){
        bad.erase(bad.begin());
    }
    bad.push_back(value.swap.second);
}
vector<int> Tabu_search(initial r,long long time) {
    long long time_used;;
    struct timeval start, finish;
    gettimeofday(&start,NULL);
    //生成Tlist列表
    //上述两步在cdn.cpp处理数据时完成，此处为逻辑完整性，列出，最终需删除
    
    
    
    int A;                          //渴望水平
    vector<int> bestSolu=selected;             //历史最优解
    vector<int> curBestSolu=selected;                  //保存候选解中的最优解
    int mincost;                       //保存候选解中最小花费
    vector<valueofOp> op;             //存储swap候选解及得到候选解所做操作
    //vector<valueof_insertN> insertN;         //存储inserN候选解
    vector<pair<int, int>> tabuList;               //存储禁忌列表
    //auto r = getinitial();
    A = r.cost+r.s*serverPrice;
    cout<<"best cost"<<A<<endl;
    vector<Solution_and_cost> nerbou;           //存储候选解及其花费（按花费从高到低排序）
    //
    //    for (int i=0; i<r.s; i++) {
    //        curBestSolu.push_back(bw[i]);
    //    }
    //
    for (int k=bestSolu.size(); k>0; k--) {                       //insert操作的循环
        gettimeofday(&finish, NULL);
        time_used=diff_in_us(&finish,&start)+time;
        if(time_used>83000000){
            cout<<"Time is used!"<<endl;
            break;
        }
        cout<<"****************"<<k<<","<<time_used<<endl;
        //drop操作
        op = getdropN(curBestSolu);
        if(isallin(op, tabuList)) break;
        mincost = 1000000;
        valueofOp tmp_drop;
        Solution_and_cost drop_tmp;
        for(auto valueof_dropN:op) {
            init_graph(valueof_dropN.solution);
            auto cur=mcmf(0,n-1);
            auto tmpcost = cur.second+valueof_dropN.solution.size()*serverPrice;
            if(cur.first!=need){
                updateBad(valueof_dropN);
                continue;
            }
            cout<<"drop cost"<<tmpcost<<endl;
            drop_tmp.solution = valueof_dropN.solution;
            drop_tmp.cost = tmpcost;
            drop_tmp.swap = valueof_dropN.swap;
            nerbou.push_back(drop_tmp);
            if(mincost>tmpcost) {
                mincost = tmpcost;
                curBestSolu = valueof_dropN.solution;
                tmp_drop = valueof_dropN;
            }
        }
        if(mincost<A) {
            bestSolu = curBestSolu;
            A=mincost;
            updateT(tabuList, tmp_drop);
        }
        else {
            sort(nerbou.begin(), nerbou.end(), cmp1);
            updateT(tabuList, dosomething2(nerbou, tabuList));
        }
        nerbou.clear();
        //drop操作结束
        
        
        //针对每一种insert最优情况，做一组swap操作
        mincost=1000000;
        op = getswapN(curBestSolu);
        valueofOp tmp;
        
        //swap中有不在禁忌列表中的情况
        if(!isallin(op, tabuList)) {
            //选次优解待定
            
            Solution_and_cost scost_tmp;
            for(auto it:op) {
                init_graph(it.solution);
                auto cur=mcmf(0,n-1);
                auto tmpcost = cur.second+it.solution.size()*serverPrice;
                if(cur.first!=need)continue;
                cout<<"swap cost"<<tmpcost<<endl;
                scost_tmp.solution = it.solution;
                scost_tmp.cost = tmpcost;
                scost_tmp.swap = it.swap;
                nerbou.push_back(scost_tmp);
                if(mincost>tmpcost) {
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
                updateT(tabuList, dosomething2(nerbou,tabuList));
            }
            nerbou.clear();
        }
        //上组swap操作结束
    }
    return bestSolu;
}


























//
//void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
//{
//    int consumerNum = 0;
//    char *c;
//    int spaceCount = 0;
//
//    c = topo[0];
//
//    while (*c != '\0' && *c != '\n' && *c != '\r')
//    {
//        if (*c == ' ')
//        {
//            c++;
//            spaceCount++;
//            continue;
//        }
//        if (spaceCount == 2)
//        {
//            consumerNum = *c - '0' + consumerNum * 10;
//        }
//        c++;
//    }
//
//    string res;
//    char a[20];
//    sprintf(a, "%d\n\n",consumerNum);
//    res = a;
//    int netnode, need;
//
//    for (int i = 1; i < consumerNum+1; i++)
//    {
//        c = topo[line_num-i];
//        netnode = need = spaceCount = 0;
//        while (*c != '\0' && *c != '\n' && *c != '\r')
//        {
//            if (*c == ' ')
//            {
//                c++;
//                spaceCount++;
//                continue;
//            }
//            if (spaceCount == 1)
//            {
//                netnode = *c - '0' + netnode * 10;
//            }
//            else if (spaceCount == 2)
//            {
//                need = *c - '0' + need * 10;
//            }
//            c++;
//        }
//        sprintf(a, "%d %d %d",netnode,consumerNum-i,need);
//        res += a;
//        if (i != consumerNum)
//        {
//            res += "\n";
//        }
//    }
//
//    char * topo_file = (char *)res.c_str();
//    write_result(topo_file, filename);
//}
