//
//  main.cpp
//  cdn
//
//  Created by 王硕 on 2017/3/22.
//  Copyright © 2017年 王硕. All rights reserved.
//

#include "deploy.h"
#include "lib_io.h"
#include "lib_time.h"
#include <sys/time.h>
//#include<stdio.h>
//#include<fstream>
//#include<iostream>
//#include<queue>

using namespace std;
vector<int> occurrence;//出现次数
map<int,int> node_consumer; //记录consumer和与其相连的点（<node,consumer>）
//map<int,int> bwMap;//从1开始
int price[N][N]={0}; //每条边的单位租用费
int consumerNum; //消费节点数量
int serverPrice;//服务器单价
int need;   //服务器总需求
vector<int> bw;     //存储每个节点的总带宽
vector<int> sortbyBw;//将节点id按bw由大到小排序
vector<int> sortbyBwandE;
vector<double> meanBw;  //存储每个节点的平均带宽
vector<double> meanPrice;//存储每个节点边的平均租用费
int n=0; // vertex number
int e[N]; // residual flow of the vertex
int h[N]; // height of the vertex
int c[N][N]={0}; // capacity of the edge
int f[N][N]; // flow of the edge
list<int> ev; // excess flow vertex
list<int> edge[N]; // edge link list
vector<int> mydege[N];
bool flag[N]; // lable whether the vertex is in the flow list
extern vector<H_tral1> Hierarchy_traversal;    //存储层次遍历结果
vector<int>sortByH_E;// 以层次和边数来排序
unsigned long diff_in_us(struct timeval *finishtime, struct timeval * starttime)
{
    unsigned long long usec;
    usec = (finishtime->tv_sec - starttime->tv_sec)*1000000;
    usec += finishtime->tv_usec - starttime->tv_usec;
    return usec;
}


void process_data(const char * const filename,const char * const resultfile){
    ifstream in;
    ofstream out;
    int m,u,v,b,p;
    
    in.open(filename);
    
    if(!in)
    {
        cout<<"Error opening input stream!"<<endl;
        return ;
    }
    in>>n>>m>>consumerNum;
    //cout<<n<<","<<m<<","<<consumerNum<<endl;
    in>>serverPrice;
    //cout<<serverPrice<<endl;
    for(int i=0;i <=n;i++)
    {
        bw.push_back(0);
        //meanBw.push_back(0);
        meanPrice.push_back(0);
        occurrence.push_back(0);
    }
    
    for(int i=0;i<m;i++)
    {
        in>>u>>v>>b>>p;
        
        //cout<<u<<","<<v<<","<<bw<<","<<p<<endl;
        //此处可构建图
        occurrence[u+1]++;
        occurrence[v+1]++;
        c[u+1][v+1]=b;
        c[v+1][u+1]=b;
        bw[u+1]+=b;
        bw[v+1]+=b;
        meanPrice[u+1]+=p;
        meanPrice[v+1]+=p;
        price[u+1][v+1]=p;
        price[v+1][u+1]=p;
    }
    meanBw.push_back(0);
    for(int i=1;i<=n;i++)
    {
        meanBw.push_back(bw[i]/occurrence[i]);
        meanPrice[i]=meanPrice[i]/occurrence[i];
    }
    //get
    vector<PAIR>pair_vec;
    for(int i=1;i<=n;i++){
        pair_vec.push_back(make_pair(i,bw[i]));
    }
    sort(pair_vec.begin(), pair_vec.end(), cmp);
    for (vector<PAIR>::iterator curr = pair_vec.begin(); curr != pair_vec.end(); ++curr)
    {
        sortbyBw.push_back(curr->first);
    }
    
    
    n+=2;
    
    need=0;
    out.open(resultfile);
    if(!out)
    {
        cout<<"Error opening output stream!"<<endl;
        return ;
    }
    out<<consumerNum<<"\n";
    out<<"\n";
    for(int i=0;i<consumerNum;i++)
    {
        in>>u>>v>>b;
        out<<v<<" "<<u<<" "<<b<<"\n";
        node_consumer.insert(pair<int,int>(v+1,u+1));
        need+=b;
        c[v+1][n-1]=b;
        //cout<<u<<","<<v<<","<<bw<<endl;
    }
    in.close();
    out.close();
    
}

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
    //清零
    memset(h, 0, sizeof(h));
    memset(e, 0 , sizeof(e));
    ev.clear();
    for(int i=0;i<=n;i++)
    {
        edge[i].clear();
        mydege[i].clear();
    }
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

maxflow_and_cost Push_Relable()
{
    maxflow_and_cost r;
    int maxflow=0;                      //存储最大流
    int cost=0;                         //存储对应的cost
    Init_PreFlow();
    while(!ev.empty())
    {
        int u = ev.front();
        Discharge(u);
        ev.pop_front();
        flag[u] = false;
    }
    
    for (int u = 0; u<n; u++) {
        for (int v = u+1; v<n; v++) {
            if(f[u][v]>0) {
                mydege[u].push_back(v);
                cost+=f[u][v]*price[u][v];
            }
            if(f[v][u]>0) {
                mydege[v].push_back(u);
                cost+=f[v][u]*price[v][u];
            }
        }
    }
    maxflow = e[n-1];
    r.maxflow = maxflow;
    r.cost = cost;
    return r;
}

void dosomething(vector<int>& p, int f1) {
    if(p.size()<2) return;
    int i=0,j=1;
    while (j<p.size()) {
        f[p[i]][p[j]] -= f1;
        //cout<<p[i]<<","<<p[j]<<","<<f[p[i]][p[j]]<<endl;
        if(f[p[i]][p[j]]==0) {
            auto it = mydege[p[i]].begin();
            while (it!=mydege[p[i]].end()) {
                if(*it==p[j]) {
                    mydege[p[i]].erase(it);
                    break;
                }
                it++;
            }
        }
        i++;
        j++;
    }
}

bool isempty(vector<int> tmp) {
    for (auto it = tmp.begin(); it!=tmp.end(); it++) {
        for (auto it2=mydege[*it].begin(); it2!=mydege[*it].end(); it2++) {
            if(f[*it][*it2]!=0) return false;
        }
    }
    return true;
}

list<path> getpath()
{
    list<path> r;
    path tmp;
    //while(!isempty(mydege[0]))
    //{
    
    int i=0;
    while(i<mydege[0].size())
    {
        vector<int>p;
        vector<int>q;
        p.push_back(0);
        if (mydege[i].size()>0)
        {
            q.push_back(f[0][mydege[0][i]]);
            int f1=10000;
            int num[N]={0};
            num[0]=i;
            int visit[N]={0};
            visit[0]=1;
            visit[mydege[0][i]]=1;
            p.push_back(mydege[0][i]);
            
            while(p.size()>1)
            {
                
                
                if(num[p.back()]<mydege[p.back()].size())
                {
                    if(mydege[p.back()][num[p.back()]]==n-1)
                    {
                        q.push_back(f[p.back()][mydege[p.back()][num[p.back()]]]);
                        p.push_back(mydege[p.back()][num[p.back()]]);
                        for(auto it =q.begin();it!=q.end();it++)
                        {
                            f1 = min(f1, *it);
                        }
                        dosomething(p, f1);
                        tmp.p = p;
                        tmp.f = f1;
                        r.push_back(tmp);
                        p.clear();
                        q.clear();
                        break;
                    }
                    else
                    {
                        if (visit[mydege[p.back()][num[p.back()]]]==0)
                        {
                            
                            visit[mydege[p.back()][num[p.back()]]]=1;
                            q.push_back(f[p.back()][mydege[p.back()][num[p.back()]]]);
                            int save=p.back();
                            
                            p.push_back(mydege[p.back()][num[p.back()]]);
                            num[save]++;
                        }
                        else
                            num[p.back()]++;
                    }
                }
                else
                {
                    //visit[mydege[p.back()][num[p.back()]-1]]=0;//-1
                    visit[p.back()]=0;
                    num[p.back()]=0;
                    
                    p.pop_back();
                    q.pop_back();
                    
                }
                
            }
        }
        else
            i++;
    }
    //}
    return r;
}



int main(int argc, char *argv[])
{
   // long time_used = 0;
   // struct timeval start, finish;
    char *topo_file = argv[1];
    char *result_file = argv[2];
    
    process_data(topo_file,result_file);
    get_H_tral();
    //auto it = Hierarchy_traversal.end();
//    cout<<Hierarchy_traversal.size()<<endl;
//    for(auto it= Hierarchy_traversal.begin();it!=Hierarchy_traversal.end();it++)
//        cout<<it->vextec<<" "<<it->Hierarchy<<","<<occurrence[it->vextec]<<endl;
    getsortByH_E();
    
    for(auto it=sortByH_E.begin();it!=sortByH_E.end();it++){
        cout<<*it<<endl;
    }
    getinitial();
    
//    initial r=getinitial();
//    cout<<"r="<<r.s<<endl;
//    if(r.s<consumerNum)
//    {
//        getTlist();
//        vector<int>solution=Tabu_search(r);
//        writeResult(result_file,solution);
//    }
//
//    gettimeofday(&start, NULL);
//    int maxflow= Push_Relable();
//    gettimeofday(&finish, NULL);
//    time_used = diff_in_us(&finish, &start);
//    printf("\n***CPU version time used %ld us!***\n\n",time_used);
//    printf("Max Flow is %d\n",maxflow);
//    
//    gettimeofday(&start, NULL);
//    auto r = getpath();
//    gettimeofday(&finish, NULL);
//    time_used = diff_in_us(&finish, &start);
//    printf("\n***CPU version time used by getpath is %ld us!***\n\n",time_used);
//    long sum=0;
//    for (auto it = r.begin(); it!=r.end(); it++) {
//                for (auto it2 = it->p.begin(); it2!=it->p.end(); it2++) {
//                    cout<<*it2<<"->";
//                }
//                cout<<"带宽："<<it->f<<endl;
//        sum+=it->f;
//    }
    
//    if(sum==maxflow)
//        cout<<"right!"<<endl;
//    
   
    
    //deploy_server(topo, line_num, result_file);
    
   // release_buff(topo, line_num);
    
    print_time("End");
    
    return 0;
}
