#include <iostream>
using namespace std;
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <streambuf>
#include <vector>
#include <windows.h>
#include <algorithm>
//待发掘VIP的个人信息
struct VIPPersonInfo
{
    string id;     // 18位身份证号
    string name;   // 姓名
    int cnt;       // 此人的乘机次数
    double milage; // 里程
    VIPPersonInfo()
    {
        id = "";
        name = "";
        cnt = 0;
        milage = 0;
    }
    VIPPersonInfo(string i, string n, int c, double m)
    {
        id = i;
        name = n;
        cnt = c;
        milage = m;
    }
};
//原始个人信息
struct PersonInfo
{
    string id;          // 18位身份证号
    string name;        // 姓名
    string flight_id;   // 航班号
    string flight_date; // 航班日期
    double milage;      // 里程
    int cnt;            // 乘机次数
    PersonInfo()
    {
        id = "";
        name = "";
        flight_id = "";
        flight_date = "";
        milage = 0;
        cnt = 0;
    }
};

//链地址法的哈希表
class HT_ChainAddress
{
private:
    struct Node
    {
        VIPPersonInfo data;
        Node *next;
        Node()
        {
            next = nullptr;
        }
        Node(VIPPersonInfo d)
        {
            data = d;
            next = nullptr;
        }
    };

public:
    Node **ht;//哈希表，每一个元素储存一个链表的头结点Node *类型
    int tablesize;  // 表长
    vector<int> clashtimes; //大小为冲突次数，值为每次冲突时进行重定位的次数
public:
    //friend class VIP;
    HT_ChainAddress(int size)
    {
        tablesize = size;
        ht = new Node *[tablesize];
        //初始化链表的空头结点
        for (int i = 0;i<tablesize;i++)
        {
            ht[i]=new Node();
            ht[i]->next = nullptr;
        }
        clashtimes.clear();
    }
    ~HT_ChainAddress()
    {
        for (int i = 0; i < tablesize; i++)
        {
            Node *p = ht[i];
            while (p)
            {
                Node *temp = p;
                p = p->next;
                delete temp;
            }
        }
        delete[] ht;
    }
    int HashFunction(string id)
    {
        int num1 = id[14] - '0';
        int num2 = id[15] - '0';
        int num3 = id[16] - '0';
        int num4 = (id[17] == 'X') ? 10 : (id[17] - '0');
        int ans = (num1 * 1 + num2 * 10 + num3 * 100 + num4 * 1000) % tablesize;
        return ans % tablesize;
    }
    void Insert(VIPPersonInfo data)
    {
        int index = HashFunction(data.id);
        Node* p = ht[index]->next;
        //如果链表为空，直接插入
        if(p==nullptr){
            p = new Node(data);
            p->next = nullptr;
            ht[index]->next = p;
        }
        else{
            int redirecttimes = 0;//重定位次数
            while(p){
                if(p->data.id==data.id){
                    p->data.cnt++;
                    p->data.milage+=data.milage;
                    return;
                    //如果找到相同的元素，直接返回，这不是冲突
                }
                redirecttimes++;
                p = p->next;
            }
            clashtimes.push_back(redirecttimes);
            //如果链表不为空，且没有找到相同的元素，此时代表冲突
            //采用头插法插入元素
            p = new Node(data);
            p->next = ht[index]->next;
            ht[index]->next = p;
        }
    }
    VIPPersonInfo Search(string id)
    {
        int index = HashFunction(id);
        Node *p = ht[index]->next;
        while (p)
        {
            if (p->data.id == id)
                return p->data;
            p = p->next;
        }
        return {"", "", 0, 0};
    }
    void Print()
    {
        cout<<"冲突次数："<<clashtimes.size()<<endl;
        for(size_t i=0;i<clashtimes.size();i++){
            cout<<"第"<<i+1<<"次冲突，进行了"<<clashtimes[i]<<"次重定位"<<endl;
        }
    }
};
//开放地址法的哈希表
//采用线性探测法
class HT_OpenAddress
{
private:
    VIPPersonInfo *ht;
    int tablesize;
    vector<int> clashtimes; //大小为冲突次数，值为每次冲突时进行重定位的次数
public:
    friend class VIP;
    HT_OpenAddress(int size)
    {
        tablesize=size;
        ht=new VIPPersonInfo[tablesize];
    }
    ~HT_OpenAddress()
    {
        delete[] ht;
    }
    int HashFunction(string id)
    {
        int num1 = id[14] - '0';
        int num2 = id[15] - '0';
        int num3 = id[16] - '0';
        int num4 = (id[17] == 'X') ? 10 : (id[17] - '0');
        int ans = (num1 * 1 + num2 * 10 + num3 * 100 + num4 * 1000) % tablesize;
        return ans % tablesize;
    }
    void Insert(VIPPersonInfo data)
    {
        int index = HashFunction(data.id);
        //如果该位置为空，直接插入
        if(ht[index].id==""){
            ht[index] = data;
            return;
        }
        else{
            //如果该位置不为空，但id相同，直接更新
            if(ht[index].id==data.id){
                ht[index].cnt++;
                ht[index].milage+=data.milage;
                return;
            }
            //如果该位置不为空，且id不同，进行重定位
            else if(ht[index].id!=data.id){
                //cout<<"冲突"<<ht[index].id<<" "<<data.id<<endl;
                int redirecttimes = 0;
                while(ht[index].id!=""){
                    index = (index+1)%tablesize;
                    //在重定位的过程中，如果找到相同的元素，直接更新,且重定位次数作废
                    if(ht[index].id==data.id){
                        ht[index].cnt++;
                        ht[index].milage+=data.milage;
                        return;
                    }
                    redirecttimes++;
                }
                clashtimes.push_back(redirecttimes);
                ht[index] = data;
            }
        }
    }
    VIPPersonInfo Search(string id)
    {
        int index = HashFunction(id);
        while(ht[index].id!=id){
            index = (index+1)%tablesize;
        }
        return ht[index];
    }
    void Print()
    {
        cout<<"冲突次数："<<clashtimes.size()<<endl;
        for(size_t i=0;i<clashtimes.size();i++){
            cout<<"第"<<i+1<<"次冲突，进行了"<<clashtimes[i]<<"次重定位"<<endl;
        }
    }
};

class VIP
{
private:
    int personcnt;//记录人数
    PersonInfo *recordlist;//存储每一条记录，记录之间的姓名和id不互相重复
    int TABLESIZE=223;//哈希表的大小
    HT_ChainAddress *ht1;//链地址法构建的哈希表
    HT_OpenAddress *ht2;//开放寻址法构建的哈希表
    double avgmilage;//平均里程数
    double avgcnt;//平均乘坐次数
    vector<string> sorted_by_milege;//按里程数排序后的id
    vector<string> sorted_by_cnt;//按乘坐次数排序后的id
public:
    VIP()
    {
        ht1=new HT_ChainAddress(TABLESIZE);
        ht2=new HT_OpenAddress(TABLESIZE);
        recordlist=new PersonInfo[200];//最多200个人
        personcnt = 0;
    }
    ~VIP()
    {
        delete[] recordlist;
        delete[] ht1;
        delete[] ht2;
    }
    void ReadInfoFromFile(string filename);
    void CreateHashTable();
    void ShowVIP();
    int FindPerson(string id);
    void SortByMilage();
    void SortByCnt();
};
int VIP::FindPerson(string id)
{
    //如果找到了，返回下标，否则返回personcnt
    for(int i=0;i<personcnt;i++){
        if(recordlist[i].id==id){
            return i;
        }
    }
    return personcnt;
}
void VIP::ReadInfoFromFile(string filename)
{
    ifstream fin;
    fin.open(filename, ios::in);
    if (!fin)
    {
        cout << "文件打开失败！" << endl;
        system("pause");
        exit(1);
    }
    istringstream sin; // 将整行字符串line读入到字符串istringstream中
    string word, line; // 临时存储数据
    //存储一行数据的vector
    vector<string> temp;
    double sumcnt = 0;
    double summilage = 0;
    while (getline(fin, line))
    {
        temp.clear();
        sin.clear();   // 每次读取完一行数据后清空sin
        sin.str(line);
        while(getline(sin,word,',')){
            temp.push_back(word);
        }
        PersonInfo p;
        p.id = temp[0];
        p.name = temp[1];
        p.flight_id = temp[2];
        p.flight_date = temp[3];
        p.milage = stod(temp[4]);
        p.cnt = 1;
        sumcnt++;
        summilage+=p.milage;
        int loc=FindPerson(p.id);
        if (loc == personcnt){
            recordlist[personcnt++] = p;
            sorted_by_cnt.push_back(p.id);
            sorted_by_milege.push_back(p.id);
        }
        else{
            recordlist[loc].milage+=p.milage;
            recordlist[loc].cnt++;
        }
    }
    avgcnt = sumcnt / personcnt;
    avgmilage = summilage / personcnt;
    fin.close();
}
void VIP::CreateHashTable()
{
    for (int i = 0; i < personcnt; i++)
    {
        VIPPersonInfo temp;
        temp.id = recordlist[i].id;
        temp.name = recordlist[i].name;
        temp.cnt = recordlist[i].cnt;
        temp.milage = recordlist[i].milage;
        ht1->Insert(temp);
        ht2->Insert(temp);
    }
    cout<<"链地址法的哈希表："<<endl;
    ht1->Print();
    cout<<"开放地址法的哈希表："<<endl;
    ht2->Print();
}
void VIP::ShowVIP()
{
    cout<<"平均乘坐次数:"<<avgcnt<<endl;
    cout<<"平均里程数:"<<avgmilage<<endl;
    cout<<"潜在VIP客户:"<<endl;
    cout.setf(ios::left);
    for(int i=0;i<personcnt;i++){
        if(recordlist[i].cnt>=avgcnt&&recordlist[i].milage>=avgmilage){
            cout << "身份证号:" << setw(20)<<recordlist[i].id;
            cout << "姓名:" << setw(10) << recordlist[i].name;
            cout << "乘坐次数:" << setw(3) << recordlist[i].cnt;
            cout << "里程数:" << setw(8) << recordlist[i].milage;
            cout<< endl;
        }
    }
}
void VIP::SortByMilage()
{
    sort(sorted_by_milege.begin(),sorted_by_milege.end(),[&](string a,string b){
        return ht1->Search(a).milage>ht1->Search(b).milage;
    });
    cout<<"按里程数排序后的信息:"<<endl;
    cout.setf(ios::left);
    for(size_t i=0;i<sorted_by_milege.size();i++){
        VIPPersonInfo temp = ht1->Search(sorted_by_milege[i]);
        cout << "身份证号:" << setw(20)<<temp.id;
        cout << "姓名:" << setw(10) << temp.name;
        cout << "乘坐次数:" << setw(3) << temp.cnt;
        cout << "里程数:" << setw(8) << temp.milage;
        cout<< endl;
    }
}
void VIP::SortByCnt()
{
    sort(sorted_by_cnt.begin(),sorted_by_cnt.end(),[&](string a,string b){
        return ht1->Search(a).cnt>ht1->Search(b).cnt;
    });
    cout<<"按乘坐次数排序后的信息:"<<endl;
    cout.setf(ios::left);
    for(size_t i=0;i<sorted_by_cnt.size();i++){
        VIPPersonInfo temp = ht1->Search(sorted_by_cnt[i]);
        cout << "身份证号:" << setw(20)<<temp.id;
        cout << "姓名:" << setw(10) << temp.name;
        cout << "乘坐次数:" << setw(3) << temp.cnt;
        cout << "里程数:" << setw(8) << temp.milage;
        cout<< endl;
    }
}
int main()
{
    VIP v;
    string filename = "record.txt";
    cout<<"正从文件中"<<filename<<"中读取数据，请稍等..."<<endl;
    v.ReadInfoFromFile(filename);
    cout<<"读取数据完毕！开始建立基于链地址法和线性探测法的哈希表..."<<endl;
    v.CreateHashTable();
    while(1)
    {
        cout<<"--------------------------------Find VIP--------------------------------"<<endl;
        cout<<"规定里程数和乘坐次数均不小于平均值的客户为潜在VIP客户"<<endl;
        cout<<"1.展示潜在VIP客户"<<endl;
        cout<<"2.按里程数展示客户信息"<<endl;
        cout<<"3.按乘坐次数展示客户信息"<<endl;
        cout<<"4.退出"<<endl;
        cout<<"------------------------------------------------------------------------"<<endl;
        cout<<"请输入您的选择:";
        int choice;
        cin>>choice;
        switch(choice)
        {
            case 1:
                v.ShowVIP();
                break;
            case 2:
                v.SortByMilage();
                break;
            case 3:
                v.SortByCnt();
                break;
            case 4:
                cout<<"--------------------------------Good Bye--------------------------------"<<endl;
                system("pause");
                return 0;
            default:
                cout<<"输入错误，请重新输入！"<<endl;
                break;
        }
    }
    system("pause");
    return 0;
}