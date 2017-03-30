# IMGforhuawei

1.修改了getpath()
//visit[mydege[p.back()][num[p.back()]-1]]=0;//-1
visit[p.back()]=0;
num[p.back()]=0;

2.void Init_PreFlow()

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
