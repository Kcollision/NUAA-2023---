#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <cstring>
#include <map>
#include <string>
#include <ctime>
#include <windows.h>
#define MAX 100
using namespace std;
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
// 维护顶点编号的并查集
class UnionFindSet
{
private:
    int *p;
    int cnt; // 连通分量个数

public:
    int find(int x)
    {
        if (p[x] != x)
            p[x] = find(p[x]);
        return p[x];
    }
    UnionFindSet(int n)
    {
        p = new int[n + 1];
        for (int i = 1; i <= n; i++)
        {
            p[i] = i;
        }
        cnt = n;
    }
    void Union(int x, int y)
    {
        int fx = find(x);
        int fy = find(y);
        if (fx != fy)
            p[fx] = fy;
        cnt--;
    }
    int get_cnt()
    {
        return cnt;
    }
    bool is_connected(int x, int y)
    {
        return find(x) == find(y);
    }
};

template <typename VertexType>
class Gragh_List
{
public:
    struct ArcNode // 边的顶点结构
    {
        int adjvex;       // 该边的终点编号
        float weight;     // 该边的权值
        ArcNode *nextarc; // 指向下一条边的指针
        ArcNode()
        {
            nextarc = nullptr;
            weight = 0;
            adjvex = 0;
        }
        ArcNode(int adjvex, float weight = 0)
        {
            this->adjvex = adjvex;
            this->weight = weight;
        }
        ArcNode operator=(const ArcNode &a)
        {
            this->adjvex = a.adjvex;
            this->weight = a.weight;
            this->nextarc = a.nextarc;
            return *this;
        }
    };
    struct VexNode // 邻接表的顶点结构
    {
        VertexType data;   // 顶点信息
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
    VexNode vexs[MAX];         // 顶点数组
    int vexnum, arcnum;        // 顶点数，边数
    int vex_array_length;      // 顶点数组的长度
    VertexType vexs_info[MAX]; // 存放顶点信息
    int GraphType;             // 图的类型:0无向图，1有向图
    bool is_visited[MAX];      // 标记是否被访问
    bool is_weighted;          // 是否为带权图
    bool is_deleted[MAX];      // 标记是否被删除

    float dist[MAX];               // 存放最短路径长度
    map<VertexType, int> vexs_map; // 顶点信息到顶点编号的映射
public:
    Gragh_List(int GraphType = 0, bool is_weighted = 0)
    {
        vexnum = 0;
        arcnum = 0;
        vex_array_length = 0;
        this->GraphType = GraphType;
        this->is_weighted = is_weighted;
        memset(is_deleted, 0, sizeof(is_deleted));
    }
    ~Gragh_List()
    {
        DeleteGraph();
    }
    void CreateGraphFromFile(string filename, bool is_input_vertexcode)
    {
        ifstream fin;
        fin.open(filename, ios::in);
        if (!fin.is_open())
        {
            SetColor("red");
            cout << "文件打开失败" << endl;
            SetColor("white");
            return;
        }
        fin >> vexnum >> arcnum;
        vex_array_length = vexnum;
        for (int i = 1; i <= vexnum; i++)
        {
            fin >> vexs[i].data;
            vexs[i].code = i;
            vexs_info[i] = vexs[i].data;
            vexs_map[vexs[i].data] = i;
        }
        for (int i = 1; i <= arcnum; i++)
        {
            int u, v;
            float w = 1;
            if (!is_input_vertexcode)
            {
                VertexType u1, v1;
                if (is_weighted)
                {
                    fin >> u1 >> v1;
                    fin >> w;
                    u = vexs_map[u1];
                    v = vexs_map[v1];
                }
                else
                {
                    fin >> u1 >> v1;
                    u = vexs_map[u1];
                    v = vexs_map[v1];
                }
            }
            else
            {
                if (is_weighted == 1)
                    fin >> u >> v >> w;
                else
                    fin >> u >> v;
            }
            InsertArc(u, v, 0, w);
        }
        fin.close();
        memset(is_deleted, 0, sizeof(is_deleted));
    }
    void CreateGraph(bool is_input_vertexcode)
    {
        cout << "请输入顶点数和边数:";
        cin >> vexnum >> arcnum;
        vex_array_length = vexnum;
        cout << "请输入顶点信息(依次输入,空格隔开):" << endl;
        for (int i = 1; i <= vexnum; i++)
        {
            cin >> vexs[i].data;
            vexs[i].code = i;
            vexs_info[i] = vexs[i].data;
            vexs_map[vexs[i].data] = i;
        }
        cout << "请输入边的信息(依次输入:起点 终点 权值(没有则不用输入)):" << endl;
        for (int i = 1; i <= arcnum; i++)
        {
            int u, v, w = 1;
            if (!is_input_vertexcode)
            {
                VertexType u1, v1;
                if (is_weighted)
                {
                    cin >> u1 >> v1 >> w;
                    u = vexs_map[u1];
                    v = vexs_map[v1];
                }
                else
                {
                    cin >> u1 >> v1;
                    u = vexs_map[u1];
                    v = vexs_map[v1];
                }
            }
            else
            {
                if (is_weighted == 1)
                    cin >> u >> v >> w;
                else
                    cin >> u >> v;
            }
            ArcNode *p;
            if (!is_weighted)
                p = new ArcNode(v);
            else
                p = new ArcNode(v, w);
            p->nextarc = vexs[u].firstarc->nextarc;
            vexs[u].firstarc->nextarc = p;
            if (GraphType == 0)
            {
                if (!is_weighted)
                    p = new ArcNode(u);
                else
                    p = new ArcNode(u, w);
                p->nextarc = vexs[v].firstarc->nextarc;
                vexs[v].firstarc->nextarc = p;
            }
        }
        memset(is_deleted, 0, sizeof(is_deleted));
    }
    void DeleteGraph()
    {
        clear();
    }
    void InsertVex(VertexType v)
    {
        for (int i = 1; i <= vex_array_length; i++)
        {
            if (is_deleted[i])
            {
                vexs[i].data = v;
                vexnum++;
                is_deleted[i] = false;
                vexs[i].code = i;
                vexs[i].firstarc = new ArcNode;
                vexs_info[i] = v;
                vexs_map[v] = i;
                return;
            }
        }
        vexnum++;
        vex_array_length++;
        vexs[vexnum].data = v;
        vexs[vexnum].code = vexnum;
        vexs[vexnum].firstarc = new ArcNode;
        vexs_info[vexnum] = v;
        vexs_map[v] = vexnum;
    }
    void InsertArc(int v, int w, int flag, float weight = 0)
    {
        // v-w,w(v,w)=weight
        // flag==1表示是最小生成树加的边，即对于无向图只加一条边
        ArcNode *p;
        if (is_weighted)
            p = new ArcNode(w, weight);
        else
            p = new ArcNode(w);
        p->nextarc = vexs[v].firstarc->nextarc;
        vexs[v].firstarc->nextarc = p;
        if (GraphType == 0 && !flag)
        {
            if (is_weighted)
                p = new ArcNode(v, weight);
            else
                p = new ArcNode(v);
            p->nextarc = vexs[w].firstarc->nextarc;
            vexs[w].firstarc->nextarc = p;
        }
    }
    void DeleteVex(int v)
    {
        for (int i = 1; i <= vex_array_length; i++)
        {
            if (!is_deleted[i])
            {
                if (i == v)
                {
                    ArcNode *p = vexs[i].firstarc->nextarc;
                    while (p != nullptr)
                    {
                        ArcNode *q = p;
                        p = p->nextarc;
                        delete q;
                    }
                }
                else
                {
                    DeleteArcNode_in_list(i, v);
                }
            }
        }
        is_deleted[v] = true;
    }
    void DeleteArc(int v, int w)
    {
        if (is_deleted[v] || is_deleted[w])
        {
            cout << "顶点不存在" << endl;
            return;
        }
        DeleteArcNode_in_list(v, w);
        if (GraphType == 0)
            DeleteArcNode_in_list(w, v);
    }
    // 在vexs[v]的邻接表中删除w
    void DeleteArcNode_in_list(int v, int w)
    {
        if (is_deleted[v] || is_deleted[w])
        {
            cout << "顶点不存在" << endl;
            return;
        }
        ArcNode *p = vexs[v].firstarc->nextarc;
        ArcNode *prev = vexs[v].firstarc;
        while (p)
        {
            if (p->adjvex == w)
            {
                prev->nextarc = p->nextarc;
                delete p;
                // 加上break，防止删除多条有重复顶点的边
                break;
                p = prev->nextarc;
            }
            else
            {
                prev = p;
                p = p->nextarc;
            }
        }
    }
    void print_adj_list()
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
                    // cout << p->adjvex << endl;
                    cout << vexs_info[p->adjvex] << " ";
                    if (is_weighted)
                        cout << "权值" << p->weight << "   -> ";
                    p = p->nextarc;
                }
                cout << endl;
            }
        }
        cout << "*************打印完成************" << endl;
    }
    float get_weight(int u, int v)
    {
        if (is_weighted == 0)
        {
            cout << "该图为无权图，不存在权值" << endl;
            return -1;
        }
        ArcNode *p = vexs[u].firstarc->nextarc;
        while (p)
        {
            if (p->adjvex == v)
                return p->weight;
            p = p->nextarc;
        }
        cout << "该边不存在" << endl;
        return -1;
    }
    float GetSumWeight()
    {
        float sum = 0;
        for (int i = 1; i <= vex_array_length; i++)
        {
            if (!is_deleted[i])
            {
                ArcNode *p = vexs[i].firstarc->nextarc;
                while (p)
                {
                    sum += p->weight;
                    p = p->nextarc;
                }
            }
        }
        if (GraphType == 0)
            sum /= 2;
        return sum;
    }
    bool MST_Kruskal(Gragh_List<VertexType> &mst)
    {
        if (GraphType == 1)
        {
            cout << "该图为有向图，无法生成最小生成树" << endl;
            return false;
        }
        UnionFindSet ufs(vex_array_length);
        // 权重、起点、终点
        vector<pair<float, pair<int, int>>> edges;
        for (int i = 1; i <= vex_array_length; i++)
        {
            if (!is_deleted[i])
            {
                mst.InsertVex(vexs_info[i]);
                ArcNode *p = vexs[i].firstarc->nextarc;
                while (p)
                {
                    edges.push_back(make_pair(p->weight, make_pair(i, p->adjvex)));
                    p = p->nextarc;
                }
            }
        }
        sort(edges.begin(), edges.end());
        for (auto x : edges)
        {
            float wt = x.first;
            int u = x.second.first;
            int v = x.second.second;
            if (!ufs.is_connected(u, v))
            {
                ufs.Union(u, v);
                mst.InsertArc(u, v, 0, wt); // 0表示双向边
            }
        }
        // 判断是否成功生成最小生成树
        return ufs.get_cnt() == 1;
    }
    bool MST_Prim(Gragh_List<VertexType> &mst)
    {
        //先给mst插入所有顶点
        for (int i = 1; i <= vex_array_length; i++)
        {
            if (!is_deleted[i])
            {
                mst.InsertVex(vexs_info[i]);
            }
        }
        // 初始化访问数组，距离数组
        memset(is_visited, 0, sizeof(is_visited));
        memset(dist, 0, sizeof(dist));
        if (GraphType == 1)
        {
            cout << "该图为有向图，无法生成最小生成树" << endl;
            return false;
        }
        int now = 1;
        for (int i = 2; i <= vexnum; i++)
        {
            dist[i] = float(0x3f3f3f3f);
        }
        for (ArcNode *i = vexs[now].firstarc->nextarc; i != nullptr; i = i->nextarc)
        {
            dist[i->adjvex] = min(dist[i->adjvex], i->weight);
        }
        // 最小生成树的边数为vexnum-1
        int totnum = 0;
        while (++totnum < vexnum)
        {
            float minn = float(0x3f3f3f3f);
            is_visited[now] = 1;
            int temp = now;
            for (int i = 1; i <= vexnum; i++)
            {
                if (!is_visited[i] && minn > dist[i])
                {
                    minn = dist[i];
                    now = i;
                }
            }
            mst.InsertArc(temp, now, 0, minn);
            for (ArcNode *i = vexs[now].firstarc->nextarc; i != nullptr; i = i->nextarc)
            {
                int v = i->adjvex;
                if (!is_visited[v] && dist[v] > i->weight)
                {
                    dist[v] = i->weight;
                }
            }
        }
        // 判断是否成功生成最小生成树
        return true;
    }
    void clear()
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
};
int main()
{
    SetColor("white");
    cout << "-----------------MST test begin!-----------------" << endl;
    while (1)
    {
        cout << "-----------------------MENU----------------------" << endl;
        cout << "1.手动输入测试数据" << endl;
        cout << "2.从文件读取测试数据" << endl;
        cout << "3.退出" << endl;
        cout << "------------------------END----------------------" << endl;
        cout << "请输入你的选择:";
        int choice;
        cin >> choice;
        Gragh_List<string> g(0, 1);
        if (choice == 1)
        {
            cout << "提示: 默认输入无向带权图" << endl;
            SetColor("yellow");
            cout << "输入格式：" << endl;
            cout << "顶点数 边数" << endl;
            cout << "顶点信息(空格隔开)" << endl;
            cout << "边的信息(起点 终点 权值(没有则不用输入))" << endl;
            cout << "y:顶点编号输入边  n:顶点名称输入边;";
            SetColor("white");
            cout << "请输入(y/n):";
            char is_input_vertexcode;
            cin >> is_input_vertexcode;
            if (is_input_vertexcode == 'y')
                g.CreateGraph(1);
            else
                g.CreateGraph(0);
        }
        else if (choice == 2)
        {
            char is_random_complete_graph;
            cout << "是否需要随机生成完全图测试文件？(y/n):";
            cin >> is_random_complete_graph;
            if (is_random_complete_graph == 'y')
            {
                string filename;
                cout << "请输入保存的文件名:";
                cin >> filename;
                srand(time(NULL));
                ofstream out;
                out.open(filename, ios::out);
                // 顶点数设置为5~8
                int n;
                cout << "请输入顶点数:";
                cin >> n;
                int lowernum, uppernum;
                cout << "请输入随机权值的范围(整数):";
                cin >> lowernum >> uppernum;
                out << n << " " << n * (n - 1) / 2 << endl;
                for (int i = 0; i < n; i++)
                {
                    // 输出字符
                    out << static_cast<char>('a' + i) << ' ';
                }
                out << endl;
                for (int i = 1; i <= n; i++)
                {
                    for (int j = i + 1; j <= n; j++)
                    {
                        //随机生成10到30的浮点数
                        int x=lowernum+rand()%(uppernum-lowernum+1);
                        double randx=x+rand()%10/10.0;
                        //设置小数位数
                        out << static_cast<char>('a' + i - 1) << ' ' << static_cast<char>('a' + j - 1) << ' ' <<randx << endl;
                    }
                }
                out.close();
                cout << "完全图创建完成...文件已保存..." << endl;
                g.CreateGraphFromFile(filename, 0);
            }
            else
            {
                cout << "请输入文件名:";
                string filename;
                cin >> filename;
                cout << "文件中边的信息是否是顶点编号?(y/n)(默认不是 eg: v1 v2 12):";
                char is_input_vertexcode;
                cin >> is_input_vertexcode;
                if (is_input_vertexcode == 'y')
                    g.CreateGraphFromFile(filename, 1);
                else
                    g.CreateGraphFromFile(filename, 0);
            }
        }
        else{
            break;
        }
        cout << "无向图构建完成，其邻接表如下:" << endl;
        g.print_adj_list();
        cout<<"开始计算最小生成树:" << endl;
        cout << "-------------------------------------------------" << endl;
        cout << "------------------MST of kruskal-----------------" << endl;
        Gragh_List<string> mst(0, 1);
        if (g.MST_Kruskal(mst))
        {
            cout << ">>>最小生成树为:" << endl;
            SetColor("green");
            mst.print_adj_list();
            SetColor("white");
            cout << ">>>最小生成树的权值和为:" << mst.GetSumWeight() << endl;
        }
        else
        {
            cout << "该图无法生成最小生成树" << endl;
        }
        
        cout << "-------------------------------------------------" << endl;
        cout << "--------------------MST of prim------------------" << endl;
        //mst.clear();
        Gragh_List<string> mst1(0, 1);
        if (g.MST_Prim(mst1))
        {
            cout << ">>>最小生成树为:" << endl;
            SetColor("blue");
            mst1.print_adj_list();
            SetColor("white");
            cout << ">>>最小生成树的权值和为:" << mst1.GetSumWeight() << endl;
        }
        else
        {
            cout << "该图无法生成最小生成树" << endl;
        }
    }  
    system("pause");
    return 0;
}