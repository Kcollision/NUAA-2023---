#include <iostream>
#include <algorithm>
#include <stack>
#include <cmath>
#include <iomanip>
#include <cstdio>
#include <string>
using namespace std;

class Calculator
{
private:
    string infix_expression;//中缀表达式
    stack<char> operator_stack;//运算符栈,用于中缀转后缀
    stack<double> num_stack;//数字栈,用于后缀表达式计算
public:
    Calculator(string str)
    {
        infix_expression = str;
    }
    ~Calculator(){
        while(operator_stack.size()!=0){
            operator_stack.pop();
        }
        while(num_stack.size()!=0){
            num_stack.pop();
        }
    }
    bool CheckExpression();//检查表达式是否合法
    pair<double,bool> CalcInfix(); // 中缀表达式计算,返回值为计算结果和结果是否合法
    double CalcPostfix(string str);//后缀表达式计算
    int CalcPriority(char ch);//计算优先级
    double string_to_double(string str);//字符串转double
    string InfixToPostfix(string str);//中缀转后缀
    void AddZero();//补0
};
pair<double,bool> Calculator::CalcInfix()
{
    if(CheckExpression()==false){
        cout << "该表达式不合法！" << endl;
        return {0,false};
    }
    AddZero();
    return {CalcPostfix(InfixToPostfix(infix_expression)),true};
}
bool Calculator::CheckExpression()
{
    stack<char> left_bracket;
    //检验开头符号，不能是+*/.)
    if(infix_expression[0]=='+'||infix_expression[0]=='*'||infix_expression[0]=='/'||infix_expression[0]==')'||infix_expression[0]=='.'){
        cout << "输入错误：表达式开头不能是"<<infix_expression[0]<< endl;
        return false;
    }
    //检验左右括号是否匹配，运算符是否连续出现，以及是否出现其他非法符号
    for (size_t i = 0; i < infix_expression.size();i++)
    {
        if(infix_expression[i]=='('){
            left_bracket.push('(');
        }
        else if(infix_expression[i]==')'){
            if(left_bracket.size())
                left_bracket.pop();
            else{
                cout << "括号不匹配：缺失左括号" << endl;
                return false;
            }
        }
        else if(infix_expression[i]=='+'||infix_expression[i]=='-'
        ||infix_expression[i]=='*'||infix_expression[i]=='/'
        ||infix_expression[i]=='.'){
            if(i>0 && (infix_expression[i-1]=='+'||infix_expression[i-1]=='-'
            ||infix_expression[i-1]=='*'||infix_expression[i-1]=='/'
            ||infix_expression[i-1]=='.')){
                cout << "输入错误：运算符连续出现" << endl;
                return false;
            }
        }
        else if(infix_expression[i]!='+' && infix_expression[i]!='-' 
        && infix_expression[i]!='*' && infix_expression[i]!='/' 
        && infix_expression[i]!='.' 
        && (infix_expression[i]<'0' || infix_expression[i]>'9')){
            cout << "输入错误：出现非法字符" <<infix_expression[i]<< endl;
            return false;
        }
    }
    if(left_bracket.size()){
        cout << "括号不匹配：缺失右括号" << endl;
        return false;
    }
    return true;
}
void Calculator::AddZero()
{
    string str = infix_expression;
    for (int i = 0; i < (int)str.size();i++)
    {
        if(i>0){
            if(str[i]=='-'&&(str[i-1]>'9'||str[i-1]<'0')&&str[i-1]!='.'){
                str.insert(i, "0");
            }
        }
        else if(i==0){
            if(str[i]=='-'){
                str.insert(i, "0");
            }
        }
    }
    infix_expression = str;
    //cout <<"补0后:" <<infix_expression << endl;
}
int Calculator::CalcPriority(char ch)
{
    switch(ch){
        case '+':
            return 1;
        case '-':
            return 1;
        case '*':
            return 2;
        case '/':
            return 2;
        case '(':
            return 3;
        case ')':
            return 3;
        default:
            return 0;
    }
}
double Calculator::string_to_double(string str)
{
    double ans = 0;
    int i = 0;
    string integer, decimal;
    while(i<(int)str.size() && str[i]!='.')
    {
            integer += str[i];
            i++;
    }
    i++;
    while(i<(int)str.size())
    {
            decimal += str[i];
            i++;
    }
    for (int j = 0; j < (int)integer.size();j++)
    {
            ans += (double)(integer[j]-'0') * pow(10, integer.size() - j - 1);
    }
    double k = 0.1;
    for (int j = 0; j < (int)decimal.size();j++)
    {
            ans += (double)(decimal[j]-'0')*k;
            k /= 10;
    }
    //printf("%.8f", ans);
    return ans;
}
string Calculator::InfixToPostfix(string str)
{
    string ans="";//返回的后缀表达式
    cout << "----------------stack changes:-----------------" << endl;
    cout << "中缀转后缀 - 运算符栈变化过程:" << endl;
    for (int i = 0; i < (int)str.size();i++)
    {
        if((str[i]<='9' && str[i]>='0') ||str[i]=='.'){
            while((str[i]<='9' && str[i]>='0') ||str[i]=='.'){
                ans += str[i];
                i++;
            }
            ans += ' ';
            i--;
        }
        else if (str[i]!=')' && str[i]!='('){
            int priority = CalcPriority(str[i]);
            if(priority==0){
                cout << "输入错误" << endl;
            }
            else{
                if(operator_stack.size()==0){
                    operator_stack.push(str[i]);
                    cout<<"入栈:"<<str[i]<<' ';
                }
                else{
                    if(priority>CalcPriority(operator_stack.top())){
                        operator_stack.push(str[i]);
                        cout<<"入栈:"<<str[i]<<' ';
                    }
                    else{
                        while(operator_stack.size()!=0 && priority<=CalcPriority(operator_stack.top()) && operator_stack.top()!='('){
                            ans+=operator_stack.top();
                            cout<<"出栈:"<<operator_stack.top()<<' ';
                            operator_stack.pop();
                        }
                        operator_stack.push(str[i]);
                        cout<<"入栈:"<<str[i]<<' ';
                    }
                }
            }
        }
        else if(str[i]=='('){
            operator_stack.push(str[i]);
            cout<<"入栈:"<<str[i]<<' ';
        }
        else if(str[i]==')'){
            //遇到右括号的情况
            while(operator_stack.size()!=0 && operator_stack.top()!='(' ){
                ans += operator_stack.top();
                cout<<"出栈:"<<operator_stack.top()<<' ';
                operator_stack.pop();
            }
            operator_stack.pop();//弹出左括号
            cout<<"出栈:"<<'('<<' ';
        }
        //cout << ans << endl;
    }
    while(operator_stack.size()!=0){
        ans += operator_stack.top();
        cout<<"出栈:"<<operator_stack.top()<<' ';
        operator_stack.pop();
    }
    cout << endl<<"后缀表达式:" << ans << endl;
    return ans;
}
double Calculator::CalcPostfix(string str)//后缀表达式计算
{
        //储存数字字符串
        string num;
        cout<<"后缀表达式计算 - 数字栈变化过程:"<<endl;
        for (int i = 0; i < (int)str.size();i++)
        {
            while((str[i]<='9' && str[i]>='0') ||str[i]=='.'){
                num += str[i];
                i++;
            };
            if(num.size()!=0){
                num_stack.push(string_to_double(num));
                cout<<"入栈:"<<string_to_double(num)<<' ';
                num = "";
            }
            int priority = CalcPriority(str[i]);
            if(priority!=0){
                double a = num_stack.top();
                cout<<"出栈:"<<a<<' ';
                num_stack.pop();
                double b = num_stack.top();
                cout<<"出栈:"<<b<<' ';
                num_stack.pop();
                switch(str[i]){
                    case '+':
                        num_stack.push(b + a);
                        cout<<"入栈:"<<b+a<<' ';
                        break;
                    case '-':
                        num_stack.push(b - a);
                        cout<<"入栈:"<<b-a<<' ';
                        break;
                    case '*':
                        num_stack.push(b * a);
                        cout<<"入栈:"<<b*a<<' ';
                        break;
                    case '/':
                        num_stack.push(b / a);
                        cout<<"入栈:"<<b/a<<' ';
                        if(a==0){
                            cout <<endl<<"warning:除数不能为0" << endl;
                        } 
                        break;
                    default:break;
                }
            }
        }
        cout <<endl<< "---------------------over----------------------" << endl;
        return num_stack.top();
}
int main()
{
    int decimal_places=0;//小数位数
    cout << "------------Welcome to Calculator!-------------" << endl;
    while(1){
        cout << "----------------Calculator Menu----------------" << endl;
        cout << "支持小数运算,支持的运算符有+,-,*,/,()" << endl;
        cout << "1.设置显示小数位数" << endl;
        cout << "2.连续计算" << endl;
        cout << "3.退出" << endl;
        cout << "-----------------------------------------------" << endl;
        cout << "-----------------------------------------------" << endl;
        int choice;
        cout << "请输入选项:";
        cin >> choice;
        if(cin.fail()){
            cin.clear();//
            //清除缓冲区的内容
            cin.ignore(1024,'\n');
            cout << "输入错误，请重新输入" << endl;
            continue;
        }
        string str;
        //提取#到#中的字符串
        string temp;
        //计算结果
        pair<double,bool> answer;
        //标记是否开始提取
        bool flag;
        if(choice==3){
            break;
        }
        switch (choice)
        {
            case 1:
                cout<<"请输入小数位数:";
                cin >> decimal_places;
                break;
            case 2:
                cout << "---------------------begin---------------------" << endl;
                cout<<"当前小数位数为:"<<((decimal_places==0)?"默认值":to_string(decimal_places))<<endl;
                str.clear();temp.clear();
                while(1)
                {
                    cout <<endl<<"请输入表达式(中缀),仅识别#...#之间的表达式" << endl;
                    cout << "输入exit退出:";
                    cin >> str;
                    if(str=="exit")
                        break;
                    flag = false;
                    for (size_t i = 0; i < str.size();i++)
                    {
                        if(str[i]=='#'){
                            if(flag==false){
                                flag = true;
                                continue;
                            }
                            else{
                                flag = false;
                                cout<<endl<<">>>expression:"<<temp<<endl;
                                Calculator calculator(temp);
                                answer=calculator.CalcInfix();
                                if(!answer.second){
                                    temp.clear();
                                    continue;
                                }
                                else{
                                    cout << ">>>answer:";
                                    if(decimal_places==0){
                                        cout<< answer.first<< endl;
                                    }
                                    else{
                                        cout << setprecision(decimal_places);
                                        cout << answer.first<< endl;
                                    }
                                    temp.clear();
                                    continue;
                                }
                            }
                        }
                        if(flag){
                            temp+=str[i];
                        }
                    }
                }
                cout << "---------------------end---------------------" << endl;
                break;
            default:
                cout << "输入选项错误,请重新输入!" << endl;
                break;
        }
    }
    cout<<"-----------------Goodbye!-----------------"<<endl;
    system("pause");
    return 0;
}
/* 
#-4*2+3#
#3-4*2##(-5/6)+2#
#3-4*2##(-5/6)+2
#(-5+5)/(3-3)#
#3-4**2##(-5/6)+2)#
*/