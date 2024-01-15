#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;
const int N = 35;
int n,m;
int g[N][N];
int g1[N][N];
int main()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            cin>>g[i][j];
            g1[i][j]=g[i][j];
        }
    }
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            int a=g1[i][j];
            int x=i,y=j;
            if(y+2<=m && g1[x][y+1]==a && g1[x][y+2]==a){
                while(y<=m && g1[x][y]==a){
                    g[x][y]=0;
                    y+=1;
                }
            }
            x=i;
            y=j;
            if(x+2<=n && g1[x+1][y]==a && g1[x+2][y]==a){
                while(x<=n && g1[x][y]==a){
                    g[x][y]=0;
                    x+=1;
                }
            }
        }
    }
    for(int i=1;i<=n;i++)
    {
        for(int j=1;j<=m;j++)
        {
            cout<<g[i][j]<<" ";
        }
        cout<<endl;
    }
    return 0;
}