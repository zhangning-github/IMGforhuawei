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

using namespace std;
const int COST=10000;
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
//int e[N]; // residual flow of the vertex
//int h[N]; // height of the vertex
int c[N][N]={0}; // capacity of the edge
int bandwidth[N][N]={0};  //每条边的带宽
int f[N][N]; // flow of the edge
//list<int> ev; // excess flow vertex
//list<int> edge[N]; // edge link list
vector<int> mydege[N];
bool flag[N]; // lable whether the vertex is in the flow list
extern vector<H_tral1> Hierarchy_traversal;    //存储层次遍历结果
vector<int>sortByH_E;// 以层次和边数来排序
vector<int> selected;     //被选为挂载服务器的节点
vector<PAIR>pair_vec;

struct Edge {
    int to, rev, cap, cost;
    Edge(int t, int c, int cc, int r) :to(t), cap(c), cost(cc), rev(r){}
};
vector<Edge>saveG[N];
vector<Edge>G[N];

const int inf = 0x33333333;


PAIR mcmf(int s, int t) {
    //init
    static int h[N];        //标号
    int flow = 0, cost = 0;
    //solve
    while(true) {
        //dijkstra init
        static int dist[N]; //距离
        static int pv[N];   //上一个顶点
        static int pe[N];   //上一条弧
        memset(dist, 0x33, sizeof dist);
        dist[s] = 0;
        priority_queue< PAIR, vector<PAIR>, greater<PAIR> > q;
        q.push(PAIR(0, s));
        //dijkstra
        while(!q.empty()) {
            PAIR x = q.top();
            q.pop();
            int u = x.second;
            if(x.first != dist[u]) {
                continue;
            }
            if(u == t) {
                break;
            }
            for(int i = 0; i < (int)G[u].size(); ++i) {
                Edge &e = G[u][i];
                int newcost = e.cost + h[u] - h[e.to];
                if(e.cap > 0 && dist[e.to] > dist[u] + newcost) {
                    dist[e.to] = dist[u] + newcost;
                    q.push(PAIR(dist[e.to], e.to));
                    pv[e.to] = u;
                    pe[e.to] = i;
                }
            }
        }
        if(dist[t] == inf) {
            break;
        }
        //augment
        for(int i = 0; i < n; ++i) {
            h[i] = min(h[i] + dist[i], inf);
        }
        int newflow = inf;
        for(int x = t; x != s; x = pv[x]) {
            Edge &e = G[pv[x]][pe[x]];
            newflow = min(newflow, e.cap);
        }
        flow += newflow;
        cost += newflow * h[t];
        for(int x = t; x != s; x = pv[x]) {
            Edge &e = G[pv[x]][pe[x]];
            e.cap -= newflow;
            G[x][e.rev].cap += newflow;
        }
    }
    return make_pair(flow, cost);
}

void addedge(int from, int to, int cap, int cost)
{
    G[from].push_back(Edge( to, cap, cost, (int)G[to].size()));
    G[to].push_back(Edge( from, 0, -cost, (int)G[from].size() - 1 ));
}


void init_graph(vector<int> select){
    clear_graph();
    for(int i=0;i<select.size();i++){
        addedge(0, select[i], COST,0);
        //G[0].push_back(Edge(select[i],10000,0,G[select[i]].size()));
    }
    for(int i=1;i<n;i++){
        for(int j=0;j<saveG[i].size();j++)
            addedge(i, saveG[i][j].to, saveG[i][j].cap,saveG[i][j].cost);
    }
    
}
void clear_graph(){
    for(int i=0;i<n;i++){
        G[i].clear();
    }
    memset(f,0,sizeof(int)*N*N);
    
    
    
}
void getf()
{
    for(auto it=G[0].begin();it!=G[0].end();it++){
        f[0][it->to]=COST-it->cap;
    }
    for(int i=1;i<n;i++) {
        for(auto it=G[i].begin();it!=G[i].end();it++){
            if(it->cost>=0)
                f[i][it->to]=c[i][it->to]-it->cap;
        }
    }
}
void getfAndPath()   //f and mydege
{
    for(auto it=G[0].begin();it!=G[0].end();it++){
        f[0][it->to]=COST-it->cap;
    }
    for(int i=1;i<n;i++) {
        for(auto it=G[i].begin();it!=G[i].end();it++){
            if(it->cost>=0)
                f[i][it->to]=c[i][it->to]-it->cap;
        }
    }
    for (int u = 0; u<n; u++) {
        for (int v = u+1; v<n; v++) {
            if(f[u][v]>0) {
                mydege[u].push_back(v);
            }
            if(f[v][u]>0) {
                mydege[v].push_back(u);
            }
        }
    }
}






long diff_in_us(struct timeval *finishtime, struct timeval * starttime)
{
    long usec;
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
        bandwidth[u+1][v+1]=b;
        bandwidth[v+1][u+1]=b;
        
        saveG[u+1].push_back(Edge(v+1,b,p,(int)saveG[v+1].size()));
        saveG[v+1].push_back(Edge(u+1,b,p,(int)saveG[u+1].size()));
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
        bandwidth[v+1][n-1]=b;
        saveG[v+1].push_back(Edge(n-1,b,0,saveG[n-1].size()));
        //cout<<u<<","<<v<<","<<bw<<endl;
        selected.push_back(v+1);
    }
    in.close();
    out.close();
    
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
            int f1=COST;
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

void writeresult(char * result_file)
{
    
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
        out<<node_consumer.at(it->p.back())-1<<" ";
        out<<it->f<<"\n";
    }
    
}

int main(int argc, char *argv[])
{
    print_time("Start");
    // long time_used = 0;
    struct timeval start, finish;
    //print_time("Start");
    char *topo_file = argv[1];
    char *result_file = argv[2];
    gettimeofday(&start, NULL);
    process_data(topo_file,result_file);
    if(n<700){
        get_H_tral();
        getsortByH_E();
    }
    double a=1.85,b=3;
    gettimeofday(&finish, NULL);
    initial r=getinitial(diff_in_us(&finish, &start),a,a*b);
    if(n>700)
    {
        init_graph(selected);
        PAIR result=mcmf(0, n-1);
        if(result.first==need&&(result.second+selected.size()<consumerNum*serverPrice)){
            getfAndPath();
            writeresult(result_file);
        }
    }
    
    // cout<<"初始解"<<r.s<<","<<r.cost+r.s*serverPrice<<"\n";
    //cout<<"直接挂载："<<consumerNum*serverPrice<<endl;
    if(n<700)
    {
        gettimeofday(&finish, NULL);
        if(n>200){
            vector<int> best=Tabu_search(r,diff_in_us(&finish, &start));
            init_graph(best);
            PAIR result=mcmf(0, n-1);
            if(result.first==need&&(result.second+best.size()<consumerNum*serverPrice)){
                getfAndPath();
                writeresult(result_file);
            }
        }
        else
        {
            vector<int> best=Tabu_search1(r,diff_in_us(&finish, &start));
            init_graph(best);
            PAIR result=mcmf(0, n-1);
            if(result.first==need&&(result.second+best.size()<consumerNum*serverPrice)){
                getfAndPath();
                writeresult(result_file);
            }
            
        }
        //        cout<<"best:"<<result.second+best.size()*serverPrice<<endl;
        //        cout<<"直接挂载："<<consumerNum*serverPrice<<endl;
    }
    
    
    print_time("End");
    
    return 0;
}
