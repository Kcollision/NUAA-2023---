#include <iostream>
using namespace std;
#include <vector>
#include <queue>
#include <cstring>
const int N=1e4+10,M=1e5+10;
typedef pair<int, int> PII;
vector<int> g[N];//存图的邻接表
int n,m;
double bfs(int s)
{
    //编号，距离
    queue<PII> q;
    bool visited[N];
    memset(visited, false, sizeof(visited));
    int sumdist = 0,vercnt=0;
    q.push({s,0});
    visited[s] = true;
    while(q.size())
    {
        int len = q.size();
        vercnt += len;
        for (int i = 0; i < len;i++)
        {
            PII t= q.front();
            q.pop();
            int ver = t.first;
            int dist = t.second;
            sumdist += dist;
            for (size_t j = 0; j < g[ver].size();j++)
            {
                int next=g[ver][j];
                if(!visited[next]){
                    q.push({next, dist + 1});
                    visited[next] = true;
                }
            }
        }
    }
    if(vercnt==n){
        return (n-1)/(sumdist*1.0);
    }
    else{
        return -1;
    }
}
int main()
{
    cin>>n>>m;
    for(int i=0;i<m;i++)
    {
        int u,v;
        cin>>u>>v;
        g[u].push_back(v);
        g[v].push_back(u);
    }
    int t;
    cin >> t;
    while(t--)
    {
        int a;
        cin >> a;
        double cc = bfs(a);
        if(cc==-1){
            cout << "Cc(i)="<<"0.00" << endl;
        }
        else{
            printf("Cc(%d)=%.2lf\n",a,cc);
        }
    }
    system("pause");
    return 0;
}