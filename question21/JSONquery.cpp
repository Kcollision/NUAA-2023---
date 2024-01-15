#include <iostream>
using namespace std;
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include <windows.h>
void SetColor(string str);
class Json
{
private:
    struct Node
    {
        string key;
        string value;
        Node *lchild;
        Node *rbrother;
        Node()
        {
            lchild = nullptr;
            rbrother = nullptr;
        }
        Node(string _key, string _value)
        {
            key = _key;
            value = _value;
            lchild = nullptr;
            rbrother = nullptr;
        }
    };

private:
    Node *root;
    string json_str;
    vector<string> query;

public:
    Json()
    {
        root = new Node();
    }
    ~Json()
    {
        DeleteTree(root);
    }
    void DeleteTree(Node *root);
    void CreateTreeFromFile(string filename);
    void CreateTree(Node *_root, string str);
    void PrintTree();
    void printtree(Node *_root, int depth);
    void ShowQuery();
    void ShowQueryResult();
    string Query(string str);
};
void Json::DeleteTree(Node *root)
{
    if (root == nullptr)
    {
        return;
    }
    DeleteTree(root->lchild);
    DeleteTree(root->rbrother);
    delete root;
}
void Json::CreateTreeFromFile(string filename)
{
    int n, m; // n为json行数，m为查询个数
    ifstream fin;
    fin.open(filename, ios::in);
    fin >> n >> m;
    string str;
    getline(fin, str);
    for (int i = 0; i < n; i++)
    {
        getline(fin, str);
        json_str += str;
    }
    for (int i = 0; i < m; i++)
    {
        string str;
        getline(fin, str);
        query.push_back(str);
    }
    fin.close();
    CreateTree(root, json_str);
}
void Json::CreateTree(Node *_root, string str)
{
    int flag = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '{' && flag == 0)
        {
            flag++;
            continue;
        }
        else if (str[i] == '"')
        {
            string key, value;
            i++;
            while (1)
            {
                if (str[i] == '\\')
                {
                    if (str[i + 1] == '\\')
                    {
                        key += '\\';
                        i += 2;
                    }
                    else if (str[i + 1] == '"')
                    {
                        key += '"';
                        i += 2;
                    }
                    else
                    {
                        key += str[i];
                        i++;
                    }
                }
                else if (str[i] == '"')
                {
                    break;
                }
                else
                {
                    key += str[i];
                    i++;
                }
            }
            while (str[i] != ':')
            {
                i++;
            }
            // 此时str[i]为冒号
            // 建立新节点，至少包含key
            Node *node = new Node(key, "");
            // 找到该插入的节点
            Node *p = _root;
            if (p->lchild == nullptr)
            {
                p->lchild = node;
            }
            else
            {
                p = p->lchild;
                while (p->rbrother != nullptr)
                {
                    p = p->rbrother;
                }
                p->rbrother = node;
            }
            // 判断先遇到{还是"
            i++;
            while (str[i] == ' ')
            {
                i++;
            }
            // 先遇到"，则value为字符串
            if (str[i] == '"')
            {
                i++;
                while (1)
                {
                    if (str[i] == '\\')
                    {
                        if (str[i + 1] == '\\')
                        {
                            value += '\\';
                            i += 2;
                        }
                        else if (str[i + 1] == '"')
                        {
                            value += '"';
                            i += 2;
                        }
                        else
                        {
                            value += str[i];
                            i++;
                        }
                    }
                    else if (str[i] == '"')
                    {
                        break;
                    }
                    else
                    {
                        value += str[i];
                        i++;
                    }
                }
                node->value = value;
            }
            // 先遇到{，则value为对象,递归调用
            else if (str[i] == '{')
            {
                // 提取到相匹配的}为止的字符串
                string temp;
                stack<char> s;
                s.push('{');
                i++;
                while (!s.empty())
                {
                    if (str[i] == '{')
                    {
                        s.push('{');
                    }
                    else if (str[i] == '}')
                    {
                        s.pop();
                    }
                    temp += str[i];
                    i++;
                }
                i--;
                CreateTree(node, temp);
            }
            i++;
            // 如果是逗号，继续循环
            if (str[i] == ',')
            {
                continue;
            }
            // 如果是}，则结束
            else if (str[i] == '}')
            {
                break;
            }
        }
    }
}

void Json::ShowQuery()
{
    cout << "-----------------begin-----------------" << endl;
    SetColor("green");
    for (size_t i = 0; i < query.size(); i++)
    {
        cout << query[i] << endl;
    }
    SetColor("white");
    cout << "------------------end------------------" << endl;
}
void Json::ShowQueryResult()
{
    cout << "-----------------begin-----------------" << endl;
    SetColor("yellow");
    for (size_t i = 0; i < query.size(); i++)
    {
        cout << Query(query[i]) << endl;
    }
    SetColor("white");
    cout << "------------------end------------------" << endl;
}
string Json::Query(string str)
{
    //以.分割字符串
    vector<string> v;
    string temp;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '.')
        {
            v.push_back(temp);
            temp = "";
        }
        else
        {
            temp += str[i];
        }
    }
    v.push_back(temp);
    // 从根节点开始查找
    Node *p = root;
    for (size_t i = 0; i < v.size();i++)
    {
        p=p->lchild;
        bool flag = false;
        while(p!=nullptr){
            if(p->key==v[i]){
                flag = true;
                break;
            }
            p=p->rbrother;
        }
        if(flag==false){
            return "NOTEXIST";
        }
    }
    if(p->value==""){
        return "OBJECT";
    }
    else{
        return "STRING "+p->value;
    }
}

void Json::printtree(Node *_root, int depth)
{
    if (_root == nullptr)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        cout << "|   ";
    }
    cout << "|---";
    if (_root->key == "")
    {
        SetColor("green");
        cout << "null";
        SetColor("white");
        cout << ":";
        SetColor("yellow");
        cout << "null";
        SetColor("white");
        cout<< endl;
    }
    else
    {
        SetColor("green");
        cout << _root->key;
        SetColor("white");
        cout << ":";
        SetColor("yellow");
        cout << _root->value;
        SetColor("white");
        cout<< endl;
    }
    printtree(_root->lchild, depth + 1);
    printtree(_root->rbrother, depth);
}
void Json::PrintTree()
{
    cout << "-----------------begin-----------------" << endl;
    printtree(root, 0);
    cout << "------------------end------------------" << endl;
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
    SetColor("white");
    Json json;
    json.CreateTreeFromFile("input.txt");
    cout << "此json树已构建完成..." << endl;
    json.PrintTree();
    while(1)
    {

        cout << "-----------------menu-----------------" << endl;
        cout << "1.展示文件输入的查询" << endl;
        cout << "2.展示文件输入的查询对应结果" << endl;
        cout << "3.手动查询某个key的value" << endl;
        cout << "4.展示json树" << endl;
        cout << "5.退出" << endl;
        cout << "------------------end-----------------" << endl;
        cout << "your choice:";
        int choice;
        cin >> choice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024,'\n');
            SetColor("red");
            cout << "输入错误，请重新输入" << endl;
            SetColor("white");
            continue;
        }
        switch(choice)
        {
            case 1:{
                json.ShowQuery();
                break;
            }
            case 2:{
                json.ShowQueryResult();
                break;
            }
            case 3:{
                string str;
                cin >> str;
                string result = json.Query(str);
                cout << result << endl;
                break;
            }
            case 4:{
                json.PrintTree();
                break;
            }
            case 5:{
                system("pause");
                return 0;
            }
            default:{
                SetColor("red");
                cout << "输入错误，请重新输入" << endl;
                SetColor("white");
                break;
            }
        }
    }
    system("pause");
    return 0;
}