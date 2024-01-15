#include <iostream>
using namespace std;
#include <algorithm>
int cnt[1010];
pair<int,int> ans[1010];//<数字,次数>
bool cmp(pair<int,int> &a,pair<int,int> &b)
{
    if(a.second!=b.second){
        return a.second>b.second;
    }
    return a.first<b.first;
}
int main()
{
    int n;
    cin>>n;
    int truelen=0;
    for(int i=0;i<n;i++){
        int a;
        cin>>a;
        cnt[a]++;
    }
    
    for(int i=0;i<1010;i++){
        if(cnt[i]){
            ans[truelen++]={i,cnt[i]};
        }
    }
    sort(ans,ans+truelen,cmp);
    for(size_t i=0;i<truelen;i++){
        cout<<ans[i].first<<" "<<ans[i].second<<endl;
    }
    system("pause");
    return 0;
}