#include <iostream>
#include <fstream>
#include <stack>
#include <cstring>
#include <vector>
#include <windows.h>
using namespace std;
// 设置控制台颜色
void SetColor(string str);
const int MAXROW=100, MAXCOL=100;
typedef pair<int, int> PII;
int dx[4]={-1,0,1,0}, dy[4]={0,1,0,-1};//方向向量

class MAZE
{
private:
    int row, col;
    char maze[MAXROW][MAXCOL];
    bool visited[MAXROW][MAXCOL];
    PII start, end;
    vector<PII> start_end;
public:
    MAZE();
    void read(string filename);//读取迷宫
    void print();//打印迷宫
    void solve(PII start,PII end);//求解迷宫
    void test();//测试
    void find_start_end();//找到可选择的起点和终点
};
MAZE::MAZE()
{
    memset(maze,0,sizeof(maze));
    memset(visited,0,sizeof(visited));
}
void MAZE::print()
{
    SetColor("green");
    cout<<"note:"<<"■表示墙壁,□表示通路,S表示起点,E表示终点"<<endl;
    SetColor("white");
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            if(maze[i][j]-'0'==1)
                cout<<"■"<<' ';
            else if(maze[i][j]-'0'==0)
                cout<<"□"<<' ';
            else{
                SetColor("green");
                if(maze[i][j]=='S')
                    cout<<"S"<<' ';
                else if(maze[i][j]=='E')
                    cout<<"E"<<' ';
                else
                    cout<<"□"<<' ';
                SetColor("white");
            }
        }
        cout<<endl;
    }
}
void MAZE::read(string filename)
{
    ifstream fin(filename,ios::in);
    if(!fin)
    {
        SetColor("red");
        cout<<"文件打开失败！"<<endl;
        SetColor("white");
        system("pause");
        exit(0);
    }
    else{
        SetColor("green");
        cout<<"文件打开成功！"<<endl;
        SetColor("white");
    }
    fin>>row>>col;
    if(row>MAXROW||col>MAXCOL)
    {
        SetColor("red");
        cout<<"迷宫尺寸超过最大限制！"<<endl;
        SetColor("white");
        system("pause");
        exit(0);
    }
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
            fin>>maze[i][j];
    fin.close();
    SetColor("green");
    cout<<"迷宫读取成功！"<<endl;
    SetColor("white");
    find_start_end();
}
void MAZE::solve(PII start,PII end)
{
    memset(visited,0,sizeof(visited));
    stack<PII> s;
    s.push(start);
    visited[start.first][start.second]=true;
    PII cur=start;
    while(s.size() || cur!=end)
    {
        if(cur==end){
            break;
        }
        int x = cur.first, y = cur.second;
        bool flag = false;
        for(int i=0; i<4; i++)
        {
            int nx = x+dx[i], ny = y+dy[i];
            if(nx<0||nx>=row||ny<0||ny>=col) continue;
            if(maze[nx][ny]-'0'==0&&!visited[nx][ny])
            {
                s.push({nx,ny});
                cur={nx,ny};
                visited[nx][ny]=true;
                flag=true;
                break;
            }
        }
        if(!flag)
        {
            s.pop();
            if(s.size())
                cur=s.top();
        }
    }
    if(cur!=end){
        SetColor("red");
        cout<<"没有找到路径！"<<endl;
        SetColor("white");
    }
    else
    {
        SetColor("green");
        cout<<"找到了一条路径！"<<endl;
        SetColor("white");
        if(maze[start.first][start.second]-'0'==0)
            maze[start.first][start.second]='S';
        if(maze[end.first][end.second]-'0'==0)
            maze[end.first][end.second]='E';
        while(s.size())
        {
            PII p=s.top();
            s.pop();
            if(p==start||p==end) continue;
            PII pre=s.top();
            if(p.first==pre.first){
                maze[p.first][p.second]=' ';
            }    
            else{
                maze[p.first][p.second]=' ';
            }
        }   
    }
}
void MAZE::test()
{
    string filename;
    cout<<"请输入迷宫文件名:";
    cin>>filename;
    read(filename);
    while(1)
    {
        cout<<"可供选择的起点和终点有："<<endl;
        for(int i=0; i<(int)start_end.size(); i++)
            cout<<"("<<start_end[i].first<<","<<start_end[i].second<<") ";
        cout<<endl;
        cout<<"请输入起点坐标：";
        cin>>start.first>>start.second;
        if(start.first==-1&&start.second==-1)
            break;
        cout<<"请输入终点坐标：";
        cin>>end.first>>end.second;
        if(start.first<0||start.first>=row||start.second<0||start.second>=col||
           end.first<0||end.first>=row||end.second<0||end.second>=col)
        {
            SetColor("red");
            cout<<"坐标超出范围！"<<endl;
            SetColor("white");
            cout << "是否选择继续?(y/n):";
            string c;
            cin >> c;
            if(c=="n")
                break;
        }
        if(maze[start.first][start.second]-'0'==1||maze[end.first][end.second]-'0'==1)
        {
            SetColor("red");
            cout<<"起点或终点为墙壁！"<<endl;
            SetColor("white");
            cout << "是否选择继续?(y/n):";
            string c;
            cin >> c;
            if(c=="n")
                break;
        }
        solve(start,end);
        print();
        cout << "是否选择继续?(y/n):";
        string c;
        cin >> c;
        if(c=="n")
            break;
    }
}
void MAZE::find_start_end()
{
    start_end.clear();
    int cnt=0;
    for(int i=0; i<row; i++)
    {
        for(int j=0; j<col; j++)
        {
            if(i==0||i==row-1||j==0||j==col-1)
            {
                if(maze[i][j]-'0'==0)
                {
                    cnt++;
                    start_end.push_back({i,j});
                }
            }
        }
    }
    if(cnt<2)
    {
        cout<<"迷宫中没有可选择的起点和终点！"<<endl;
        exit(0);
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
int main()
{
    MAZE maze;
    maze.test();
    system("pause");
    return 0;
}