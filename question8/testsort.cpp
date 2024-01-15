/*
直接插入排序、希尔排序，冒泡排序、快速排序、选择排序、堆排序，归并排序、基数排序
*/
#include <iostream>
using namespace std;
#include <vector>
#include <windows.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip>
class Sort{
public:
    int *data;
    int length;
public:
    Sort(int _length){
        length = _length;
        data = new int[length];
    }
    ~Sort(){
        delete []data;
    }
    int maxbit(){
        int maxnum = data[0];
        for (int i = 1; i < length;i++){
            if(data[i]>maxnum)
                maxnum = data[i];
        }
        int d = 1;
        while(maxnum>=10){
            maxnum /= 10;
            d++;
        }
        return d;
    }
    bool isNonDecreasing()
    {
        //是否是非递减序列（不严格递增序列）
        for (int i = 1; i < length;i++)
        {
            if(data[i]<data[i-1]){
                return false;
            }    
        }
        return true;
    }
    void CreateData(string filename);
    void ReadData(string filename);
    int GetLength(){
        return length;
    }
    //直接插入排序
    void DirectInsertSort();
    //希尔排序
    void ShellSort();
    //冒泡排序
    void BubbleSort();
    //普通快速排序
    int Partition(int l,int r);
    void QSort(int l,int r);
    void QuickSort(int l,int r);
    //改进快速排序
    void ImprovedQuickSort(int l, int r);
    //选择排序
    void SelectSort();
    //堆排序
    void HeapSort();
    void HeapAdjust(int low, int high);
    //归并排序
    void MergeSort();
    void Msort(int l, int r,int *temp);
    //基数排序
    void RadixSort(int d);
};

void Sort::CreateData(string filename)
{
    //10个样本，每个样本50000个数据
    srand(time(NULL));
    int n;
    //前两个文件为有序
    vector<int> v;
    for(int i = 0; i < length; i++){
        n = rand() % 60000;
        v.push_back(n);
    }
    sort(v.begin(), v.end());
    ofstream out1;
    out1.open(filename+to_string(1)+".txt", ios::out);
    //第一个文件从小到大
    int cnt = 1;
    for(int i = 0; i < length; i++){
        if(cnt<10){
            out1 << v[i] << ' ';
            cnt++;
        }
        else{
            out1 << v[i] << endl;
            cnt = 1;
        }
    }
    //第二个文件从大到小
    cnt = 1;
    ofstream out2;
    out2.open(filename+to_string(2)+".txt", ios::out);
    for(int i = length-1; i >= 0; i--){
        if(cnt<10){
            out2 << v[i] << ' ';
            cnt++;
        }
        else{
            out2 << v[i] << endl;
            cnt = 1;
        }
    }
    //8个文件为无序
    int filecnt = 10;
    while(filecnt>=3){
        cnt = 1;
        ofstream out;
        out.open(filename+to_string(filecnt)+".txt", ios::out);
        for(int i = 0; i < length; i++){
            n = rand() % 60000;
            if(cnt<10){
                out << n << ' ';
                cnt++;
            }
            else{
                out << n << endl;
                cnt = 1;
            }
        }
        out.close();
        filecnt--;
    }
}
void Sort::ReadData(string filename)
{
    ifstream in;
    in.open(filename, ios::in);
    int cnt = 0;
    while(!in.eof()){
        in >> data[cnt];
        cnt++;
    }
    in.close();
}
void Sort::DirectInsertSort()
{
    int temp;
    for (int i = 1; i < length;i++)
    {
        temp = data[i];
        int j = i - 1;
        while(j>=0&&data[j]>temp){
            data[j + 1] = data[j];
            j--;
        }
        data[j + 1] = temp;
    }
}
void Sort::ShellSort()
{
    //增量序列为d1=5,d2=3,d3=1
    vector<int> d = {5, 3, 1};
    for (size_t k = 0; k < d.size();k++){
        int dk = d[k];
        for (int i = dk; i < length;i++){
            int temp = data[i];
            int j = i - dk;
            while(j>=0&&data[j]>temp){
                data[j + dk] = data[j];
                j = j - dk;
            }
            data[j + dk] = temp;
        }
    }
}
void Sort::BubbleSort()
{
    for (int i = 0; i < length;i++){
        bool flag = false;
        for (int j = 0; j < length - i - 1;j++){
            if(data[j]>data[j+1]){
                int temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
                flag = true;
            }
        }
        if(!flag)
            break;
    } 
}
int Sort::Partition(int l,int r)
{
    int temp = data[l];
    while(l<r){
        while(l<r&&data[r]>=temp)
            r--;
        data[l] = data[r];
        while(l<r&&data[l]<=temp)
            l++;
        data[r] = data[l];
    }
    data[l] = temp;
    return l;
}

void Sort::QSort(int l,int r)
{
    if(l<r){
        int i = Partition(l, r);
        if(i-1-l<r-i-1)
        {
            QSort(l, i - 1);
            QSort(i + 1, r);
        }
        else
        {
            QSort(i + 1, r);
            QSort(l, i - 1);
        }
    }
}
void Sort::QuickSort(int l,int r)
{
    QSort(l, r);
}
void Sort::ImprovedQuickSort(int l,int r)
{
    if(l>=r)
        return;
    int i = l - 1, j = r + 1, x = data[(l + r) >> 1];
    while(i<j){
        do{
            i++;
        } while (data[i] < x);
        do{
            j--;
        } while (data[j] > x);
        if(i<j){
            int temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    }
    ImprovedQuickSort(l, j);
    ImprovedQuickSort(j + 1, r);
}
void Sort::SelectSort()
{
    for (int i = 0; i < length;i++){
        int k = i;
        for (int j = i + 1; j < length;j++){
            if(data[j]<data[k])
                k = j;
        }
        if(k!=i){
            int temp = data[i];
            data[i] = data[k];
            data[k] = temp;
        }
    }
}
void Sort::HeapAdjust(int low,int high)
{
    int temp=data[low];
    int i=low,j=2*i;
    //当i存在左孩子时
    while(j<=high){
        //如果右孩子存在且右孩子大于左孩子,选右孩子
        if(j+1<=high && data[j+1]>data[j])
            j++;
        //已经是大顶堆
        if(temp>=data[j])
            break;
        //将孩子结点上移(上浮操作)
        data[i]=data[j];
        i=j;
        j=2*i;
    }
    //将temp放在最终位置
    data[i]=temp;
}
void Sort::HeapSort()
{
    //建立大顶堆
    for (int i = (length-1) / 2; i >= 0;i--)
    {
        HeapAdjust(i, length - 1);
    }
    //堆排序
    for (int i = length - 1; i >= 1;i--){
        //将堆顶元素与堆中最后一个元素交换
        int temp = data[0];
        data[0] = data[i];
        data[i] = temp;
        //将剩余元素重新调整为大顶堆
        HeapAdjust(0, i - 1);
    }
}
void Sort::Msort(int l, int r,int *temp)
{
    if(l>=r)
        return;
    int mid = (l + r) >> 1;
    Msort(l, mid, temp);
    Msort(mid + 1, r, temp);
    int i = l, j = mid + 1, k = 0;
    while(i<=mid&&j<=r){
        if(data[i]<=data[j])
            temp[k++] = data[i++];
        else
            temp[k++] = data[j++];
    }
    while(i<=mid)
        temp[k++] = data[i++];
    while(j<=r)
        temp[k++] = data[j++];
    for (int i = l, j = 0; i <= r;i++,j++){
        data[i] = temp[j];
    }
}
void Sort::MergeSort()
{
    int *temp = new int[length];
    Msort(0, length-1,temp);
    delete []temp;
}
void Sort::RadixSort(int d)
{
    //d为最大位数
    int *temp = new int[length];
    int *cnt = new int[10];
    int radix = 1;
    for (int i = 1; i <= d;i++)
    {
        //初始化cnt数组为0
        for (int j = 0; j < 10;j++)
            cnt[j] = 0;
        //将data中的元素映射到桶cnt
        for (int j = 0; j < length;j++)
        {
            int k = (data[j] / radix) % 10;
            cnt[k]++;
        }
        //计算前缀和
        //从而得知位置
        for (int j = 1; j <= 9;j++){
            cnt[j] += cnt[j - 1];
        }
        //将桶中记录收集到temp中
        for (int j = length - 1; j >= 0;j--)
        {
            int k = (data[j] / radix) % 10;
            temp[cnt[k] - 1] = data[j];
            cnt[k]--;
        }
        //temp移到data
        for (int j = 0; j < length;j++)
            data[j] = temp[j];
        radix *= 10;
    }
    delete[] temp;
    delete[] cnt;
}
int main()
{
    Sort s(50000);
    string filename = "input";
    s.CreateData(filename);
    cout<<"创建数据完成,开始测试"<<endl;
    cout.setf(ios::left);
    cout << setw(20) << "排序方法";
    cout << setw(15) << "input1.txt" << setw(15) << "input2.txt" << setw(15) << "input3.txt" << setw(15) << "input4.txt"
    << setw(15) << "input5.txt" << setw(15) << "input6.txt" << setw(15) << "input7.txt" << setw(15) << "input8.txt" << setw(15) << "input9.txt"
    << setw(15) << "input10.txt" << endl;
    cout<<setw(20)<<"直接插入排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.DirectInsertSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "希尔排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.ShellSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "冒泡排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.BubbleSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "改进快速排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.ImprovedQuickSort(0, s.GetLength() - 1);
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "选择排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.SelectSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "堆排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.HeapSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "归并排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.MergeSort();
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl;
    cout << setw(20) << "基数排序：";
    for (int i = 1;i<=10;i++)
    {
        s.ReadData(filename + to_string(i) + ".txt");
        int d = s.maxbit();
        LARGE_INTEGER t1, t2, tc;
        QueryPerformanceFrequency(&tc);
        QueryPerformanceCounter(&t1);
        s.RadixSort(d);
        QueryPerformanceCounter(&t2);
        auto time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
        double milliseconds = time * 1000.0;
        string str = to_string(milliseconds)+"ms";   
        cout << setw(15)<<str;
    }
    cout << endl; 
    system("pause");
    return 0;
}