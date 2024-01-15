#include <iostream>
using namespace std;
#include <vector>
#include <algorithm>
const int N = 1e6 + 10;
typedef long long LL;
int n, m;
LL a[N], b[N];
LL a_[N], b_[N];
bool cmp(LL a,LL b)
{
    return a > b;
}
int main()
{
    cin >> n;
    int positivea = 0, negativea = 0;
    for (int i = 0; i < n;i++)
    {
        LL t;
        cin >> t;
        if(t>0)
            a[positivea++]=t;
        else{
            a_[negativea++] = t;
        }
    }
    cin >> m;
    int positiveb = 0;
    int negativeb = 0;
    for (int i = 0; i < m;i++)
    {
        LL t;
        cin >> t;
        if(t>0)
            b[positiveb++]=t;
        else{
            b_[negativeb++] = t;
        }
    }
    LL ans = 0;
    sort(a,a+positivea,cmp);
    sort(b,b+positiveb,cmp);
    sort(a_,a_+negativea);
    sort(b_,b_+negativeb);
    for (int i = 0; i < positivea&&i<positiveb;i++)
    {
        ans += a[i] * b[i];
    }
    for (int i = 0; i < negativea&&i<negativeb;i++)
    {
        ans += a_[i] * b_[i];
    }
    cout << ans << endl;
    system("pause");
    return 0;
}