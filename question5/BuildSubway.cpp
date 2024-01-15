#include <iostream>
using namespace std;
#include <vector>
#include <algorithm>
const int N = 100000+10,M=200000+10;
int n, m;
struct edge
{
    int a, b, w;
}e[2*M];

bool cmp(edge a,edge b)
{
    return a.w < b.w;
}
int p[N];
int find(int x)
{
    if(p[x]!=x){
        p[x]=find(p[x]);
    }
    return p[x];
}
int main()
{
    cin >> n >> m;
    for (int i = 0; i < m;i++)
    {
        int a, b,w;
        cin >> a >> b>>w;
        e[i] = {a, b, w};
    }
    sort(e, e + m,cmp);
    for (int i = 1; i <= n;i++)
    {
        p[i] = i;
    }
    int ans = -1;
    for (int i = 0; i < m;i++)
    {
        int a = find(e[i].a), b = find(e[i].b);
        if(a!=b){
            p[a] = b;
            ans = max(ans, e[i].w);
        }
        if(find(1)==find(n)){
            break;
        }
    }
    cout << ans << endl;
    system("pause");
    return 0;
}