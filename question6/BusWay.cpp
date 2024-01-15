/*
公交线路构建无向图
*/
#include <iostream>
#include <cmath>
using namespace std;
#include <fstream>

#include <algorithm>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>

#include <windows.h>
#include <cstring>
#include <string>
#include <streambuf>
#include <sstream>
#define MAX 10000
#define INF 0x3f3f3f3f
using namespace std;
int is_in_same_line[5500][5500]; // 两个站点是否在同一条线路上,如果在，值为1，不在为INF
class Graph_List
{
public:
    struct ArcNode // 边的顶点结构
    {
        int adjvex;       // 该边的终点编号
        int weight;       // 该边的权值
        string roadname;  // 该边所属的公交线路
        ArcNode *nextarc; // 指向下一条边的指针
        ArcNode()
        {
            nextarc = nullptr;
            weight = 0;
            adjvex = 0;
            roadname = "";
        }
        ArcNode(int adjvex, string name, int weight = 1)
        {
            this->adjvex = adjvex;
            this->roadname = name;
            this->weight = weight;
        }
        ArcNode operator=(const ArcNode &a)
        {
            this->adjvex = a.adjvex;
            this->weight = a.weight;
            this->nextarc = a.nextarc;
            this->roadname = a.roadname;
            return *this;
        }
    };
    struct VexNode // 邻接表的顶点结构
    {
        string data;       // 顶点信息
        int code;          // 顶点编号
        ArcNode *firstarc; // 指向第一条依附该顶点的边的指针
        VexNode()
        {
            firstarc = new ArcNode;
            code = 0;
            firstarc->nextarc = nullptr;
        }
        VexNode operator=(const VexNode &v)
        {
            this->data = v.data;
            this->code = v.code;
            this->firstarc = v.firstarc;
            return *this;
        }
    };
public:
    VexNode vexs[MAX];     // 顶点数组
    int vexnum, arcnum;    // 顶点数，边数
    int vex_array_length;  // 顶点数组的长度
    string vexs_info[MAX]; // 存放顶点信息
    int GraphType;         // 图的类型:0无向图，1有向图
    bool is_weighted;      // 是否为带权图
    bool is_deleted[MAX];  // 标记是否被删除

    map<string, int> vexs_map;        // 顶点信息到顶点编号的映射
    vector<vector<string>> roads; // 存放公交线路和站点信息
    map<pair<int, int>, vector<string>> CommonLineOfTwoStation; // 两个站点所在共同的线路CommonLineOfTwoStation
    map<int,string> rownum_to_roadname;       // 线路编号到线路名的映射
    map<string,int> roadname_to_rownum;       // 线路名到线路编号的映射
public:
    Graph_List(int, bool);
    ~Graph_List();
    string UTF8ToGB(const char *str);
    void InitIsInSameLine();//
    int get_graph_type();                                          // 获取图的类型
    bool get_is_weighted();                                        // 获取是否为带权图
    bool isStationExist(string name);                              // 判断站点是否存在,并给出模糊匹配的站点
    int GetWeightOfTrans(int u, int v);                            // 获取边的权值,仅用于两个站点之间的转车次数
    int GetWeight(int u, int v);                                   // 获取边的权值
    void CreateBusGraph(string filename,bool is_utf8);             // 创建公交图
    void InsertArc(int u, int v, string roadname, int weight = 1); // 插入边
    void DeleteGraph();                                            // 删除图
    string GetVex(int v);                                          // v是顶点编号
    void PrintAdjList();                                           // 打印邻接表
    void Clear();                                                  // 清空图
    bool isEmpty() { return vexnum == 0; }                         // 判断图是否为空看到过
    void GetPathOfMinTrans(string name1, string name2);            // 获取转车次数最少的乘车路线
    void GetPathOfMinStation(string name1, string name2);          // 获取经过站点最少的乘车路线
    //两个Dijkstra算法内容几乎一致，区别在于获取两个站点之间的权重不同
    //邻居站台之间的权重为1
    vector<int> DijkstraMinStation(int start,int end);                              // Dijkstra算法 用于求最短经过站点数
    //在同一条线路上的两个站台之间的权重为1，不在同一条线路上的站台之间的权重为INF
    vector<int> DijkstraMinTrans(int start,int end);                                // Dijkstra算法 用于求最短转车次数
    vector<string> GetAllPath(int start, int end);                    // 获取start到end的所有线路名称，其中end一定是start的邻接点
    vector<string> GetAllPathOfTrans(int start, int end);             // 获取start到end的所有线路名称，其中end可以不是start的邻接点
    int GetStationCntOfTwoStation(string start, string end , int roadcode);                // 获取两个站点之间的站点数
};
Graph_List::Graph_List(int GraphType, bool is_weighted)
{
    vexnum = 0;
    arcnum = 0;
    vex_array_length = 0;
    this->GraphType = GraphType;
    this->is_weighted = is_weighted;
    memset(is_deleted, 0, sizeof(is_deleted));
}
Graph_List::~Graph_List()
{
    DeleteGraph();
}
void Graph_List::InitIsInSameLine()
{
    //is_in_same_line.clear();
    // 遍历roads,同一条线路上的站点之间的转车次数为0(权重设置为1)，不在同一条线路上的站点之间的转车次数为INF
    for (size_t i = 0; i < roads.size();i++)
    {
        for (size_t j = 0; j < roads[i].size();j++)
        {
            for (size_t k = j + 1; k < roads[i].size();k++)
            {
                int u=vexs_map[roads[i][j]];
                int v=vexs_map[roads[i][k]];
                //is_in_same_line[{u,v}] = 1;
                is_in_same_line[u][v] = 1;
                //is_in_same_line[{v,u}] = 1;
                is_in_same_line[v][u] = 1;
                CommonLineOfTwoStation[{u,v}].push_back(rownum_to_roadname[i]);
                CommonLineOfTwoStation[{v,u}].push_back(rownum_to_roadname[i]);
            }
        }
    }
}
string Graph_List::UTF8ToGB(const char *str)
{
    string result;
    WCHAR *strSrc;
    LPSTR szRes;

    // 获得临时变量的大小
    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

    // 获得临时变量的大小
    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new CHAR[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[] strSrc;
    delete[] szRes;

    return result;
}
int Graph_List::get_graph_type()
{
    return Graph_List::GraphType;
}
bool Graph_List::get_is_weighted()
{
    return Graph_List::is_weighted;
}
int Graph_List::GetWeightOfTrans(int u, int v)
{
    if(u==v){
        return 0;
    }
    else if(is_in_same_line[u][v] ==1){
        return 1;
    }
    else if(is_in_same_line[u][v] ==0){
        return INF;
    }
    return INF;
}
int Graph_List::GetWeight(int u, int v)
{
    if (u == v)
    {
        return 0;
    }
    ArcNode *p = vexs[u].firstarc->nextarc;
    while (p != nullptr)
    {
        if (p->adjvex == v)
        {
            return p->weight;
        }
        p = p->nextarc;
    }
    return INF;
}
void Graph_List::CreateBusGraph(string filename,bool is_utf8)
{
    // 读取txt文件,获取节点数和边数
    vector<string> name_list;
    map<string, int> name_map;
    int n = 0, m = 0;

    ifstream fin;
    fin.open(filename, ios::in);
    if (!fin)
    {
        cout << "文件打开失败" << endl;
        return;
    }

    string curname;
    istringstream sin;
    //标记是线路名还是站点名
    int check = 0;
    while (fin >> curname)
    {
        // 偶数是站点名，奇数是公交线路名
        check++;
        if (check % 2)
        {
            continue;
        }
        //记录这条公交线路的所有站点
        vector<string> v;
        // 逗号数即为边数
        m += count(curname.begin(), curname.end(), ',');
        string stationname;
        if(!is_utf8){
            stationname = curname;
        }
        else{
            stationname = UTF8ToGB(curname.c_str()).c_str();
        }

        sin.clear();
        sin.str(stationname);

        string temp;
        while (getline(sin, temp, ','))
        {
            string str1 = temp;
            v.push_back(str1);
            if (name_map[str1] > 0)
            {
                continue;
            }
            else
            {
                name_map[str1]++;
                name_list.push_back(str1);
            }
        }
        //将这条线路及其所有站点加入到roads中
        roads.push_back(v);
    }
    fin.close();
    name_map.clear();
    // 将站点按字典序排序
    sort(name_list.begin(), name_list.end());
    n = name_list.size();
    vexnum = n;
    arcnum = m;
    cout << "顶点数:" << n <<" "<< " 边数:" << m << endl;
    vex_array_length = n;
    // 将set中的节点加入顶点数组
    int cnt = 1;
    for (auto it : name_list)
    {
        vexs[cnt].data = it;
        vexs[cnt].code = cnt;
        vexs_info[cnt] = it;
        vexs_map[it] = cnt;
        cnt++;
    }
    // 读取txt文件，将边加入邻接表
    ifstream fin2;
    fin2.open(filename, ios::in);
    if (!fin2)
    {
        cout << "文件打开失败" << endl;
        return;
    }
    check = 0;
    string last_roadname;
    while (fin2 >> curname)
    {
        // 偶数是站点名，奇数是公交线路名
        check++;
        if (check % 2)
        {
            if(!is_utf8)
                last_roadname = curname;
            else
                last_roadname = UTF8ToGB(curname.c_str()).c_str();
            rownum_to_roadname[(check-1) / 2] = last_roadname;
            roadname_to_rownum[last_roadname] = (check-1) / 2;
            continue;
        }
        string stationname;
        if(!is_utf8){
            stationname = curname;
        }
        else{
            stationname = UTF8ToGB(curname.c_str()).c_str();
        }
        sin.clear();
        sin.str(stationname);
        string temp;
        vector<string> v;
        while (getline(sin, temp, ','))
        {
            string str1 = temp;
            v.push_back(str1);
        }
        for (size_t i = 0; i < v.size() - 1; i++)
        {
            int a = vexs_map[v[i]];
            int b = vexs_map[v[i + 1]];
            InsertArc(a, b, last_roadname, 1);
        }
    }
    fin2.close();
    memset(is_deleted, 0, sizeof(is_deleted));
    //PrintAdjList();
}
bool Graph_List::isStationExist(string name)
{
    string stationname = name;
    // 二分查找
    int left = 1, right = vex_array_length;
    while (left < right)
    {
        int mid = (left + right) / 2;
        if(vexs[mid].data<stationname){
            left = mid + 1;
        }
        else{
            right = mid;
        }
    }
    if (vexs[left].data == stationname)
    {
        return true;
    }
    else{
        //根据二分结果模糊匹配
        cout << "Error: 未查找到此站点" << endl;
        cout << "您可能想要输入的站点有:";
        if(left-2>=1)
            cout << vexs_info[left-2] <<" ";
        if(left-1>=1)
            cout << vexs_info[left-1] <<" ";
        cout << vexs_info[right] << " ";
        if(right+1<=vex_array_length)
            cout <<vexs_info[right+1] <<" ";
        cout << endl;
    }
    return false;
}
void Graph_List::InsertArc(int u, int v, string roadname, int weight)
{
    Graph_List::ArcNode *p;
    if (Graph_List::is_weighted)
        p = new Graph_List::ArcNode(v, roadname, weight);
    else
        p = new Graph_List::ArcNode(v, roadname);
    p->nextarc = Graph_List::vexs[u].firstarc->nextarc;
    vexs[u].firstarc->nextarc = p;
    if (Graph_List::GraphType == 0)
    {
        if (Graph_List::is_weighted)
            p = new Graph_List::ArcNode(u, roadname, weight);
        else
            p = new Graph_List::ArcNode(u, roadname);
        p->nextarc = vexs[v].firstarc->nextarc;
        vexs[v].firstarc->nextarc = p;
    }
}
void Graph_List::DeleteGraph()
{
    Clear();
}
void Graph_List::PrintAdjList()
{
    cout << "*************打印邻接表************" << endl;
    for (int i = 1; i <= vex_array_length; i++)
    {
        if (!is_deleted[i])
        {
            cout << vexs_info[i] << ": ";
            ArcNode *p = vexs[i].firstarc->nextarc;
            while (p != nullptr)
            {
                cout << p->roadname << ":" << vexs_info[p->adjvex] << "->";
                p = p->nextarc;
            }
            cout << endl;
        }
    }
    cout << "*************打印完成************" << endl;
}
void Graph_List::Clear()
{
    for (int i = 1; i <= vex_array_length; i++)
    {
        if (!is_deleted[i])
        {
            ArcNode *p = vexs[i].firstarc->nextarc;
            while (p != nullptr)
            {
                ArcNode *q = p;
                p = p->nextarc;
                delete q;
            }
        }
    }
    vexnum = 0;
    arcnum = 0;
    vex_array_length = 0;
    memset(is_deleted, 0, sizeof(is_deleted));
}
void Graph_List::GetPathOfMinTrans(string name1, string name2)
{
    int start = vexs_map[name1];
    int end = vexs_map[name2];
    vector<int> ans = DijkstraMinTrans(start, end);
    cout << "转车次数最少的乘车路线为:" << endl;
    cout<<"起点站:"<<vexs_info[start]<<endl;
    for (size_t i = 1; i < ans.size()-1;i++)
    {

        vector<string> roadnames = CommonLineOfTwoStation[{ans[i - 1], ans[i]}]; // GetAllPathOfTrans(ans[i-1], ans[i]);
        cout<<"|->可乘坐：";
        for(auto x:roadnames){
            cout<<x;
            cout << "(经过"<<GetStationCntOfTwoStation(vexs_info[ans[i-1]],vexs_info[ans[i]],roadname_to_rownum[x])<< "站) ";
        }
        cout << endl;
        cout << "到第" << i << "换乘站：" << vexs_info[ans[i]];
        cout << endl;
    }
    vector<string> roadnames = CommonLineOfTwoStation[{ans[ans.size() - 2], ans[ans.size() - 1]}]; // GetAllPathOfTrans(ans[ans.size()-2], ans[ans.size()-1]);
    cout<<"|->可乘坐：";
    for(auto x:roadnames){
        cout<<x;
        cout << "(经过" << GetStationCntOfTwoStation(vexs_info[ans[ans.size() - 2]], vexs_info[ans[ans.size() - 1]], roadname_to_rownum[x]) << "站) ";
    }
    cout << endl;
    cout<<"到终点站:"<<vexs_info[end]<<endl;
    cout<<"总共乘坐"<<ans.size()-1<<"条线路，"<<"总共转车次数"<<ans.size()-2<<"次"<<endl;
}
void Graph_List::GetPathOfMinStation(string name1, string name2)
{
    int start = vexs_map[name1];
    int end = vexs_map[name2];
    vector<int> ans = DijkstraMinStation(start, end);
    cout << "经过站点最少的乘车路线为:" << endl;
    cout<<"起点站:"<<vexs_info[start]<<endl;
    for (size_t i = 1; i < ans.size()-1;i++)
    {
        vector<string> roadnames = GetAllPath(ans[i-1], ans[i]);
        cout<<"|->可乘坐：";
        for(auto x:roadnames){
            cout<<x<<" ";
        }
        cout << endl;
        cout << "到第" << i << "站：" << vexs_info[ans[i]];
        cout << endl;
    }
    vector<string> roadnames = GetAllPath(ans[ans.size()-2], ans[ans.size()-1]);
    cout<<"|->可乘坐：";
    for(auto x:roadnames){
        cout<<x<<" ";
    }
    cout << endl;
    cout<<"到终点站:"<<vexs_info[end]<<endl;
    cout<<"除去起点站和终点站,中间总共经过"<<ans.size()-2<<"站"<<endl;
}
vector<string> Graph_List::GetAllPath(int start,int end)
{
    vector<string> ans;
    ArcNode *p = vexs[start].firstarc->nextarc;
    while(p!=nullptr){
        if(p->adjvex==end){
            ans.push_back(p->roadname);
        }
        p = p->nextarc;
    }
    return ans;
}
vector<int> Graph_List::DijkstraMinTrans(int start,int end)
{
    int dist[MAX], path[MAX], flag[MAX];
    for (int i = 1; i <= vexnum;i++)
    {
        flag[i] = 0;
        dist[i] = GetWeightOfTrans(start, i);
        if(dist[i]<INF){
            path[i] = start;
        }
        else{
            path[i] = -1;
        }
    }
    flag[start] = 1;

    for (int i = 1; i < vexnum;i++)
    {
        int mindist = INF;
        int u = start;
        for (int j = 1; j <= vexnum;j++)
        {
            if(!flag[j]&&dist[j]<mindist){
                u = j;
                mindist = dist[j];
            }
        }
        flag[u] = 1;
        mindist=dist[u];
        if(mindist<INF){
            for (int j = 1; j <= vexnum;j++)
            {
                if(!flag[j]&&dist[u]+GetWeightOfTrans(u,j)<dist[j]){
                    dist[j] = dist[u] + GetWeightOfTrans(u, j);
                    path[j] = u;
                }
            }                                        
        }    
    }

    vector<int> ans;
    ans.push_back(end);
    while(path[end]!=start){
        ans.push_back(path[end]);
        end = path[end];
    }
    ans.push_back(start);
    reverse(ans.begin(), ans.end());
    return ans;
}
vector<int> Graph_List::DijkstraMinStation(int start,int end)
{
    int dist[MAX], path[MAX], flag[MAX];
    for (int i = 1; i <= vexnum;i++)
    {
        flag[i] = 0;
        dist[i] = GetWeight(start, i);
        if(dist[i]<INF){
            path[i] = start;
        }
        else{
            path[i] = -1;
        }
    }
    flag[start] = 1;
    for (int i = 1; i < vexnum;i++)
    {
        int mindist = INF;
        int u = start;
        for (int j = 1; j <= vexnum;j++)
        {
            if(!flag[j]&&dist[j]<mindist){
                u = j;
                mindist = dist[j];
            }
        }
        flag[u] = 1;
        mindist=dist[u];
        if(mindist<INF){
            for (int j = 1; j <= vexnum;j++)
            {
                if(!flag[j]&&dist[u]+GetWeight(u,j)<dist[j]){
                    dist[j] = dist[u] + GetWeight(u, j);
                    path[j] = u;
                }
            }                                        
        }
    } 
    vector<int> ans;
    ans.push_back(end);
    while(path[end]!=start){
        ans.push_back(path[end]);
        end = path[end];
    }
    ans.push_back(start);
    reverse(ans.begin(), ans.end());
    return ans;
}
int Graph_List::GetStationCntOfTwoStation(string start, string end , int roadcode)
{
    vector<string> res= roads[roadcode];
    int loc1, loc2;
    for (size_t i = 0; i < res.size();i++)
    {
        if(res[i]==start){
            loc1 = i;
        }
        if(res[i]==end){
            loc2 = i;
        }
    }
    return abs(loc1 - loc2);
}
//设置控制台颜色
void SetColor(string str){
    if(str=="red"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
    }
    else if(str=="green"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }
    else if(str=="blue"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
    }
    else if(str=="yellow"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    }
    else if(str=="purple"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
    }
    else if(str=="cyan"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    else if(str=="white"){
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |FOREGROUND_GREEN | FOREGROUND_BLUE);//设置三色相加:白色
    }
}
int main()
{
    SetColor("white");
    Graph_List g(0, 1); // 无向带权
    string filename="南京公交线路.txt";
    //此处文件不是utf-8编码
    g.CreateBusGraph(filename,0);
    cout<<"创建图成功,正在进入菜单..."<<endl;
    g.InitIsInSameLine();
    while (1)
    {
        cout << "---------------------------------------" << endl;
        cout << "-----------------begin-----------------" << endl;
        cout << "1.输入任意两站点，给出转车次数最少的乘车路线" << endl;
        cout << "2.输入任意两站点，给出经过站点最少的乘车路线" << endl;
        cout << "3.判断站点是否存在，并给出相似站点" << endl;
        cout << "4.退出" << endl;
        cout << "------------------end------------------" << endl;
        cout << "---------------------------------------" << endl;
        cout << "请输入选项:";
        int choice;
        cin >> choice;
        if(cin.fail()){
            cin.clear();//
            //清除缓冲区的内容
            cin.ignore(1024,'\n');
            cout << "输入错误，请重新输入" << endl;
            continue;
        }
        string filename, name1, name2;
        bool flag;
        switch (choice)
        {
        case 1:
            cout << "请输入起点站:";
            cin >> name1;
            flag=g.isStationExist(name1);
            if(!flag){
                break;
            }
            cout << "请输入终点站:";
            cin >> name2;
            flag=g.isStationExist(name2);
            if(!flag){
                break;
            }
            SetColor("yellow");
            g.GetPathOfMinTrans(name1, name2);
            SetColor("white");
            break;
        case 2:
            cout << "请输入起点站:";
            cin >> name1;
            flag=g.isStationExist(name1);
            if(!flag){
                break;
            }
            cout << "请输入终点站:";
            cin >> name2;
            flag=g.isStationExist(name2);
            if(!flag){
                break;
            }
            SetColor("yellow");
            g.GetPathOfMinStation(name1, name2);
            SetColor("white");
            break;
        case 3:
            cout << "请输入站点名:";
            cin >> name1;
            flag=g.isStationExist(name1);
            if(!flag){
                break;
            }
            SetColor("yellow");
            cout << "站点存在" << endl;
            SetColor("white");
            break;
        case 4:
            system("pause");
            return 0;
        default:
            break;
        }
    }
    system("pause");
    return 0;
}
/*
1
bus.txt
2
南堡公园站
随家仓站
新街口东站
翠屏山站
中山路珠江路北站
乐山路金沙江西街站
*/
