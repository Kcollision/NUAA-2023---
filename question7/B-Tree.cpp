#include <iostream>
using namespace std;
#include <vector>
#include <cmath>
#include <queue>
#include <windows.h>
#include <stack>
#include <sstream>
template <typename KeyType>
class BTreePrinter;

template <typename KeyType>
class BTNode
{
public:
    vector<KeyType> key;    // 关键字向量
    vector<BTNode *> child; // 孩子向量
    BTNode *parent;         // 父节点
public:
    BTNode()
    {
        parent = nullptr;
        // 初始时有一个空孩子指针
        child.push_back(nullptr);
    }
    BTNode(KeyType k, BTNode *lc = nullptr, BTNode *rc = nullptr)
    {
        key.push_back(k);
        child.push_back(lc);
        if (lc)
        {
            lc->parent = this;
        }
        child.push_back(rc);
        if (rc)
        {
            rc->parent = this;
        }
        parent = nullptr;
    }
};

template <typename KeyType>
class BTree
{
public:
    int _size;                              // 存放的关键字总数
    int _order;                             // B树的阶，至少为3
    BTNode<KeyType> *_root;                 // 根节点
    BTNode<KeyType> *_hot;                  // search()最后访问的非空节点位置
    void solveOverflow(BTNode<KeyType> *);  // 因插入而上溢之后的分裂处理
    void solveUnderflow(BTNode<KeyType> *); // 因删除而下溢之后的合并处理
public:
    BTree(int order)
    {
        _order = order;
        _size = 0;
        _root = new BTNode<KeyType>();
    }
    ~BTree()
    {
        if (_root)
        {
            release(_root);
        }
    }
    // 释放以v为根的子树
    void release(BTNode<KeyType> *v)
    {
        if (!v)
        {
            return;
        }
        for (size_t i = 0; i < v->child.size(); i++)
        {
            release(v->child[i]);
        }
        delete v;
    }
    // 返回阶次
    int const order()
    {
        return _order;
    }
    // 返回规模
    int const size()
    {
        return _size;
    }
    // 返回树根
    BTNode<KeyType> *root()
    {
        return _root;
    }
    // 判断是否为空
    bool empty() const
    {
        return !_root;
    }
    // 查找
    BTNode<KeyType> *search(const KeyType &e);
    // 插入
    bool insert(const KeyType &e);
    // 删除
    bool remove(const KeyType &e);
    // 层次遍历
    void PrintTreeLevelOrder();
    // 用linux "tree" 命令类似的方式打印树
    void PrintTreeLikeLinuxTree(BTNode<KeyType> *p, int level);

    friend class BTreePrinter<KeyType>;
};
// 查找关键字e,返回关键字所在的节点指针
template <typename KeyType>
BTNode<KeyType> *BTree<KeyType>::search(const KeyType &e)
{
    BTNode<KeyType> *v = _root;
    _hot = nullptr;
    while (v)
    {
        int r = 0;
        while (r < int(v->key.size()) && e >= v->key[r])
        {
            if (e == v->key[r])
            {
                return v;
            }
            r++;
        }
        //_hot为关键字所在节点的父节点
        _hot = v;
        v = v->child[r];
    }
    return nullptr;
}
// 插入关键字e
template <typename KeyType>
bool BTree<KeyType>::insert(const KeyType &e)
{
    BTNode<KeyType> *v = search(e);
    if (v)
    {
        return false;
    }
    // 在_hot中寻找合适的插入位置
    int r = 0;
    while (r < int(_hot->key.size()) && e > _hot->key[r])
    {
        // cout<<"第"<<r<<"个关键字:"<<_hot->key[r]<<endl;
        r++;
    }
    // 将关键码插入到合适的位置
    _hot->key.insert(_hot->key.begin() + r, e);
    // 创建一个空子树指针
    _hot->child.insert(_hot->child.begin() + r + 1, nullptr);
    // 更新全树规模
    _size++;
    // 如果有必要，进行分裂
    solveOverflow(_hot);
    return true;
}
// 处理上溢
template <typename KeyType>
void BTree<KeyType>::solveOverflow(BTNode<KeyType> *v)
{
    // 孩子数未超过_order,没有上溢
    if (int(v->child.size()) <= _order)
    {
        return;
    }
    // 中间点
    int s = _order / 2;
    BTNode<KeyType> *u = new BTNode<KeyType>(); // 注意此新节点已经有一个空孩子
    // 将v的右侧的_order-s-1个孩子和关键码分裂入u
    for (int j = 0; j < _order - s - 1; j++)
    {
        u->child.insert(u->child.begin() + j, v->child[s + 1]);
        v->child.erase(v->child.begin() + s + 1);
        u->key.insert(u->key.begin() + j, v->key[s + 1]);
        v->key.erase(v->key.begin() + s + 1);
    }
    // 移动v最靠右的孩子
    u->child[_order - s - 1] = v->child[s + 1]; // 不能用插入，否则会多一个孩子
    v->child.erase(v->child.begin() + s + 1);
    // 如果u的孩子不为空，将他们的父节点更新为u
    if (u->child[0])
    {
        for (int j = 0; j < _order - s; j++)
        {
            u->child[j]->parent = u;
        }
    }
    // v的父节点p
    BTNode<KeyType> *p = v->parent;
    // 如果p为空，创建新的根节点
    if (!p)
    {
        _root = p = new BTNode<KeyType>();
        p->child[0] = v;
        v->parent = p;
    }
    // 在p中找到中点值的合适的插入位置
    int r = 0;
    while (r < int(p->key.size()) && v->key[s] >= p->key[r])
    {
        r++;
    }
    // 将中点值插入到p中
    p->key.insert(p->key.begin() + r, v->key[s]);
    // 删除v中的中点值
    v->key.erase(v->key.begin() + s);
    // 新节点u与父节点p关联
    u->parent = p;
    p->child.insert(p->child.begin() + r + 1, u);
    // 如果有必要，继续分裂
    solveOverflow(p);
}
// 删除关键字e
template <typename KeyType>
bool BTree<KeyType>::remove(const KeyType &e)
{
    // 查找关键字e,确定存在
    BTNode<KeyType> *v = search(e);
    if (!v)
    {
        return false;
    }
    // 确定关键字e在节点v中的秩
    int r = 0;
    while (r < int(v->key.size()) && e != v->key[r])
    {
        r++;
    }
    if (v->child[0])
    {
        // 如果v不是叶子节点，e的后继必然在某个叶子节点中
        // 在v的右子树中一直向左，即可找到e的后继
        BTNode<KeyType> *u = v->child[r + 1];
        while (u->child[0])
        {
            u = u->child[0];
        }
        // 将e的后继与e交换
        v->key[r] = u->key[0];
        v = u;
        r = 0;
    } // 至此，v必然是一个叶子节点,且第r(即0)个关键字就是待删除的关键字e
    v->key.erase(v->key.begin() + r);
    v->child.erase(v->child.begin() + r + 1);
    _size--;
    // 如果有必要，进行下溢处理
    solveUnderflow(v);
    return true;
}
// 处理下溢
template <typename KeyType>
void BTree<KeyType>::solveUnderflow(BTNode<KeyType> *v)
{
    // 当前节点并未下溢
    // 每个节点至少有ceil(_order/2)个孩子，ceil(_order/2)-1个关键字
    // ceil(_order/2)==(_order+1)/2
    if (int((_order + 1) / 2) <= int(v->child.size()))
    {
        return;
    }
    BTNode<KeyType> *p = v->parent;
    // 已到根节点
    if (!p)
    {
        // 但如果树根v没有关键字，且只有一个非空孩子
        if (!v->key.size() && v->child[0])
        {
            // 跳过此节点
            _root = v->child[0];
            _root->parent = nullptr;
            // 且被销毁
            v->child[0] = nullptr;
            release(v);
        } // 树高降低一层
        return;
    }
    // 确定v是p的第r个孩子
    int r = 0;
    while (r < int(p->child.size()) && p->child[r] != v)
    {
        r++;
    }
    // case1:向左兄弟借关键字
    if (0 < r)
    { // 有左兄弟
        BTNode<KeyType> *ls = p->child[r - 1];
        // 如果左兄弟有多余的关键字
        if (int((_order + 1) / 2) < int(ls->child.size()))
        {
            // p中的第r-1个关键字下移至v开头
            v->key.insert(v->key.begin(), p->key[r - 1]);
            // ls的最大关键字上移至p
            p->key[r - 1] = ls->key[ls->key.size() - 1];
            ls->key.erase(ls->key.begin() + ls->key.size() - 1);
            // ls的最右侧孩子过继给v
            v->child.insert(v->child.begin(), ls->child[ls->child.size() - 1]);
            ls->child.erase(ls->child.begin() + ls->child.size() - 1);
            // 作为v的最左侧孩子
            if (v->child[0])
            {
                v->child[0]->parent = v;
            }
            return;
        }
    } // 至此，左兄弟要么为空，要么没多的关键字
    // case2:向右兄弟借关键字
    if (r < int(p->child.size()) - 1)
    {
        BTNode<KeyType> *rs = p->child[r + 1];
        // 如果右兄弟有多余的关键字
        if (int((_order + 1) / 2) < int(rs->child.size()))
        {
            // p借出一个关键字给v;
            v->key.push_back(p->key[r]);
            // rs的最小关键字上移至p
            p->key[r] = rs->key[0];
            rs->key.erase(rs->key.begin());
            // rs的最左侧孩子过继给v
            v->child.push_back(rs->child[0]);
            rs->child.erase(rs->child.begin());
            // 作为v的最右侧孩子
            if (v->child[v->child.size() - 1])
            {
                v->child[v->child.size() - 1]->parent = v;
            }
            return;
        }
    } // 至此，右兄弟要么为空，要么没多的关键字
    // case3:左右兄弟都没有多余的关键字
    // 向左兄弟合并
    if (0 < r)
    {
        BTNode<KeyType> *ls = p->child[r - 1]; // 左兄弟必然存在
        // 将p的第r-1个关键字合并到ls，v不再是p的第r个孩子
        ls->key.push_back(p->key[r - 1]);
        p->key.erase(p->key.begin() + r - 1);
        p->child.erase(p->child.begin() + r);
        // v的最左侧孩子过继给ls做最右侧孩子
        ls->child.push_back(v->child[0]);
        v->child.erase(v->child.begin());
        if (ls->child[ls->child.size() - 1])
        {
            ls->child[ls->child.size() - 1]->parent = ls;
        }
        // v的剩余关键字和孩子都过继给ls
        while (v->key.size())
        {
            ls->key.push_back(v->key[0]);
            v->key.erase(v->key.begin());
            ls->child.push_back(v->child[0]);
            v->child.erase(v->child.begin());
            if (ls->child[ls->child.size() - 1])
            {
                ls->child[ls->child.size() - 1]->parent = ls;
            }
        }
        // v的销毁
        release(v);
    }
    // 与右兄弟合并
    else
    {
        BTNode<KeyType> *rs = p->child[r + 1]; // 右兄弟必然存在
        // 将p的第r个关键字合并到rs，v不再是p的第r+1个孩子
        rs->key.insert(rs->key.begin(), p->key[r]);
        p->key.erase(p->key.begin() + r);
        p->child.erase(p->child.begin() + r);
        // v的最右侧孩子过继给rs做最左侧孩子
        rs->child.insert(rs->child.begin(), v->child[v->child.size() - 1]);
        v->child.erase(v->child.begin() + v->child.size() - 1);
        if (rs->child[0])
        {
            rs->child[0]->parent = rs;
        }
        // v的剩余关键字和孩子都过继给rs
        while (v->key.size())
        {
            rs->key.insert(rs->key.begin(), v->key[v->key.size() - 1]);
            v->key.erase(v->key.begin() + v->key.size() - 1);
            rs->child.insert(rs->child.begin(), v->child[v->child.size() - 1]);
            v->child.erase(v->child.begin() + v->child.size() - 1);
            if (rs->child[0])
            {
                rs->child[0]->parent = rs;
            }
        }
        // v的销毁
        release(v);
    }
    // 如果p下溢，继续处理
    solveUnderflow(p);
    return;
}
// 层次遍历
template <typename KeyType>
void BTree<KeyType>::PrintTreeLevelOrder()
{
    queue<BTNode<KeyType> *> q;
    q.push(_root);
    while (!q.empty())
    {
        int n = q.size();
        for (int i = 0; i < n; i++)
        {
            BTNode<KeyType> *p = q.front();
            q.pop();
            cout << "[";
            for (size_t j = 0; j < p->key.size(); j++)
            {
                cout << p->key[j];
                if (j != p->key.size() - 1)
                    cout << ",";
            }
            cout << "]";
            for (size_t j = 0; j < p->child.size(); j++)
            {
                if (p->child[j])
                {
                    q.push(p->child[j]);
                }
            }
        }
        cout << endl;
    }
}
// 用linux "tree" 命令类似的方式打印树
template <typename KeyType>
void BTree<KeyType>::PrintTreeLikeLinuxTree(BTNode<KeyType> *p, int level)
{
    if (!p)
    {
        return;
    }
    for (int i = 0; i < level; i++)
    {
        cout << "|   ";
    }
    cout << "|---";
    for (size_t i = 0; i < p->key.size(); i++)
    {
        cout << p->key[i];
        if (i != p->key.size() - 1)
            cout << ",";
    }
    cout << endl;
    for (size_t i = 0; i < p->child.size(); i++)
    {
        PrintTreeLikeLinuxTree(p->child[i], level + 1);
    }
}

// 打印b树
template <typename KeyType>
class BTreePrinter
{
    struct NodeInfo
    {
        string text;
        int text_pos, text_end; // half-open range
    };
    typedef vector<NodeInfo> LevelInfo;
    vector<LevelInfo> levels;

    string node_text(vector<KeyType> &key)
    {
        string ans="";
        string sep = "";
        ans += "[";
        for (size_t i = 0; i < key.size(); ++i, sep = " ")
        {
            ans += sep;
            ans += to_string(key[i]);
        }
        ans += "]";
        return ans;
    }

    void before_traversal()
    {
        levels.resize(0);
        levels.reserve(10); 
    }

    void visit(BTNode<KeyType>* node, int level = 0, int child_index = 0)
    {
        if (level >= int(levels.size()))
            levels.resize(level + 1);

        LevelInfo &level_info = levels[level];
        NodeInfo info;

        info.text_pos = 0;
        if (!level_info.empty()) 
            info.text_pos = level_info.back().text_end + (child_index == 0 ? 2 : 1);

        info.text = node_text(node->key);

        // node是叶子节点
        if (node->child[0] == nullptr)
        {
            info.text_end = info.text_pos + int(info.text.length());
        }
        else 
        {
            for (int i = 0, e = int(node->key.size()); i <= e; ++i) 
                visit(node->child[i], level + 1, i);

            info.text_end = levels[level + 1].back().text_end;
        }

        levels[level].push_back(info);
    }
    void print_blanks(int n)
    {
        while (n--)
            cout << ' ';
    }
    void after_traversal()
    {
        for (size_t l = 0, level_count = levels.size();;)
        {
            auto const &level = levels[l];
            int prev_end = 0;

            for (auto const &node : level)
            {
                int total = node.text_end - node.text_pos;
                int slack = total - int(node.text.length());
                int blanks_before = node.text_pos - prev_end;

                print_blanks(blanks_before + slack / 2);
                cout << node.text;

                if (&node == &level.back())
                    break;

                print_blanks(slack - slack / 2);

                prev_end += blanks_before + total;
            }

            if (++l == level_count)
                break;

            cout << "\n\n";
        }

        cout << "\n";
    }

public:
    void print(BTree<KeyType> const &tree)
    {
        before_traversal();
        visit(tree._root);
        after_traversal();
    }
};

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
    int max_m;
    cout << "请输入最大阶数：";
    cin >> max_m;
    BTree<int> bt(max_m);
    while (1)
    {
        cout << "-----------------------B- Tree-----------------------" << endl;
        cout << "1.插入关键字" << endl;
        cout << "2.删除关键字" << endl;
        cout << "3.打印B-树" << endl;
        cout << "4.以tree命令形式打印" << endl;
        cout << "6.退出" << endl;
        cout << "-----------------------------------------------------" << endl;
        int choice;
        cout << "请输入你的选择：";
        cin >> choice;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "输入错误，请重新输入！" << endl;
            continue;
        }
        switch (choice)
        {
            case 1:
            {
                int e;
                cout << "请输入要插入的关键字：";
                cin >> e;
                if (bt.insert(e))
                {
                    SetColor("green");
                    cout << "插入成功！" << endl;
                    SetColor("white");
                }
                else
                {
                    SetColor("red");
                    cout << "插入失败！" << endl;
                    SetColor("white");
                }
                break;
            }
            case 2:
            {
                int e;
                cout << "请输入要删除的关键字：";
                cin >> e;
                if (bt.remove(e))
                {
                    SetColor("green");
                    cout << "删除成功！" << endl;
                    SetColor("white");
                }
                else
                {
                    SetColor("red");
                    cout << "删除失败！" << endl;
                    SetColor("white");
                }
                break;
            }
            case 3:
            {
                SetColor("yellow"); 
                BTreePrinter<int> btp;
                btp.print(bt);
                SetColor("white");
                break;
            }
            case 4:
            {
                SetColor("yellow");
                bt.PrintTreeLikeLinuxTree(bt.root(), 0);
                SetColor("white");
                break;
            }
            case 5:
            {
                system("pause");
                return 0;
            }
            default:
            {
                SetColor("red");
                cout << "输入错误，请重新输入！" << endl;
                SetColor("white");
                break;
            }
        }
    }
    system("pause");
    return 0;
}