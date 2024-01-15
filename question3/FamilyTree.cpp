#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <streambuf>
#include <sstream>
#include <vector>
#include <stack>
#include <locale>
#include <queue>
using namespace std;
void SetColor(string str);//设置控制台颜色
class FamilyTree
{
private:
    struct Person{
        string name;//姓名
        string birthday;//出生日期
        bool isMarried;//是否结婚
        string spouse;//(若结婚)配偶姓名
        string address;//地址
        bool isAlive;//是否健在
        string deathday;//(若死亡)死亡日期
        Person()
        {
            name = "";
            birthday = "";
            isMarried = false;
            spouse = "";
            address = "";
            isAlive = true;
            deathday = "";
        }
        Person(string name,string birthday,bool isMarried,string spouse,string address,bool isAlive,string deathday)
        {
            this->name = name;
            this->birthday = birthday;
            this->isMarried = isMarried;
            this->spouse = spouse;
            this->address = address;
            this->isAlive = isAlive;
            this->deathday = deathday;
        }
    };
    struct Node{
        Person person;
        Node *lchild;
        Node *rbrother;
        Node()
        {
            lchild = nullptr;
            rbrother = nullptr;
        }
        Node(Person person,Node *lchild,Node *rbrother)
        {
            this->person = person;
            this->lchild = lchild;
            this->rbrother = rbrother;
        }
    };
private:
    Node *root;//根节点
public:
    FamilyTree()
    {
        root = nullptr;
    }
    ~FamilyTree()
    {
        DeleteTree(root);
        delete root;
    }
    int CompareDate(string date1, string date2);//比较两个日期的大小，date1>date2返回1，date1<date2返回-1，date1=date2返回0
    int GetDateYearDifference(string date1, string date2);//计算两个日期之间的年数差
    bool isEmptyTree()
    {return root==nullptr;}

    pair<Node*,Node*> SearchByName(string searchname);//按姓名查询成员父亲、本人、孩子的信息,返回<父亲，本人>
    void SearchByBirthdate(string, string);//按出生日期范围查询人员名单
    vector<Node *> SearchPathToRoot(string);//查询某人到根节点的路径
    bool FindPath(Node *p,string name, vector<Node *> &path);//查询某人到节点p的路径

    void GetChildInfo(Node *);// 获取打印孩子信息
    void GetPersonRelation(string name1, string name2);//获取两个人的关系
    void SaveTreeToFile(string filename);//保存家谱树到文件
    void createTreeFromFile(string filename);//从文件中读取数据创建家谱树
    void InsertChild(string namefa, Person p, pair<Node *, Node *> fap);
    void ModifyPersonInfo(string chname);
    Person CreatePersonByUser();//手动创建成员
    void DeleteTree(Node* &p);//删除家谱树
    void DeletePersonAndChild(string name);//删除此成员和他的后代

    void PrintPersonInfo(Node *);// 打印成员信息
    void PrintTree();//打印家谱树
    void PrintTreeOfPerson(Node *);//打印以p为祖宗的家谱,不包括p的兄弟
    void printTreeLevelOrder();//层次遍历打印家谱树
    void printTreeLevelOrder(int level);//层次遍历打印家谱树
};
int FamilyTree::CompareDate(string date1,string date2)
{
    int year1,year2,month1,month2,day1,day2;
    sscanf(date1.c_str(),"%d.%d.%d",&year1,&month1,&day1);
    sscanf(date2.c_str(),"%d.%d.%d",&year2,&month2,&day2);
    if(year1>year2)
        return 1;
    else if(year1<year2)
        return -1;
    else{
        if(month1>month2)
            return 1;
        else if(month1<month2)
            return -1;
        else{
            if(day1>day2)
                return 1;
            else if(day1<day2)
                return -1;
            else
                return 0;
        }
    }
}
int FamilyTree::GetDateYearDifference(string date1,string date2)
{
    int year1,year2,month1,month2,day1,day2;
    sscanf(date1.c_str(),"%d.%d.%d",&year1,&month1,&day1);
    sscanf(date2.c_str(),"%d.%d.%d",&year2,&month2,&day2);
    int yearDifference = year1-year2;
    if(month1<month2)
        yearDifference--;
    else if(month1==month2){
        if(day1<day2)
            yearDifference--;
    }
    return yearDifference;
}
void FamilyTree::PrintPersonInfo(Node *p)
{
    SetColor("yellow");
    cout.setf(ios::left);
    cout << "姓名：" << setw(7) << p->person.name;
    cout << "出生日期：" << setw(11) << p->person.birthday;
    cout << "是否结婚：" << setw(4) << ((p->person.isMarried) == true?"是":"否");
    cout << "伴侣名称：" << setw(7) <<((p->person.spouse == "") ? "无" : p->person.spouse);
    cout << "家庭住址：" << setw(20) << p->person.address;
    cout << "是否健在：" << setw(4) << ((p->person.isAlive) == true?"是":"否");
    cout << "死亡日期：" << setw(10) << ((p->person.deathday == "") ? "无" :p->person.deathday);
    cout << endl;
    SetColor("white");
}
void FamilyTree::createTreeFromFile(string filename)
{
    DeleteTree(root);
    ifstream fin(filename,ios::in);
    if(!fin)
    {
        SetColor("red");
        cout << "Error: 文件打开失败！" << endl;
        SetColor("white");
        return;
    }

    istringstream sin;//将整行字符串line读入到字符串istringstream中
    vector<string> words; //存储每行的数据
    string word,line;//临时存储数据
    // 读取标题行
    getline(fin, line);
    string str = line;
    cout << "-------------------------begin--------------------------" << endl;
    cout << "此文件包含信息格式如下：" << endl;
    cout<<str<<endl;
    // 模拟递归，存储:<辈分,节点指针>
    stack<pair<int,Node *>> stk;
    while (getline(fin, line))
    {
        if(line==""){
            cout<<"空行"<<endl;
            continue;
        }
        sin.clear();
        sin.str(line);
        words.clear();

        int index;//辈分
        string name;//姓名
        string birthday;//出生日期
        bool isMarried;//是否结婚
        string spouse;//(若结婚)配偶姓名
        string address;//地址
        bool isAlive;//是否健在
        string deathday;//(若死亡)死亡日期
        while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
        {
            words.push_back(word);
        }
        //cout << endl;
        //辈分字符串转int
        istringstream ss(words[0]);
        ss >> index;
        name = words[1];
        birthday = words[2];
        isMarried = (words[3]=="1")?true:false;
        spouse = (words[4]=="NULL")?"":words[4];
        address = words[5];
        isAlive = (words[6]=="1")?true:false;
        deathday = (words[7]=="NULL")?"":words[7];
        if(isAlive==1 && deathday!="")
        {
            SetColor("red");
            cout<<"Error: "<<name<<" 健在但存在死亡日期"<<endl;
            SetColor("white");
            return;
        }
        if(isAlive==0 && deathday=="")
        {
            SetColor("red");
            cout<<"Error: "<<name<<" 死亡但没有死亡日期"<<endl;
            SetColor("white");
            return;
        }
        if(isMarried==1 && spouse=="")
        {
            SetColor("red");
            cout<<"Error: "<<name<<" 结婚但没有配偶姓名"<<endl;
            SetColor("white");
            return;
        }
        if(isMarried==0 && spouse!="")
        {
            SetColor("red");
            cout<<"Error: "<<name<<" 未结婚但存在配偶姓名"<<endl;
            SetColor("white");
            return;
        }
        if(deathday!="" && CompareDate(birthday,deathday)>0)
        {
            SetColor("red");
            cout<<"Error: "<<name<<" 出生日期不应晚于死亡日期"<<endl;
            SetColor("white");
            return;
        }
        //创建节点
        Node *p = new Node(Person(name,birthday,isMarried,spouse,address,isAlive,deathday),nullptr,nullptr);
        if(root==nullptr){
            root = p;
            stk.push({index, p});
        }
        else{
            pair<int,Node *> top = stk.top();
            if(index==top.first){
                top.second->rbrother = p;
                stk.pop();
                stk.push({index, p});
            }
            else if(index==top.first+1)
            {
                //检验出生日期是否满足条件
                if(CompareDate(birthday,top.second->person.birthday)<=0)
                {
                    SetColor("red");
                    cout << "Error: " << name << " 孩子的出生日期需要晚于父亲的出生日期" << endl;
                    SetColor("white");
                    cout << "ErrorMessage:" << endl;
                    cout << "父亲：" << top.second->person.name << " 出生日期：" << top.second->person.birthday << endl;
                    cout << "孩子：" << name << " 出生日期：" << birthday << endl;
                    return;
                }
                if(GetDateYearDifference(birthday,top.second->person.birthday)>=80|| GetDateYearDifference(birthday,top.second->person.birthday)<=15)
                {
                    SetColor("red");
                    cout << "Warning: " << name << " 孩子一般在父亲15岁之后且80岁之前出生" << endl;
                    SetColor("white");
                    cout << "WarningMessage:" << endl;
                    cout << "父亲：" << top.second->person.name << " 出生日期：" << top.second->person.birthday << endl;
                    cout << "孩子：" << name << " 出生日期：" << birthday << endl;
                    cout <<"是否继续？(y/n):";
                    string s;
                    cin>>s;
                    if(s=="n")
                    {
                        return;
                    }
                }
                //检验是否未结婚却有孩子
                if(top.second->person.isMarried==false){
                    SetColor("red");
                    cout << "Warning: " << name << " 未结婚却有孩子" << endl;
                    SetColor("white");
                    cout << "WarningMessage:" << endl;
                    cout << "父亲：" << top.second->person.name << endl;
                    cout << "孩子：" << name << endl;
                    cout <<"是否继续？(y/n):";
                    string s;
                    cin>>s;
                    if(s=="n")
                    {
                        return;
                    }
                }
                top.second->lchild = p;
                stk.push({index, p});
            }
            else if(index<top.first)
            {
                while(index<top.first)
                {
                    stk.pop();
                    top = stk.top();
                }
                if(top.first==index){
                    top.second->rbrother = p;
                    stk.pop();
                    stk.push({index, p});
                }
            }
            else{
                cout<<"Error: "<<name<<" index is wrong"<<endl;
                return ;
            }
        }
    }
    fin.close();
    cout << "--------------------------end---------------------------" << endl;
}
void FamilyTree::SaveTreeToFile(string filename)
{
    ofstream fout(filename,ios::out);
    if(!fout)
    {
        cout << "错误，文件打开失败！" << endl;
        return;
    }
    fout << "辈分,姓名,出生日期,是否结婚,配偶姓名,住址,是否健在,死亡日期" << endl;
    //打印以p为祖宗的家谱,不包括p的兄弟
    Node *p = root;
    stack<pair<int, Node *>> stk;
    stk.push({1, p});
    string str=to_string(1)+","+p->person.name+","+p->person.birthday+","
    +to_string(p->person.isMarried)+","+(p->person.isMarried==0?"NULL":p->person.spouse)+","+p->person.address+","+
    to_string(p->person.isAlive)+","+(p->person.isAlive==1?"NULL":p->person.deathday);
    fout<<str<<endl;
    pair<int, Node *> cur=stk.top();
    while(stk.size() && cur.second!=nullptr)
    {
        if(cur.second->lchild){
            stk.push({cur.first+1, cur.second->lchild});
            cur = stk.top();
            Node *temp = cur.second;
            string str=to_string(cur.first)+","+temp->person.name+","+temp->person.birthday+","
            +to_string(temp->person.isMarried)+","+(temp->person.isMarried==0?"NULL":temp->person.spouse)+","+temp->person.address+","+
            to_string(temp->person.isAlive)+","+(temp->person.isAlive==1?"NULL":temp->person.deathday);
            fout<<str<<endl;
            continue;
        }
        if(cur.second->rbrother){
            //先删除栈中的上一个兄弟
            stk.pop();
            if(cur.second==p){
                return;
            }
            //将下一个兄弟压栈
            stk.push({cur.first, cur.second->rbrother});
            cur = stk.top();
            Node *temp = cur.second;
            string str=to_string(cur.first)+","+temp->person.name+","+temp->person.birthday+","
            +to_string(temp->person.isMarried)+","+(temp->person.isMarried==0?"NULL":temp->person.spouse)+","+temp->person.address+","+
            to_string(temp->person.isAlive)+","+(temp->person.isAlive==1?"NULL":temp->person.deathday);
            fout<<str<<endl;
            continue;
        }
        if(!cur.second->lchild && !cur.second->rbrother){
            stk.pop();//删除最后一代
            //从上一代的兄弟开始
            if(stk.size()){
                if(stk.top().second==p){
                    return;
                }
                cur = {stk.top().first,stk.top().second->rbrother};
            }
            else
                cur = {0, nullptr};
            while(cur.second==nullptr && stk.size()){
                //删除上一代
                stk.pop();
                if(stk.size()){
                //将上一代兄弟压进栈
                    if(stk.top().second==p){
                        return;
                    }
                    cur = {stk.top().first,stk.top().second->rbrother};
                }
                else{
                    cur = {0, nullptr};
                }
            }
            //删除上一代
            if(stk.size())
                stk.pop();
            //将上一代兄弟压进栈
            if(cur.second!=nullptr){
            Node *temp = cur.second;
            string str=to_string(cur.first)+","+temp->person.name+","+temp->person.birthday+","
            +to_string(temp->person.isMarried)+","+(temp->person.isMarried==0?"NULL":temp->person.spouse)+","+temp->person.address+","+
            to_string(temp->person.isAlive)+","+(temp->person.isAlive==1?"NULL":temp->person.deathday);
            fout<<str<<endl;
            }
            stk.push(cur);
        }
    }
    fout.close();
    SetColor("yellow");
    cout << "保存成功(@_@)" << endl;
    SetColor("white");
}
FamilyTree::Person FamilyTree::CreatePersonByUser()
{
    string name;//姓名
    string birthday;//出生日期
    bool isMarried;//是否结婚
    string spouse;//(若结婚)配偶姓名
    string address;//地址
    bool isAlive;//是否健在
    string deathday;//(若死亡)死亡日期
    cout << "开始创建新成员......"<<endl;
    cout << "-------------------------begin--------------------------" << endl;
    cout << "请输入姓名:";
    cin >> name;
    cout << "请输入出生日期:";
    cin >> birthday;
    cout << "是否结婚(1/0):";
    cin >> isMarried;
    if(isMarried){
        cout << "请输入配偶姓名:";
        cin >> spouse;
    }
    else
        spouse = "";
    cout << "请输入地址:";
    cin >> address;
    cout << "是否健在(1/0):";
    cin >> isAlive;
    if(!isAlive){
        cout << "请输入死亡日期:";
        cin >> deathday;
    }
    else
        deathday = "";
    cout << "--------------------------end---------------------------" << endl;
    Person p(name, birthday, isMarried, spouse, address, isAlive, deathday);
    return p;
}
void FamilyTree::InsertChild(string namefa,FamilyTree::Person p,pair<Node *,Node *> fap)
{
    Node *fa = fap.second;
    if(fa==nullptr)
    {
        SetColor("red");
        cout << "Error: 未找到需要添加孩子的人";
        SetColor("white");
        return;
    }
    else{
        if(CompareDate(fa->person.birthday,p.birthday)>=0){
            SetColor("red");
            cout << "Error: " << " 孩子的出生日期需要晚于父亲的出生日期" << endl;
            SetColor("white");
            cout << "ErrorMessage:" << endl;
            cout << "父亲：" << fa->person.name<< " 出生日期：" << fa->person.birthday << endl;
            cout << "孩子：" << p.name << " 出生日期：" << p.birthday << endl;
            return;
        }
        if (GetDateYearDifference(p.birthday, fa->person.birthday) >= 80 || GetDateYearDifference(p.birthday, fa->person.birthday) <= 15)
        {
            SetColor("red");
            cout << "Warning: "
                 << " 孩子一般在父亲15岁之后且80岁之前出生" << endl;
            SetColor("white");
            cout << "WarningMessage:" << endl;
            cout << "父亲：" << fa->person.name << " 出生日期：" << fa->person.birthday << endl;
            cout << "孩子：" << p.name << " 出生日期：" << p.birthday << endl;
            cout << "是否继续？(y/n):";
            string s;
            cin >> s;
            if (s == "n")
            {
                return;
            }
        }
        // 检验是否未结婚却有孩子
        if (fa->person.isMarried == false)
        {
            SetColor("red");
            cout << "Warning: "<< " 未结婚却有孩子" << endl;
            SetColor("white");
            cout << "WarningMessage:" << endl;
            cout << "父亲：" << fa->person.name << endl;
            cout << "孩子：" << p.name << endl;
            cout << "是否需要将父亲改为已结婚？(y/n):";
            string s;
            cin >> s;
            if (s == "y")
            {
                fa->person.isMarried = true;
                cout << "已经改为已结婚.请输入母亲姓名:";
                cin >> fa->person.spouse;
                cout << "修改成功(@_@)" << endl;
            }
        }
        Node *son = new Node(p, nullptr, nullptr);
        Node *bro;
        if(fa->lchild){
            bro = fa->lchild;
            while(bro->rbrother){
                bro = bro->rbrother;
            }
            bro->rbrother = son;
        }
        else{
            fa->lchild = son;
        }
        cout << "添加孩子成功(@_@)"<<endl;
    }
    return;
}
void FamilyTree::DeleteTree(FamilyTree::Node* &p)
{
    if(p==nullptr){
        return;
    }
    DeleteTree(p->rbrother);
    DeleteTree(p->lchild);
    delete p;
    p=nullptr;    
    return;
}
void FamilyTree::DeletePersonAndChild(string delname)
{
    pair<Node *,Node *> delp = SearchByName(delname);
    Node *del = delp.second;
    if(del==nullptr){
        SetColor("red");
        cout << "Error: 未找到需要删除的人";
        SetColor("white");
        return;
    }
    else{
        Node *father = delp.first;
        //为根节点
        if(father==nullptr){
            DeleteTree(del->lchild);
            delete del;
        }
        else{
            //删除儿子
            DeleteTree(del->lchild);
            // 通过父亲找到前一个兄弟;
            Node *pre = nullptr,*p=father->lchild;
            while(p!=del){
                pre = p;
                p = p->rbrother;
            }
            if(pre==nullptr){
                //说明第一个儿子就是要删除的
                father->lchild = del->rbrother;
                delete del;
            }
            else{
                //pre是前一个兄弟
                pre->rbrother = del->rbrother;
                delete del;
            }
        }
        SetColor("yellow");
        cout << "删除完成(@_@)"<<endl;
        SetColor("white");
    }
    return;
}
void FamilyTree::ModifyPersonInfo(string chname)
{
    pair<Node *, Node *> modpersons = SearchByName(chname);
    Node *mod = modpersons.second,*moddad=modpersons.first;
    if(mod==nullptr){
        SetColor("red");
        cout << "错误：未找到待修改人" << endl;
        SetColor("white");
        return;
    }
    while(1){
        cout << "--------------------------------------------------------" << endl;
        cout << "--------------------modifiable menu---------------------" << endl;
        cout << "1.修改名字"<<endl;
        cout << "2.修改出生日期"<<endl;
        cout << "3.修改是否已婚"<<endl;
        cout << "4.修改住址"<<endl;
        cout << "5.修改是否健在"<<endl;
        cout << "6.显示最新信息"<<endl;
        cout << "7.退出修改"<<endl;
        cout << "---------------------------end--------------------------" << endl;
        cout << "--------------------------------------------------------" << endl;
        int choice;
        cout<<"请输入选项：";
        cin >> choice;
        string modname,birday;
        Node *elderson=nullptr;
        Node *oldestson = nullptr;
        switch (choice)
        {
            case 1:
                cout << "输入新的名字：";
                cin >> modname;
                mod->person.name = modname;
                break;
            case 2:
                cout << "输入新的出生日期：";
                cin >> birday;
                //检验父亲和待修改人的出生日期是否满足条件
                if(CompareDate(birday,moddad->person.birthday)<=0)
                {
                    SetColor("red");
                    cout << "Error: " <<" 孩子的出生日期需要晚于父亲的出生日期" << endl;
                    SetColor("white");
                    cout << "ErrorMessage:" << endl;
                    cout << "父亲：" << moddad->person.name << " 出生日期：" << moddad->person.birthday << endl;
                    cout << "孩子：" << mod->person.name<< " 出生日期：" << birday << endl;
                    break;
                }
                if(GetDateYearDifference(birday,moddad->person.birthday)>=80|| GetDateYearDifference(birday,moddad->person.birthday)<=15)
                {
                    SetColor("red");
                    cout << "Warning: " << " 孩子一般在父亲15岁之后且80岁之前出生" << endl;
                    SetColor("white");
                    cout << "WarningMessage:" << endl;
                    cout << "父亲：" << moddad->person.name << " 出生日期：" << moddad->person.birthday << endl;
                    cout << "孩子：" << mod->person.name << " 新的出生日期：" << birday << endl;
                    cout <<"是否继续？(y/n)"<<endl;
                    string s;
                    cin>>s;
                    if(s=="n")
                    {
                        break;
                    }
                }
                elderson = nullptr;
                if(mod->lchild){
                    elderson = mod->lchild;
                }
                oldestson = elderson;
                while(elderson){
                    if(CompareDate(oldestson->person.birthday,elderson->person.birthday)>0){
                        oldestson = elderson;
                    }
                    elderson = elderson->rbrother;
                }
                if(oldestson){
                    //检验待修改人和最大孩子的出生日期是否满足条件
                    if(CompareDate(oldestson->person.birthday,birday)<=0)
                    {
                        SetColor("red");
                        cout << "Error: " <<" 孩子的出生日期需要晚于父亲的出生日期" << endl;
                        SetColor("white");
                        cout << "ErrorMessage:" << endl;
                        cout << "父亲：" <<  mod->person.name<< " 出生日期：" <<birday<< endl;
                        cout << "孩子：" << oldestson->person.name<< " 出生日期：" << oldestson->person.birthday << endl;
                        break;
                    }
                    if(GetDateYearDifference(oldestson->person.birthday,birday)>=80|| GetDateYearDifference(oldestson->person.birthday,birday)<=15)
                    {
                        SetColor("red");
                        cout << "Warning: " << " 孩子一般在父亲15岁之后且80岁之前出生" << endl;
                        SetColor("white");
                        cout << "WarningMessage:" << endl;
                        cout << "父亲：" << mod->person.name << " 出生日期：" << birday << endl;
                        cout << "孩子：" <<oldestson->person.name<< " 出生日期：" << oldestson->person.birthday << endl;
                        cout <<"是否继续？(y/n)"<<endl;
                        string s;
                        cin>>s;
                        if(s=="n")
                        {
                            break;
                        }
                    }
                }
                mod->person.birthday = birday;
                break;
            case 3:
                cout << "输入新的是否已婚(1/0):";
                cin >> mod->person.isMarried;
                if(mod->person.isMarried){
                    cout << "请输入配偶姓名:";
                    cin >> mod->person.spouse;
                }
                else
                    mod->person.spouse = "";
                break;
            case 4:
                cout << "输入新的住址:";
                cin >> mod->person.address;
                break;
            case 5:
                cout << "输入新的是否健在(1/0):";
                cin >> mod->person.isAlive;
                if(!mod->person.isAlive){
                    cout << "请输入死亡日期:";
                    cin >> mod->person.deathday;
                }
                else
                    mod->person.deathday = "";
                break;
            case 6:
                PrintPersonInfo(mod);
            case 7:
                return;
            default:
                break;
        }
    }
}
void FamilyTree::printTreeLevelOrder()
{
    queue<Node *> q;
    q.push(root);
    int floor = 1;//层数
    cout << "--------------------------begin-------------------------" << endl;
    while(q.size())
    {
        int size = q.size();
        cout<<"第"<<floor++<<"代的家族成员名称为：";
        while(size--){
            Node * tmp = q.front();
            cout << tmp->person.name << " ";
            q.pop();
            if(tmp->lchild)
                q.push(tmp->lchild);
            Node * p = tmp->lchild;
            while(p&&p->rbrother){
                q.push(p->rbrother);
                p = p->rbrother;
            }
        }
        cout << endl;
    }
    cout << "---------------------------end--------------------------" << endl;
    return;
} 
void FamilyTree::printTreeLevelOrder(int level)
{
    if(level==0){
        printTreeLevelOrder();
        return;
    }
    queue<Node *> q;
    q.push(root);
    int floor = 1;//层数
    bool flag = false;
    while(q.size())
    {
        int size = q.size();
        if(floor==level){
            flag = true;
            cout << "第" << floor << "代的家族成员信息为：" << endl;
            cout << "--------------------------begin-------------------------" << endl;
            while(size--){
                Node *temp = q.front();
                q.pop();
                PrintPersonInfo(temp);
            }
            cout << "---------------------------end--------------------------" << endl;
            break;
        }
        while(size--){
            Node * tmp = q.front();
            q.pop();
            if(tmp->lchild)
                q.push(tmp->lchild);
            Node * p = tmp->lchild;
            while(p&&p->rbrother){
                q.push(p->rbrother);
                p = p->rbrother;
            }
        }
        floor++;
    }
    if(!flag){
        SetColor("red");
        cout << "Error: 未找到第" << level << "代的家族成员" << endl;
        SetColor("white");
    }
    return;
} 
void FamilyTree::PrintTree()
{
    PrintTreeOfPerson(root);
}
void FamilyTree::PrintTreeOfPerson(Node *p)
{
    if(p==nullptr){
        SetColor("red");
        cout << "ERROR :未找到此人" << endl;
        SetColor("white");
        return;
    }
    //打印以p为祖宗的家谱,不包括p的兄弟
    stack<pair<int, Node *>> stk;
    stk.push({1, p});
    cout << "- " << p->person.name<<endl;
    pair<int, Node *> cur=stk.top();
    while(stk.size() && cur.second!=nullptr)
    {
        if(cur.second->lchild){
            stk.push({cur.first+1, cur.second->lchild});
            cur = stk.top();
            bool flag=true;//打印两个空格暂停一次
            for (int i = 1; i < cur.first;i++)
            {
                if(flag){
                    cout << "  ";
                    flag = false;
                }
                else{
                    cout << "|";
                    flag = true;
                    i--;
                }
            }
            cout << "|- " << cur.second->person.name << endl;
            continue;
        }
        if(cur.second->rbrother){
            //先删除栈中的上一个兄弟
            stk.pop();
            if(cur.second==p){
                return;
            }
            //将下一个兄弟压栈
            stk.push({cur.first, cur.second->rbrother});
            cur = stk.top();
            bool flag=true;//打印两个空格暂停一次
            for (int i = 1; i < cur.first;i++)
            {
                if(flag){
                    cout << "  ";
                    flag = false;
                }
                else{
                    cout << "|";
                    flag = true;
                    i--;
                }
            }
            cout << "|- " << cur.second->person.name << endl;
            continue;
        }
        if(!cur.second->lchild && !cur.second->rbrother){
            stk.pop();//删除最后一代
            //从上一代的兄弟开始
            if(stk.size()){
                if(stk.top().second==p){
                    return;
                }
                cur = {stk.top().first,stk.top().second->rbrother};
            }
            else
                cur = {0, nullptr};
            while(cur.second==nullptr && stk.size()){
                //删除上一代
                stk.pop();
                if(stk.size()){
                //将上一代兄弟压进栈
                    if(stk.top().second==p){
                        return;
                    }
                    cur = {stk.top().first,stk.top().second->rbrother};
                }
                else{
                    cur = {0, nullptr};
                }
            }
            //删除上一代
            if(stk.size())
                stk.pop();
            //将上一代兄弟压进栈
            bool flag=true;//打印两个空格暂停一次
            for (int i = 1; i < cur.first;i++)
            {
                if(flag){
                    cout << "  ";
                    flag = false;
                }
                else{
                    cout << "|";
                    flag = true;
                    i--;
                }
            }
            if(cur.second!=nullptr)
                cout << "|- " << cur.second->person.name << endl;
            stk.push(cur);
        }
    }
}
void FamilyTree::GetChildInfo(Node *p)
{
    Node *child;
    if(p==nullptr){
        cout << "错误:该指针为空,没有孩子" << endl;
        return;
    }
    if(p->lchild){
        child = p->lchild;
        while(child){
            PrintPersonInfo(child);
            child = child->rbrother;
        }
    }
    else{
        cout << "此人无孩子" << endl;
    }
}
void FamilyTree::GetPersonRelation(string name1,string name2)
{
    vector<Node *> path1 = SearchPathToRoot(name1);
    vector<Node *> path2 = SearchPathToRoot(name2);
    if(!path1.size()){
        SetColor("red");
        cout << "错误：未找到 " << name1 << endl;
        SetColor("white");
        return;
    }
    if(!path2.size()){
        SetColor("red");
        cout << "错误：未找到 " << name2 << endl;
        SetColor("white");
        return;
    }
    int flag = 0;
    while(flag<(int)path1.size()&&flag<(int)path2.size()&&path1[flag]==path2[flag])
        flag++;
    flag--;
    if(flag==-1){
        SetColor("red");
        cout << "错误：未找到 " << name1 << " 和 " << name2 << " 的最近关系人" << endl;
        SetColor("white");
        return;
    }
    cout << name1 << " 和 " << name2 << " 的最近关系人为：" << path1[flag]->person.name << endl;
    vector<pair<string,Node *>> relation1to2;//name1的xxxxx是name2
    relation1to2.push_back({"本人",path1.back()});
    for (int i = path1.size() - 1; i>flag;i--)
    {
        if(path1[i-1]->rbrother==path1[i]){
            //i-1出生日期小于i,即i-1是i的哥哥
            if(CompareDate(path1[i]->person.birthday,path1[i-1]->person.birthday)>0){
                if(relation1to2.back().first=="的哥哥"){
                    //前一个是的哥哥，删除前一个
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的哥哥",path1[i-1]});
                }
                else if(relation1to2.back().first=="的弟弟"){
                    //a的弟弟的哥哥和a作比较
                    //a的出生日期大于a的弟弟的哥哥的出生日期，即a的弟弟的哥哥是a的哥哥
                    if(CompareDate(relation1to2[relation1to2.size()-2].second->person.birthday,path1[i-1]->person.birthday)>0){
                        relation1to2.pop_back();
                        relation1to2.push_back({"的哥哥",path1[i-1]});
                    }
                    else{
                        //a的弟弟的哥哥是a的弟弟
                        relation1to2.pop_back();
                        relation1to2.push_back({"的弟弟",path1[i-1]});
                    }
                }
                else if(relation1to2.back().first=="的儿子"){
                    //依然是儿子
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的儿子",path1[i-1]});
                }
                else{
                    relation1to2.push_back({"的哥哥",path1[i-1]});
                }
            }
            else{
                if(relation1to2.back().first=="的哥哥"){
                    //a的哥哥的弟弟和a作比较
                    //a的出生日期大于a的哥哥的弟弟的出生日期，即a的哥哥的弟弟是a的哥哥
                    if(CompareDate(relation1to2[relation1to2.size()-2].second->person.birthday,path1[i-1]->person.birthday)>0){
                        relation1to2.pop_back();
                        relation1to2.push_back({"的哥哥",path1[i-1]});
                    }
                    else{
                        //a的哥哥的弟弟是a的弟弟
                        relation1to2.pop_back();
                        relation1to2.push_back({"的弟弟",path1[i-1]});
                    }
                }
                else if(relation1to2.back().first=="的弟弟"){
                    //前一个是的弟弟，删除前一个
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的弟弟",path1[i-1]});
                }
                else if(relation1to2.back().first=="的儿子"){
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的儿子",path1[i-1]});
                }
                else{
                    relation1to2.push_back({"的弟弟",path1[i-1]});
                }
            }
        }
        else if(path1[i-1]->lchild==path1[i]){
            while(relation1to2.back().first=="的哥哥" || relation1to2.back().first=="的弟弟"){
                relation1to2.pop_back();
            }
            relation1to2.push_back({"的父亲",path1[i-1]});
        }
    }
    for(int i=flag;i<(int)path2.size()-1;i++)
    {
        if(path2[i]->rbrother==path2[i+1]){
            //i+1出生日期小于i,即i+1是i的哥哥
            if(CompareDate(path2[i]->person.birthday,path2[i+1]->person.birthday)>0){
                if(relation1to2.back().first=="的哥哥"){
                    //前一个是的哥哥，删除前一个
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的哥哥",path2[i+1]});
                }
                else if(relation1to2.back().first=="的弟弟"){
                    //a的弟弟的哥哥和a作比较
                    //a的出生日期大于a的弟弟的哥哥的出生日期，即a的弟弟的哥哥是a的哥哥
                    if(CompareDate(relation1to2[relation1to2.size()-2].second->person.birthday,path2[i+1]->person.birthday)>0){
                        relation1to2.pop_back();
                        relation1to2.push_back({"的哥哥",path2[i+1]});
                    }
                    else{
                        //a的弟弟的哥哥是a的弟弟
                        relation1to2.pop_back();
                        relation1to2.push_back({"的弟弟",path2[i+1]});
                    }
                }
                else if(relation1to2.back().first=="的儿子"){
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的儿子",path2[i+1]});                 
                }
                else{
                    relation1to2.push_back({"的哥哥",path2[i+1]});
                }
            }
            else{
                if(relation1to2.back().first=="的哥哥"){
                    //a的哥哥的弟弟和a作比较
                    //a的出生日期大于a的哥哥的弟弟的出生日期，即a的哥哥的弟弟是a的哥哥
                    if(CompareDate(relation1to2[relation1to2.size()-2].second->person.birthday,path2[i+1]->person.birthday)>0){
                        relation1to2.pop_back();
                        relation1to2.push_back({"的哥哥",path2[i+1]});
                    }
                    else{
                        //a的哥哥的弟弟是a的弟弟
                        relation1to2.pop_back();
                        relation1to2.push_back({"的弟弟",path2[i+1]});
                    }
                }
                else if(relation1to2.back().first=="的弟弟"){
                    //前一个是的弟弟，删除前一个
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的弟弟",path2[i+1]});
                }
                else if(relation1to2.back().first=="的儿子"){
                    relation1to2.pop_back();
                    //更新出生日期
                    relation1to2.push_back({"的儿子",path2[i+1]});                 
                }
                else{
                    relation1to2.push_back({"的弟弟",path2[i+1]});
                }
            }
        }
        else if(path2[i]->lchild==path2[i+1]){
            relation1to2.push_back({"的儿子",path2[i+1]});
        }
    }
    vector<pair<string,Node *>> new_relation1to2;//name1的xxxxx是name2
    for (int i = 1;i<(int)relation1to2.size();i++)
    {
        if(relation1to2[i].first=="的父亲"){
            int cnt = 0;//记录辈分数
            while(i<(int)relation1to2.size() && relation1to2[i].first=="的父亲" && cnt<=3){
                cnt++;
                i++;
            }
            if(cnt==1){
                new_relation1to2.push_back({"的爸爸", relation1to2[i - 1].second});
            }
            else if(cnt==2){
                new_relation1to2.push_back({"的爷爷", relation1to2[i - 1].second});
            }
            else if(cnt==3){
                new_relation1to2.push_back({"的曾祖父", relation1to2[i - 1].second});
            }
            else if(cnt==4){
                new_relation1to2.push_back({"的高祖父", relation1to2[i - 1].second});
            }
            i--;
        }
        else if(relation1to2[i].first=="的儿子")
        {
            int cnt = 0;//记录辈分数
            while(i<(int)relation1to2.size() && relation1to2[i].first=="的儿子" && cnt<=4){
                cnt++;
                i++;
            }
            if(cnt==1){
                new_relation1to2.push_back({"的儿子", relation1to2[i - 1].second});
            }
            else if(cnt==2){
                new_relation1to2.push_back({"的孙子", relation1to2[i - 1].second});
            }
            else if(cnt==3){
                new_relation1to2.push_back({"的曾孙", relation1to2[i - 1].second});
            }
            else if(cnt==4){
                new_relation1to2.push_back({"的玄孙", relation1to2[i - 1].second});
            }
            else if(cnt==5){
                new_relation1to2.push_back({"的来孙", relation1to2[i - 1].second});
            }
            i--;
        }
        else{
            new_relation1to2.push_back(relation1to2[i]);
        }
    }
    SetColor("green");
    cout << "重要关系词说明如下：" << endl;
    cout << "长辈：爸爸-爷爷-曾祖父-高祖父"<< endl;
    cout << "晚辈：儿子-孙子-曾孙-玄孙-来孙" << endl;
    SetColor("white");
    cout << name1;
    for(int i=0;i<(int)new_relation1to2.size();i++)
    {
        if(i==(int)new_relation1to2.size()-1)
            cout << new_relation1to2[i].first;
        else
            cout << new_relation1to2[i].first<<"("<<new_relation1to2[i].second->person.name<<")"<< " -> ";
    }
    cout << "是" << name2 << endl;
    return;
}
bool FamilyTree::FindPath(FamilyTree::Node *p,string name,vector<FamilyTree::Node *> &path)
{
    if(p==nullptr)
        return false;
    path.push_back(p);
    if(p->person.name==name)
        return true;
    if(p->lchild){
        if(FindPath(p->lchild,name,path))
            return true;
    }
    if(p->rbrother){
        if(FindPath(p->rbrother,name,path))
            return true;
    }
    path.pop_back();
    return false;
}
vector<FamilyTree::Node *> FamilyTree::SearchPathToRoot(string name)
{
    vector<Node *> path;
    if(FindPath(root, name, path))
        return path;
    else{
        path.clear();
        return path;
    }
}
pair<FamilyTree::Node*,FamilyTree::Node*> FamilyTree::SearchByName(string searchname)
{
    Node *father=nullptr, *ans=nullptr;
    if(searchname==root->person.name){
        SetColor("green");
        cout << "查询成功(@_@)"<<endl;
        SetColor("white");
        ans = root;
        cout << "本人信息如下：" << endl;
        PrintPersonInfo(ans);
        cout << "父亲信息如下：根节点无父亲信息" << endl;
        cout << "孩子信息如下：" << endl;
        GetChildInfo(ans);
        return {father,ans};
    }
    else{
        queue<Node *> q;
        q.push(root);
        bool flag = false;
        while(q.size())
        {
            int size = q.size();
            while(size--){
                Node * tmp = q.front();
                q.pop();
                if(tmp->lchild){
                    q.push(tmp->lchild);
                    if(tmp->lchild->person.name==searchname){
                        father = tmp;
                        ans = tmp->lchild;
                        flag = true;
                        break;
                    }
                }
                Node * p = tmp->lchild;
                while(p&&p->rbrother){
                    q.push(p->rbrother);
                    if(p->rbrother->person.name==searchname){
                        father = tmp;
                        ans = p->rbrother;
                        flag = true;
                        break;
                    }
                    p = p->rbrother;
                }
            }
            if(flag)
                break;
        }
        if(father && ans){
            SetColor("green");
            cout << "查询成功(@_@)"<<endl;
            SetColor("white");
            cout << "本人信息如下：" << endl;
            PrintPersonInfo(ans);
            cout << "父亲信息如下：" << endl;
            PrintPersonInfo(father);
            cout << "孩子信息如下：" << endl;
            GetChildInfo(ans);
            return {father,ans};
        }
        else{
            SetColor("red");
            cout << "查询失败(T_T)" << endl;
            SetColor("white");
        }
    }
    return {nullptr,nullptr};
}
void FamilyTree::SearchByBirthdate(string lowerdate, string upperdate)
{
    if(CompareDate(lowerdate,upperdate)>0){
        SetColor("red");
        cout << "Error: 最早出生日期应该早于最晚出生日期" << endl;
        SetColor("white");
        return;
    }
    queue<Node *> q;
    q.push(root);
    bool flag = false;
    cout << "--------------------------begin-------------------------" << endl;
    while(q.size())
    {
        int size = q.size();
        while(size--){
            Node * tmp = q.front();
            if(CompareDate(tmp->person.birthday,lowerdate)>=0 && CompareDate(tmp->person.birthday,upperdate)<=0){
                flag = true;
                PrintPersonInfo(tmp);
            }
            q.pop();
            if(tmp->lchild)
                q.push(tmp->lchild);
            Node * p = tmp->lchild;
            while(p&&p->rbrother){
                q.push(p->rbrother);
                p = p->rbrother;
            }
        }
    }
    if(!flag){
        SetColor("red");
        cout << "查询不到出生日期符合条件的人"<<endl;
        SetColor("white");
    }
    cout << "---------------------------end--------------------------" << endl;
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
    FamilyTree familyTree;
    while(1)
    {
        //Sleep(1000);
        cout << "------------------Welcome to FamilyTree-----------------" << endl;
        cout << "---------------------------MENU-------------------------" << endl;
        cout << "1.从文件读取家谱" << endl;
        cout << "2.以图形方式显示家谱树" << endl;
        cout << "3.显示第n代所有人的信息" << endl;
        cout << "4.按照姓名查询，输出成员信息（包括其本人、父亲、孩子的信息）" << endl;
        cout << "5.按照出生日期查询成员名单" << endl;
        cout << "6.输入两人姓名，确定其关系" << endl;
        cout << "7.某成员添加孩子" << endl;
        cout << "8.删除某成员（若其还有后代，则一并删除）" << endl;
        cout << "9.修改某成员信息" << endl;
        cout << "10.以图形方式显示某一个人的后代家谱树" << endl;
        cout << "11.保存家谱到文件" << endl;
        cout << "12.退出菜单" << endl;
        cout << "--------------------------------------------------------" << endl;
        cout << "--------------------------------------------------------" << endl;
        int choice;
        cout << "请输入你的选择：";
        cin >> choice;
        if(cin.fail()){
            cin.clear();
            cin.ignore(1024,'\n');
            SetColor("red");
            cout << "Error: 请输入数字" << endl;
            SetColor("white");
            continue;
        }   
        if(familyTree.isEmptyTree()&& choice!=1&&choice!=12){
            SetColor("red");
            cout << "Error: 请先读取家谱文件" << endl;
            SetColor("white");
            continue;
        }
        switch(choice)
        {
            case 1:
            {
                string filename;
                cout << "输入文件名称:" ;
                cin >> filename;
                familyTree.createTreeFromFile(filename);
                cout << "家谱读取完成" << endl;
                break;
            }
            case 2:
            {
                familyTree.PrintTree();
                break;
            }
            case 3:
            {
                int generation;
                cout << "你想查看第几代人的信息？(输入0可查看所有代人的姓名)" << endl;
                cin >> generation;
                familyTree.printTreeLevelOrder(generation);
                break;
            }
            case 4:
            {
                string name;
                cout << "请输入待查询人员名称：";
                cin >> name;
                familyTree.SearchByName(name);
                break;
            }
            case 5:{
                string lowerdate,upperdate;
                cout << "请输入待查询出生日期的范围："<<endl;
                cout << "最早出生日期(xxxx.xx.xx):";
                cin >> lowerdate;
                cout << "最晚出生日期(xxxx.xx.xx):";
                cin >> upperdate;
                familyTree.SearchByBirthdate(lowerdate, upperdate);
                break;
            }
            case 6:{
                string name1,name2;
                cout << "请输入两人姓名："<<endl;
                cout << "第一位：";
                cin >> name1;
                cout << "第二位：";
                cin >> name2;
                familyTree.GetPersonRelation(name1, name2);
                break;
            }
            case 7:{
                string name1;
                cout << "请输入待添加孩子人员姓名："<<endl;
                cin >> name1;
                familyTree.InsertChild(name1, familyTree.CreatePersonByUser(),familyTree.SearchByName(name1));
                break;
            }
            case 8:{
                string name1;
                cout << "请输入待删除人员姓名："<<endl;
                cin >> name1;
                familyTree.DeletePersonAndChild(name1);
                break;
            }
            case 9:{
                string name1;
                cout << "请输入待修改信息人员姓名："<<endl;
                cin >> name1;
                familyTree.ModifyPersonInfo(name1);
                break;
            }
            case 10:{
                string name1;
                cout << "请输入待显示其后代的人员姓名："<<endl;
                cin >> name1;
                familyTree.PrintTreeOfPerson(familyTree.SearchByName(name1).second);
                break;
            }
            case 11:{
                string filename;
                cout << "请输入文件名称：";
                cin >> filename;
                familyTree.SaveTreeToFile(filename);
                break;} 
            case 12:{
                system("pause");
                return 0;
            }
            default:
                break;
        }
    }
    system("pause");
    return 0;
}
/*
1
input.txt
y
y
*/
