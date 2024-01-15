#include <iostream>
using namespace std;
const int N = 15;
struct window{
    int cnt, x1, y1, x2, y2;
}w[N];
int n, m;
int main()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++)
    {
        cin>>w[i].x1>>w[i].y1>>w[i].x2>>w[i].y2;
        w[i].cnt=i;
    }
    int x, y;
    while(m--)
    {
        cin >> x >> y;
        int k=-1;
        window temp;
        int i;//记录是第几个窗口
        for (i = n; i ;i--)
        {
            if(x>=w[i].x1&&y>=w[i].y1&&x<=w[i].x2&&y<=w[i].y2)
            {
                k = w[i].cnt;//记录窗口编号
                temp=w[i];//记录窗口
                break;
            }
        }
        if(k==-1){
            cout<<"IGNORED"<<endl;
        }
        else{
            cout<<k<<endl;
            //注意区分初始值是窗口编号还是第几个窗口
            for(int j=i;j<n;j++)
            {
                w[j] = w[j+1];
            }
            w[n] = temp;
        }
    }
    system("pause");
    return 0;
}