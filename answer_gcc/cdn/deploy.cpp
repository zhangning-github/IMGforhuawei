#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>

using namespace std;
struct edge {
    char start;
    char end;
    char cost;
    char bandwidth;
    char CostperM;
};
struct consumerlist {
    edge* myedge;
};
struct depot {
    char exchange;
    char* consumerlist;
};
/*
 res存储n行（n为consumer个数）。
 每行内容：消费节点ID，相连网络ID，总带宽，单位网络租用费用，视频带宽需求
 */
depot* getintialSolution(string res){
    depot* r;
    return r;
}
depot* improvement(){
    depot* r;
    return r;
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
