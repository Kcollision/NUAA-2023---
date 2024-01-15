#include <iostream>
#include <windows.h>
#include <fstream>
using namespace std;
const int ALPHABET_SIZE = 26;

class DictTree{
private:
    struct TrieNode{
        TrieNode *children[ALPHABET_SIZE];
        bool isEndOfWord;
    };
public:
    TrieNode *root;
public:
    TrieNode *getNode()
    {
        TrieNode *pNode =new TrieNode;
        pNode->isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = nullptr;
        return pNode;
    }
    DictTree()
    {
        root = getNode();
    }
    ~DictTree()
    {
        Clear(root);
    }
    void Clear(TrieNode *p)
    {
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            if(p->children[i]){
                Clear(p->children[i]);
            }
        }
        delete p;
    }
    void Insert(string key);
    bool Search(string key);
    void Print(TrieNode *p,string str)
    {
        if(p->isEndOfWord){
            cout<<str<<endl;
        }
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            if(p->children[i]){
                Print(p->children[i],str+char(i+'a'));
            }
        }
    }
    void Save(TrieNode *p,ofstream &fout,string str)
    {
        if(p->isEndOfWord){
            fout<<str<<endl;
        }
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            if(p->children[i]){
                Save(p->children[i],fout,str+char(i+'a'));
            }
        }
    }
    TrieNode * Delete(TrieNode *proot,string key,int depth);
    bool isEmpty(TrieNode *root);//当一个节点没有子节点时，为空，返回true
    bool Delete(string key)
    {
        if(Search(key)){
            Delete(root,key,0);
            return true;
        }
        else{
            return false;
        }
    }
    bool Modify(string key,string newkey)
    {
        if(Delete(key)){
            Insert(newkey);
            return true;
        }
        else{
            return false;
        }
    }
};
void DictTree::Insert(string key)
{
    TrieNode *p = root;
    for (size_t i = 0; i < key.size();i++)
    {
        int index = key[i] - 'a';
        if(!p->children[index]){
            p->children[index] = getNode();
        }
        p = p->children[index];
    }
    p->isEndOfWord = true;
}
bool DictTree::Search(string key)
{
    TrieNode *p = root;
    for (size_t i = 0; i < key.size();i++)
    {
        int index = key[i] - 'a';
        if(!p->children[index]){
            return false;
        }
        p=p->children[index];
    }
    return (p!=nullptr && p->isEndOfWord);
}
DictTree::TrieNode *DictTree::Delete(TrieNode *proot,string key,int depth=0)
{
    if(!proot){
        return nullptr;
    }
    //到达最后一个字符
    if(depth==int(key.size())){
        //如果当前节点是一个单词的结尾,需要标记为false,代表删除了这个单词
        if(proot->isEndOfWord){
            proot->isEndOfWord = false;
        }
        //如果当前节点没有子节点，可以删除
        if(isEmpty(proot)){
            delete proot;
            proot = nullptr;
        }
        return proot;
    }
    int index = key[depth] - 'a';
    proot->children[index] = Delete(proot->children[index],key,depth+1);
    //回溯
    //最后一个字符已经被删除了，回溯到上一个字符，如果上一个字符没有子节点且不是单词结尾，删除
    if(isEmpty(proot)&&proot->isEndOfWord==false){
        delete proot;
        proot = nullptr;
    }
    return proot;
}
bool DictTree::isEmpty(TrieNode *p)
{
    for (int i = 0;i<ALPHABET_SIZE;i++)
    {
        if(p->children[i])
            return false;
    }
    return true;
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
    DictTree dict;
    while(1)
    {
        cout << "----------Welcome to mydict----------" << endl;
        cout << "1. Insert a word" << endl;
        cout << "2. Search a word" << endl;
        cout << "3. Delete a word" << endl;
        cout << "4. Modify a word" << endl;
        cout << "5. Show all words" << endl;
        cout << "6. Save to file" << endl;
        cout << "7. Read from file" << endl;
        cout << "8. Exit" << endl;
        cout << "-------------------------------------" << endl;
        cout<<"Please input your choice:";
        int choice;
        cin>>choice;
        if(cin.fail()){
            SetColor("red");
            cout<<"Please input the right choice!"<<endl;
            SetColor("white");
            cin.clear();
            cin.ignore(1024,'\n');
            continue;
        }
        switch(choice)
        {
            case 1:
            {
                cout << "Please input the word you want to insert:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string word;
                cin>>word;
                dict.Insert(word);
                SetColor("yellow");
                cout<<"Insert successfully!"<<endl;
                SetColor("white");
                break;
            }
            case 2:
            {
                cout << "Please input the word you want to search:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string word;
                cin>>word;
                if(dict.Search(word)){
                    SetColor("yellow");
                    cout<<"The word is in the dict!"<<endl;
                    SetColor("white");
                }
                else{
                    SetColor("red");
                    cout<<"The word is not in the dict!"<<endl;
                    SetColor("white");
                }
                break;
            }
            case 3:
            {
                cout << "Please input the word you want to delete:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string word;
                cin>>word;
                if(dict.Delete(word)){
                    SetColor("yellow");
                    cout<<"Delete successfully!"<<endl;
                    SetColor("white");
                }
                else{
                    SetColor("red");
                    cout<<"The word is not in the dict!"<<endl;
                    SetColor("white");
                }
                break;
            }
            case 4:
            {
                cout << "Please input the word you want to modify:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string word;
                cin>>word;
                cout << "Please input the new word:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string newword;
                cin>>newword;
                if(dict.Modify(word,newword)){
                    SetColor("yellow");
                    cout<<"Modify successfully!"<<endl;
                    SetColor("white");
                }
                else{
                    SetColor("red");
                    cout<<"The word is not in the dict!"<<endl;
                    SetColor("white");
                }
                break;
            }
            case 5:
            {
                
                cout << "All words in the dict:" << endl;
                SetColor("yellow");
                dict.Print(dict.root,"");
                SetColor("white");
                break;
            }
            case 6:
            {
                cout << "Please input the file name:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string filename;
                cin>>filename;
                ofstream fout;
                fout.open(filename,ios::out);
                if(!fout){
                    SetColor("red");
                    cout<<"Open file failed!"<<endl;
                    SetColor("white");
                    break;
                }
                dict.Save(dict.root,fout,"");
                fout.close();
                SetColor("yellow");
                cout<<"Save successfully!"<<endl;
                SetColor("white");
                break;
            }
            case 7:
            {
                cout << "Please input the file name:" << endl;
                SetColor("yellow");
                cout<< ">>>";
                SetColor("white");
                string filename;
                cin>>filename;
                ifstream fin;
                fin.open(filename,ios::in);
                if(!fin){
                    SetColor("red");
                    cout<<"Open file failed!"<<endl;
                    SetColor("white");
                    break;
                }
                string word;
                while(fin>>word){
                    dict.Insert(word);
                }
                fin.close();
                SetColor("yellow");
                cout<<"Read successfully!"<<endl;
                SetColor("white");
                break;
            }
            case 8:
            {
                cout << "-----------------Bye!----------------" << endl;
                system("pause");
                return 0;
            }
            default:
            {
                SetColor("red");
                cout<<"Please input the right choice!"<<endl;
                SetColor("white");
                break;
            }
        }
    }
    return 0;
}