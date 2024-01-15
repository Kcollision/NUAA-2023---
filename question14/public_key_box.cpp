#include <iostream>
using namespace std;
#include <vector>
#include <set>
#include <algorithm>
#include <map>
const int N = 1010,M=10200;
int n,k;
int ans[N];
vector<int> startt[M],endt[M];
set<int> se;//存储钥匙位置空缺处
map<int,int> loc;//获取某把钥匙当前的位置
int main()
{
    cin>>n>>k;
    int maxtime=0;
    while(k--)
    {
        int w,s,c;
        cin>>w>>s>>c;
        maxtime=max(maxtime,s+c);
        startt[s].push_back(w);
        endt[s+c].push_back(w);
    }
    for(int i=1;i<=n;i++)
        loc[i]=i;
    for(int i=1;i<=maxtime;i++)
    {
        if(endt[i].size()){
            //有很多还钥匙的人，按照钥匙编号从小到大排序
            sort(endt[i].begin(),endt[i].end());
            for(auto x:endt[i]){
                if(se.size()){
                    //取出最小的空缺的位置
                    int nowloc=*(se.begin());
                    ans[nowloc]=x;
                    //更新钥匙位置
                    loc[x]=nowloc;
                    //将此空缺位置删除
                    se.erase(se.begin());
                }
            }
        }
        if(startt[i].size())
        {
            for(auto x:startt[i]){
                se.insert(loc[x]);
                loc[x]=0;
            } 
        }
    }
    for(int i=1;i<=n;i++)
    {
        if(!ans[i]){
            cout<<i<<" ";
        }
        else{
            cout<<ans[i]<<" ";
        }
    }
    cout<<endl;
    return 0;
}