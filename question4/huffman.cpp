#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <string>
#include <bitset>
#include <windows.h>
#include <algorithm>
#define INF 0x3f3f3f3f
void SetColor(string str);
class HuffmanTree
{
private:
    struct HTNode
    {
        int weight;
        int parent, lchild, rchild;
        HTNode()
        {
            parent = lchild = rchild = 0;
        }
    };
private:
    HTNode *HT;
    int chweight[256];
    vector<int> chset;//存储字符集,用int表示
    map<char, string> HC;//存储字符集对应的编码
    int len;
public:
    HuffmanTree()
    {
        HT = nullptr;
        memset(chweight, 0, sizeof(chweight));
        len = 0;
    }
    ~HuffmanTree()
    {
        delete[] HT;
        chset.clear();
        HC.clear();
    }
    void ReadFile()
    {
        ifstream fin;
        fin.open("source.txt", ios::in);
        string ans = "";
        //一行一行读取
        while(getline(fin,ans))
        {
            for (size_t i = 0; i < ans.size();i++)
            {
                chweight[int(ans[i])]++;//统计字符出现次数
            }
            chweight[int('\n')]++;//统计换行符出现次数
        }
        fin.close();
    }
    void CreateHufTree()
    {
        int n = 0;
        for (int i = 0; i < 256;i++)
        {
            if(chweight[i]!=0){
                n++;
                chset.push_back(int(i));
            }
        }
        HT = new HTNode[2 * n-1];
        for (int i = 0; i < n;i++)
        {
            HT[i].weight = chweight[chset[i]];
        }
        //构造霍夫曼树
        for (int i = n; i < 2*n-1;i++)
        {
            int firstmin_val;
            int secondmin_val;
            int firstmin=-1, secondmin=-1;
            firstmin_val = secondmin_val = INF;
            for (int j = 0; j <= i - 1;j++)
            {
                if(HT[j].parent==0 && HT[j].weight<firstmin_val){
                    secondmin_val = firstmin_val;
                    secondmin = firstmin;
                    firstmin_val = HT[j].weight;
                    firstmin = j;
                }
                else if(HT[j].parent==0 && HT[j].weight<secondmin_val){
                    secondmin_val = HT[j].weight;
                    secondmin = j;
                }
            }
            HT[i].weight = HT[firstmin].weight + HT[secondmin].weight;
            HT[i].lchild = firstmin;
            HT[i].rchild = secondmin;
            HT[i].parent = 0;
            HT[firstmin].parent = i;
            HT[secondmin].parent = i;
        }
    }
    void Write_Weight_Code()
    {
        //将权重和编码写入文件
        ofstream fout;
        fout.open("Huffman.txt", ios::out);
        vector<int> temp=chset;
        //选择排序从大到小
        for (size_t i = 0; i < temp.size()-1;i++)
        {
            int maxi = i;
            for (size_t j = i+1; j < temp.size();j++)
            {
                if(chweight[temp[j]]>chweight[temp[maxi]])
                    maxi = j;
            }
            if(maxi!=int(i)){
                swap(temp[i], temp[maxi]);
            }
        }
        for(size_t i=0;i<temp.size();i++){
            if(temp[i]!=10)
                fout << char(temp[i]) << " " << chweight[temp[i]] << " " << HC[temp[i]] << endl;
            else
                fout << "\\n" << " " << chweight[temp[i]] << " " << HC[temp[i]] << endl;
        }
        fout.close();
    }
    void Code()
    {
        for (size_t i = 0; i < chset.size();i++)
        {
            int c = i;
            string code;
            while(HT[c].parent!=0){
                int p = HT[c].parent;
                if(HT[p].lchild==c)
                    code = "0" + code;
                else
                    code = "1" + code;
                c = p;
            }
            HC[chset[i]] = code;
        }
    }
    void Print()
    {
        //打印编码
        for (size_t i = 0; i < chset.size();i++)
        {
            if(char(chset[i])!='\n')
                cout << char(chset[i]) << " " << HC[chset[i]] << endl;
            else
                cout << "\\n" << " " << HC[chset[i]] << endl;
        }
    }
    void GetSourceFileSize()
    {
        //获取源文件大小：单位字节
        ifstream fin;
        fin.open("source.txt", ios::in);
        fin.seekg(0, ios::end);
        int size = fin.tellg();
        cout << "源文件大小为：" << size << "字节" << endl;
        fin.close();
    }
    void GetCodeFileSize()
    {
        //获取编码文件大小：单位字节
        ifstream fin;
        fin.open("code.dat", ios::in | ios::binary);
        fin.seekg(0, ios::end);
        int size = fin.tellg();
        cout << "压缩文件大小为：" << size << "字节" << endl;
        fin.close();
    }
    void GetRecodeFileSize()
    {
        //获取解码文件大小：单位字节
        ifstream fin;
        fin.open("recode.txt", ios::in);
        fin.seekg(0, ios::end);
        int size = fin.tellg();
        SetColor("yellow");
        cout << "解压文件大小为：" << size << "字节" << endl;
        SetColor("white");
        fin.close();
        //对比源文件和解码文件是否一致
        ifstream fin1, fin2;
        fin1.open("source.txt", ios::in);
        fin2.open("recode.txt", ios::in);
        string s1, s2;
        while(getline(fin1,s1) && getline(fin2,s2)){
            if(s1!=s2){
                SetColor("red");
                cout << "源文件和解码文件不一致" << endl;
                SetColor("white");
                return;
            }
        }
        SetColor("green");
        cout << "源文件和解码文件一致" << endl;
        SetColor("white");
        fin1.close();
        fin2.close();
    }
    void WriteBinaryFile()
    {
        //将源文件写为二进制文件
        ifstream fin;
        fin.open("source.txt", ios::in);
        ofstream fout;
        fout.open("code.dat", ios::out | ios::binary);
        string temp,ans = "";
        while(getline(fin,temp)){
            for (size_t i = 0; i < temp.size();i++)
            {
                ans.append(HC[temp[i]]);    
            }
            ans.append(HC['\n']);
        }
        //cout<<"HC:"<<HC['\n']<<endl;
        for (size_t i = 0; i < ans.size();i+=8)
        {
            bitset<8> b(ans.substr(i, 8));
            char byteChar;
            if(i+8>ans.size()){
                len = ans.size() - i;
                bitset<8> b(ans.substr(i, len));
                byteChar = static_cast<char>(b.to_ulong());
            }
            else{
                byteChar = static_cast<char>(b.to_ulong());
            }
            fout << byteChar;
        }
        fin.close();
        fout.close();
    }
    void Decode()
    {
        //将二进制文件重新编码为源文件
        ifstream fin;
        fin.open("code.dat", ios::in | ios::binary);
        ofstream fout;
        fout.open("recode.txt", ios::out);
        char ch;
        string code = "";
        while(fin.read(&ch,1))
        {
            bitset<8> b(ch);
            code.append(b.to_string());
        }
        //取出后面的len位
        string last_str=code.substr(code.size()-len, len);
        //去掉8位后加上后面的len位
        code = code.substr(0, code.size() - 8).append(last_str);
        int root = 2 * chset.size()-2;
        for (size_t i = 0; i < code.size();i++)
        {
            if(code[i]=='0')
                root = HT[root].lchild;
            else
                root = HT[root].rchild;
            if(HT[root].lchild==0 && HT[root].rchild==0){
                fout << char(chset[root]);
                root = 2 * chset.size()-2;
            }
        }
        fin.close();
        fout.close();
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
    HuffmanTree huf;
    cout << "开始读取源文件source.txt..." << endl;
    huf.ReadFile();
    cout<<"读取源文件完成"<<endl<<"获取源文件大小(字节)..."<<endl;
    SetColor("yellow");
    huf.GetSourceFileSize();
    SetColor("white");
    cout<<"开始构造哈夫曼树..."<<endl;
    huf.CreateHufTree();
    SetColor("green");
    cout << "构造哈夫曼树完成" << endl;
    SetColor("white");
    cout<< "开始编码..." << endl;
    huf.Code();
    SetColor("green");
    cout << "编码完成" << endl;
    SetColor("white");
    cout << "写入权重和编码到Huffman.txt..." << endl;
    huf.Write_Weight_Code();
    SetColor("green");
    cout << "写入完成" << endl;
    SetColor("white");
    cout << "开始压缩..." << endl;
    huf.WriteBinaryFile();
    SetColor("green");
    cout << "压缩完成,文件名为code.dat" << endl;
    SetColor("white");
    cout << "获取压缩文件code.dat大小(字节)..." << endl;
    SetColor("yellow");
    huf.GetCodeFileSize();
    SetColor("white");
    cout << "开始解压..." << endl;
    huf.Decode();
    SetColor("green");
    cout << "解压完成,文件名为recode.txt" << endl;
    SetColor("white");
    cout<< "获取解压文件(recode.txt)大小以及将源文件和解压文件对比..." << endl;
    huf.GetRecodeFileSize();
    system("pause");
    return 0;
}