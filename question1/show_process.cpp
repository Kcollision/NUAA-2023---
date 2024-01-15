/*
如果不是VS 
请在编译链接里加上 -lpsapi
即g++ test.cpp -o test.exe -lpsapi
如果是VS
直接加上#pragma comment(lib,"Psapi.lib")
即可
*/
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iomanip>
using namespace std;
typedef long long ll;
void SetColor(string str);
class process{
private:
    //储存当前进程的单向链表
    typedef struct NProcess_node{
        int pid;//进程id
        string pname;//进程名
        SYSTEMTIME duration;//持续时间
        ll memory;//内存使用情况
        struct NProcess_node *next;
        NProcess_node(){
            next=nullptr;
        }
        NProcess_node(int id,string name,SYSTEMTIME dur,int mem){
            pid=id;
            duration=dur;
            pname=name;
            memory=mem;
            next=nullptr;
        }
    }NProcess_node,*NProcess;

    //储存已完成进程的双向链表
    typedef struct EProcess_node{
        int pid;//进程id
        string pname;//进程名
        SYSTEMTIME endtime;//结束时间
        SYSTEMTIME duration;//持续时间
        struct EProcess_node *next,*prior;
        EProcess_node(){
            next=nullptr;
            prior=nullptr;
        }
        EProcess_node(int id,string name,SYSTEMTIME endtime,SYSTEMTIME duration){
            pid=id;
            this->endtime=endtime;
            this->duration=duration;
            pname=name;
            next=nullptr;
            prior=nullptr;
        }
    }EProcess_node,*EProcess;
private:
    NProcess sum_pc;//总进程链表
    NProcess cur_pc;//当前进程
    EProcess end_pc;//已完成进程

public:
    process();
    ~process();
    bool create_nprocess_list(NProcess &pc);//创建当前进程链表
    void show_process_info(NProcess curpc, EProcess endpc);
    void insert_endprocess_list(EProcess &pc,EProcess_node *p);//向已完成进程链表（有序）插入节点
    void select_sort_byMem(NProcess& pc);//选择排序

    bool is_in_process(NProcess &pc, int pid);
    bool is_in_process(EProcess &pc, int pid);

    void delete_process_list(NProcess &pc);
    void delete_process_list(EProcess &pc);

    void UpdateSumProcess(NProcess& sumpc,NProcess& curpc,EProcess &endpc);
    void UpdateEndProcess(EProcess &endpc,NProcess &curpc);
public:
    int GetMemoryInfo(DWORD processID);//获取进程内存信息
    void GetProcessStartTime(DWORD processID, FILETIME& createTime);//获取进程开始时间
    int comp_time(SYSTEMTIME start,SYSTEMTIME end);//比较时间大小
    SYSTEMTIME GetProcessDuration(FILETIME& startTime);//获取进程持续时间
    void test();//测试函数
};
process::process(){
    sum_pc=new NProcess_node();
    cur_pc=new NProcess_node();
    end_pc=new EProcess_node();
}
process::~process(){
    delete_process_list(end_pc);
    delete_process_list(sum_pc);
    delete_process_list(cur_pc);
    delete sum_pc;
    delete cur_pc;
    delete end_pc;
}
void process::GetProcessStartTime(DWORD processID, FILETIME& createTime) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);

    if (hProcess != NULL) {
        FILETIME dummy, exitTime, kernelTime;
        GetProcessTimes(hProcess, &createTime, &dummy, &kernelTime, &exitTime);

        CloseHandle(hProcess);
    }
}

SYSTEMTIME process::GetProcessDuration(FILETIME& startTime) {
    SYSTEMTIME systemTime;
    FILETIME currentTime;
    
    // 获取当前时间
    GetSystemTimeAsFileTime(&currentTime);

    // 计算进程运行时间
    ULARGE_INTEGER start, current;
    start.LowPart = startTime.dwLowDateTime;
    start.HighPart = startTime.dwHighDateTime;
    current.LowPart = currentTime.dwLowDateTime;
    current.HighPart = currentTime.dwHighDateTime;

    ULONGLONG elapsedTime = current.QuadPart - start.QuadPart;

    // 将时间间隔转换为 SYSTEMTIME
    ULONGLONG intervals = elapsedTime / 10000000; // 转换单位为秒
    systemTime.wHour = intervals / 3600;
    systemTime.wMinute = (intervals % 3600) / 60;
    systemTime.wSecond = (intervals % 60);
    systemTime.wMilliseconds = 0;

    return systemTime;
}
bool process::create_nprocess_list(NProcess &pc){
    //获取当前系统进程快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cout << "不能获取进程！" << std::endl;
        return false;
    }
    //存放进程信息的结构体
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    //获取进程
	BOOL bMore = Process32First(snapshot, &entry);
	while (bMore) {
		//获得当前进程的内存量
		int judge = GetMemoryInfo(entry.th32ProcessID);
		if (judge) {
            string str(entry.szExeFile);
            /*
            VS请改为以下:
            wstring wstrValue(entry.szExeFile);
            string str(wstrValue.begin(), wstrValue.end());
            */
            //获取运行时间
            FILETIME startTime;
            GetProcessStartTime(entry.th32ProcessID, startTime);
            SYSTEMTIME dur=GetProcessDuration(startTime);

            NProcess_node *p = new NProcess_node(entry.th32ProcessID,str, dur, judge);
            //将进程头插法加入链表
            p->next = pc->next;
            pc->next = p;
		}
		bMore = Process32Next(snapshot, &entry);
	}
    CloseHandle(snapshot);
    return true;
}
int process::GetMemoryInfo(DWORD processID)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    //获取进程句柄
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL == hProcess)
        return 0;
    //获取进程内存信息
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        //返回进程使用内存
        return pmc.WorkingSetSize;
    }
    return 0;
}
void process::select_sort_byMem(NProcess& pc)
{
    NProcess pre = pc, p = pc->next;
    bool flag = false;
    while(p)
    {
        NProcess min_node = p, q = p->next, min_prev = pre, q_prev = p;
        while(q)
        {
            if(q->memory < min_node->memory){
                min_prev = q_prev;
                min_node = q;
            }
            q_prev = q;
            q = q->next;
        }
        if(min_node!=p){
            min_prev->next = min_node->next;
            min_node->next = pc->next;
            pc->next = min_node;
            if(!flag){
                pre = min_node;
                flag = true;
            }
            p = p;
        }
        else{
            pre->next = p->next;
            p->next = pc->next;
            pc->next = p;
            p=pre->next;
        }
    }
}
void process::show_process_info(NProcess curpc,EProcess endpc){
    if(curpc->next==nullptr){
        cout<<"****当前无进程****"<<endl;
        return;
    }
    else{
        SetColor("green");
        for (int i = 0; i < 66;i++)
            cout << "-";
        cout << "开始打印当前进程";
        for (int i = 0; i < 52;i++)
            cout << "-";
        cout << endl;
        SetColor("white");
        cout<<setw(40)<<"进程名"<<setw(10)<<"进程id"<<setw(15)<<"内存使用情况"<<setw(15)<<"持续时间"<<endl;

        for (int i = 0; i < 134;i++)
            cout << "-";
        cout << endl;

        NProcess_node *p=curpc->next;
        while(p!=nullptr){
            cout << setw(40) << p->pname
                 << setw(10) << p->pid
                 << setw(12) << p->memory << " bytes";
            string hourtime = to_string(p->duration.wHour);
            string minutetime = to_string(p->duration.wMinute);
            string secondtime = to_string(p->duration.wSecond); 
            string durtime=hourtime.append("时").append(minutetime).append("分").append(secondtime).append("秒");
            cout << setw(15)<<durtime<<endl;
            p=p->next;
        }
        for (int i = 0; i < 70;i++)
            cout << "-";
        cout << "end";
        for (int i = 0; i < 67; i++)
            cout<<"-";
    }
    cout<<endl;
    if(endpc->next==nullptr){
        cout<<"****没有已结束进程****"<<endl;
        return;
    }
    else{
        SetColor("red");
        for (int i = 0; i < 66;i++)
            cout << "-";
        cout << "开始打印已结束进程";
        for (int i = 0; i < 56;i++)
            cout << "-";
        cout << endl;
        SetColor("white");
        cout<<setw(40)<<"进程名"<<setw(10)<<"进程id"<<setw(30)<<"结束时间"<<setw(15)<<"持续时间"<<endl;

        for (int i = 0; i < 140;i++)
            cout << "-";
        cout << endl;
        EProcess_node *p=endpc->next;
        while(p!=nullptr){
            cout << setw(40) << p->pname << setw(10) << p->pid;
            string yeartime = to_string(p->endtime.wYear);
            string monthtime = to_string(p->endtime.wMonth);
            string daytime = to_string(p->endtime.wDay);
            string hourtime = to_string(p->endtime.wHour);
            string minutetime = to_string(p->endtime.wMinute);
            string secondtime = to_string(p->endtime.wSecond);
            string endtime = yeartime.append(".").append(monthtime).append(".").append(daytime).append(" ").append(hourtime)
            .append("时").append(minutetime).append("分").append(secondtime).append("秒");
            cout << setw(30) << endtime;
            hourtime = to_string(p->duration.wHour);
            minutetime = to_string(p->duration.wMinute);
            secondtime = to_string(p->duration.wSecond); 
            string durtime=hourtime.append("时").append(minutetime).append("分").append(secondtime).append("秒");
            cout << setw(15) << durtime<<endl;
            
            p=p->next;
        }
        for (int i = 0; i < 66;i++)
            cout << "-";
        cout << "打印完成";
        for (int i = 0; i < 66; i++)
            cout<<"-";
    }
}
void process::insert_endprocess_list(EProcess &pc,EProcess_node *p){
    //按持续时间从小到大排序
    EProcess_node *q = pc->next, *pre = pc;
    
    while(q!=nullptr&&comp_time(q->duration,p->duration)<=0){
        pre = q;
        q=q->next;
    }
    if(q==nullptr){
        pre->next = p;
        p->prior = pre;
        p->next = nullptr;
    }
    else{
        pre->next = p;
        p->prior = pre;
        p->next = q;
        q->prior = p;
    }
}
bool process::is_in_process(NProcess &pc, int pid){
    NProcess_node *p = pc->next;
    while(p!=nullptr){
        if(p->pid==pid)
            return true;
        p=p->next;
    }
    return false;
}
bool process::is_in_process(EProcess &pc, int pid){
    EProcess_node *p = pc->next;
    while(p!=nullptr){
        if(p->pid==pid)
            return true;
        p=p->next;
    }
    return false;
}
void process::delete_process_list(EProcess &pc)
{
    EProcess_node *p = pc->next;
    while(p!=nullptr){
        pc->next = p->next;
        delete p;
        p = pc->next;
    }
}
void process::delete_process_list(NProcess &pc)
{
    NProcess_node *p = pc->next;
    while(p!=nullptr){
        pc->next = p->next;
        delete p;
        p = pc->next;
    }
}
void process::UpdateSumProcess(NProcess& sumpc,NProcess& curpc, EProcess& endpc)
{
    //如果有总进程不在当前进程中，那么添加到结束进程,并从总进程中删除
    NProcess_node *p = sumpc->next;
    while(p!=nullptr)
    {
        if(!is_in_process(curpc,p->pid))
        {
            //将该进程添加到结束进程中
            //获取当前时间
            SYSTEMTIME endtime;
            GetLocalTime(&endtime);
            //当前时间就是该进程结束时间
            EProcess_node *q = new EProcess_node(p->pid,p->pname,endtime,p->duration);
            //将该进程添加到结束进程中
            insert_endprocess_list(end_pc,q);
            //从总进程中删除
            NProcess_node *pre = sumpc;
            while(pre->next!=p)
                pre = pre->next;
            pre->next = p->next;
            delete p;
            p = pre->next;
        }
        else
            p = p->next;
    }
    //更新总进程中为当前进程
    delete_process_list(sumpc);
    p = curpc->next;
    while(p!=nullptr)
    {
        NProcess_node *q = new NProcess_node(p->pid,p->pname,p->duration,p->memory);
        q->next = sumpc->next;
        sumpc->next = q;
        p = p->next;
    }
}
void process::UpdateEndProcess(EProcess &endpc,NProcess &curpc)
{
    //如果当前进程中有结束进程，将其从结束进程中删除
    NProcess_node *p = curpc->next;
    while(p!=nullptr)
    {
        EProcess_node *q = endpc->next, *q_pre = endpc;
        while(q!=nullptr)
        {
            if(q->pid==p->pid)
            {
                q_pre->next = q->next;
                delete q;
                break;
            }
            else{
                q_pre = q;
                q = q->next;
            }
        }
        p = p->next;
    }
}
int process::comp_time(SYSTEMTIME a,SYSTEMTIME b)
{
    if(a.wHour>b.wHour)
        return 1;
    else if(a.wHour<b.wHour)
        return -1;
    else
    {
        if(a.wMinute>b.wMinute)
            return 1;
        else if(a.wMinute<b.wMinute)
            return -1;
        else
        {
            if(a.wSecond>b.wSecond)
                return 1;
            else if(a.wSecond<b.wSecond)
                return -1;
            else
                return 0;
        }
    }
}
void process::test()
{
    create_nprocess_list(cur_pc);
    select_sort_byMem(cur_pc);
    UpdateSumProcess(sum_pc,cur_pc,end_pc);
    UpdateEndProcess(end_pc,cur_pc);
    show_process_info(cur_pc,end_pc);
    while(1){
            Sleep(1000);
            system("cls");
            delete_process_list(cur_pc);
            create_nprocess_list(cur_pc);
            select_sort_byMem(cur_pc);
            UpdateSumProcess(sum_pc,cur_pc,end_pc);
            UpdateEndProcess(end_pc,cur_pc);
            show_process_info(cur_pc,end_pc);
    }
}
// 设置控制台颜色
void SetColor(string str)
{
    if (str == "red")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    }
    else if (str == "green")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }
    else if (str == "blue")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    }
    else if (str == "yellow")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    }
    else if (str == "purple")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
    }
    else if (str == "cyan")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    else if (str == "white")
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // 设置三色相加:白色
    }
}
int main() {
    SetColor("white");
    process p;
    p.test();
    system("pause");
    return 0;
}