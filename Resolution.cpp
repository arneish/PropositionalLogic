#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <assert.h>

using namespace std;

inline bool isInteger(const std::string &s)
{
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;

    char *p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}
template <typename Out>
void split(const string &s, char delim, Out result)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
    {
        *(result++) = item;
    }
}
vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}
void AbstractOut(string &expression, char op, unordered_map<int, string> &exp_map, int &map_counter)
{
    int i = 0;
    string new_exp;
    while (i < expression.size())
    {
        if (expression[i] == op)
        {
            string temp_exp = expression.substr(i - 1, 3);
            new_exp.erase(new_exp.begin() + new_exp.size() - 1);
            expression = new_exp + to_string(map_counter) + expression.substr(i + 2, expression.size() - i - 2);
            exp_map.insert(make_pair(map_counter++, temp_exp));
            new_exp.clear();
            i = 0;
        }
        else
        {
            new_exp += expression[i];
            i++;
        }
    }
}
string Parser(string &expression, unordered_map<int, string> &exp_map)
{
    /*Operators to support: AND, OR, NOT, EQUIVALECE, IMPLIES, BRACKETS
    */
    //1. Remove space if any
    string temp_exp;
    for (int i = 0; i < expression.size(); i++)
    {
        if (expression[i] != ' ')
            temp_exp += expression[i];
    }
    expression = temp_exp;
    //2. Parse all present brackets as Expressions
    string new_exp;
    int i = 0;
    int map_counter = exp_map.size();
    while (i < expression.size())
    {
        if (expression[i] == ')')
        {
            expression.erase(expression.begin() + i);
            //find its closing
            temp_exp.clear();
            int j = i - 1;
            while (expression[j] != '(')
            {
                temp_exp = expression[j] + temp_exp;
                expression.erase(expression.begin() + j);
                j--;
            }
            expression[j] = Parser(temp_exp, exp_map)[0];
            cerr << expression[j] << "\n";
            map_counter++;
            i = j + 1;
        }
        else
        {
            i++;
        }
    }
    map_counter = exp_map.size();
    cerr << expression << "\n";
    //3. Rearrange remaining expression to brackets in priority order
    //P.O: NOT, AND, OR, IMPLIES/EQUIVALENCE
    //3.1: Remove NOT
    i = 0;
    while (i < expression.size())
    {
        if (expression[i] == '-')
        {
            temp_exp.clear();
            temp_exp = expression.substr(i, 2);
            expression[i] = to_string(map_counter)[0];
            exp_map.insert(make_pair(map_counter++, temp_exp));
            expression.erase(i + 1, 1);
        }
        else
        {
            i++;
        }
    }
    //3.2: Remove AND
    AbstractOut(expression, '.', exp_map, map_counter);
    //3.3 Remove OR
    AbstractOut(expression, '+', exp_map, map_counter);
    //3.4 Remove Implies
    AbstractOut(expression, '<', exp_map, map_counter);
    //3.5 Remove Equivalence
    AbstractOut(expression, '=', exp_map, map_counter);
    return expression;
}

void Implies(vector<string> &child_exp)
{
    cerr<<"RULE 6: IMPLIES INVOKED ON "<<child_exp[0]<<"<"<<child_exp[1]<<"\n";
    child_exp[0] = '-' + child_exp[0];
    cerr<<"OUTPUT:"<<child_exp[0]<<"+"<<child_exp[1]<<"\n";
}

void modifyToNOT(vector<string> &child_exp, char &delim, unordered_map<int, string> &exp_map)
{
    cerr << "modifyToNot called:" << child_exp[0] << "," << delim << "," << child_exp[1] << "\n";
    if (delim == '+')
    {
        cerr<<"RULE 4: [NOT-ON-OR] INVOKED ON - "<<child_exp[0]<<"+"<<child_exp[1]<<"\n";
        child_exp[0] = '-' + child_exp[0];
        child_exp[1] = '-' + child_exp[1];
        delim = '.';
        cerr<<"OUTPUT:"<<child_exp[0]<<delim<<child_exp[1]<<"\n";
    }
    else if (delim == '.')
    {
        cerr<<"RULE 2: [NOT-ON-AND] INVOKED ON - "<<child_exp[0]<<"."<<child_exp[1]<<"\n";
        child_exp[0] = '-' + child_exp[0];
        child_exp[1] = '-' + child_exp[1];
        delim = '+';
        cerr<<"OUTPUT:"<<child_exp[0]<<delim<<child_exp[1]<<"\n";
    }
    else if (delim == '<')
    {
        cerr<<"RULE 6: [NOT-ON-IMPLIES] INVOKED ON - "<<child_exp[0]<<"<"<<child_exp[1]<<"\n";
        child_exp[1] = '-' + child_exp[1];
        delim = '.';
        cerr<<"OUTPUT:"<<child_exp[0]<<delim<<child_exp[1]<<"\n";
    }
    else if (delim == '=')
    {
        cerr<<"RULE 9: [NOT-ON-EQUALS] INVOKED!\n";
        int MB = exp_map.size();
        exp_map[MB] = '-' + child_exp[1];
        int MA = exp_map.size();
        exp_map[MA] = '-' + child_exp[0];
        int first = exp_map.size();
        exp_map[first] = child_exp[0] + '.' + to_string(MB);
        int second = exp_map.size();
        exp_map[second] = child_exp[1] + '.' + to_string(MA);
        child_exp[0] = to_string(first);
        child_exp[1] = to_string(second);
        delim = '+';
    }
    cerr << "modifyToNot returning:" << child_exp[0] << "," << delim << "," << child_exp[1] << "\n";
}

void clearBracket(string& str)
{
    if (str[0]=='('&&str[str.size()-1]==')')
        str = str.substr(1, str.size()-2);
}
vector<string> MorganDist(string &expression, unordered_map<int, string>& CNF_map)
{
    //deMorgan forms
    //-(_._)
    cerr<<"In MorganDist with expression:"<<expression<<"\n";
    int minus = -1;
    int i=0;
    for (i=0; i<expression.size(); i++)
    {
        if (expression[i]=='-'&&expression[i+1]=='(')
        {
            minus = i;
        }
        break;
    }
    if (minus!=-1)
    {
        string substring;
        char delim;
        while(expression[i+2]!=')')
        {
            substring+=expression[i+2];
            if (expression[i+2]=='.'||expression[i+2]=='+')
                delim = expression[i+2];
            i++;
        }
        cerr<<"MD substring:"<<substring<<"\n";
        vector<string> tokens = split(substring, delim);
        modifyToNOT(tokens, delim, CNF_map);
        expression = expression.substr(0, minus)+tokens[0]+delim+tokens[1]+expression.substr(i+3, expression.size()-(i+2));
        cerr<<"post minus expression:"<<expression<<"\n";
        //check for --r
    }
    cerr<<"minus:"<<minus<<"\n";
    //distributive
    int plus = -1;
    for (i=0; i<expression.size(); i++)
    {
        cerr<<"expression[i]:"<<expression[i]<<"\n";
        if (expression[i]=='+'&&(expression[i+1]=='('||expression[i-1]==')'))
        {
            plus = i;
            cerr<<"plus="<<plus<<"\n";
            break;
        }
    }
    if (plus!=-1)
    {
        if (expression[i+1]=='(')
        {
            string substring;
            char delim='!';
            while(expression[i+2]!=')')
            {
                substring+=expression[i+2];
                if (expression[i+2]=='.')
                    delim = expression[i+2];
                i++;
            }
            cerr<<"MD substring:"<<substring<<"\n";
            if (delim=='.')
            {
                vector<string> tokens = split(substring, delim);
                expression = '('+expression.substr(0, plus)+tokens[0]+')'+delim+'('+expression.substr(0, plus)+tokens[1]+')'+expression.substr(i+3, expression.size()-(i+2));
            }
            else 
                plus = -1;
        }
        else if (expression[i-1]==')')
        {
            string substring;
            char delim='0';
            while(expression[i-2]!='(')
            {
                substring=expression[i-2]+substring;
                if (expression[i-2]=='.')
                    delim = expression[i-2];
                i--;
            }
            cerr<<"MD substring:"<<substring<<"\n";
            if (delim=='.')
            {
                vector<string> tokens = split(substring, delim);
                expression = '('+tokens[0]+expression.substr(plus, expression.size()-plus)+')'+delim+'('+tokens[1]+expression.substr(plus, expression.size()-plus)+')';                
            }
            else 
                plus = -1;
        }
        //check for --r
    }
    cerr<<"plus:"<<plus<<"\n";

    //search a split about (.)
    for (i=0; i<expression.size();i++)
    {
        if (expression[i]=='.')
        {
            break;
        }
    }
    vector<string> result;
    if (i!=expression.size())
    {
        string one = expression.substr(0, i);
        clearBracket(one);
        string two = expression.substr(i+1, expression.size()-(i+1));
        clearBracket(two);
        cerr<<"one:"<<one<<" two:"<<two<<"\n";
        CNF_map[CNF_map.size()]=one;
        CNF_map[CNF_map.size()]=two;
        result = {one, two};
    }
    else
    {
        result = {expression};
    }
    return result;
}

void resolveNOT(string &expression)
{
    cerr << "resolveNOT called:" << expression << "\n";
    int count = 0, i = 0, j = 0;
    while (i < expression.size())
    {
        if (expression[i] == '-')
        {
            count++;
            j = i + 1;
        }
        if ((count) && (i == j))
            break;
        i++;
    }
    expression = expression.substr(j, expression.size() - j);
    if (count % 2)
    {
        expression = '-' + expression;
    }
    cerr << "resolveNOT returning:" << expression << "\n";
}

bool containsInteger(string str)
{
    for (int i=0; i<str.size(); i++)
    {
        if (isdigit(str[i]))
            return true;
    }
    return false;
}

vector<string> final_set;
void convert_process(char sign, string &current_exp, unordered_map<int, string>& CNF_map)
{
    cin.ignore();
    if (!containsInteger(current_exp))
    {
        cerr<<"pushing to final_set:"<<current_exp<<"\n";
        final_set.push_back(current_exp);
        return;
    }
    cerr<<"inside convert process:"<<current_exp<<"\n";
    assert(current_exp[0] != '-');
    char delim;
    for (int i = 0; i < current_exp.size(); i++)
    {
        if (current_exp[i] == '.')
        {
            delim = '.';
            break;
        }
        else if (current_exp[i] == '+')
        {
            delim = '+';
            break;
        }
    }
    vector<string> child_exp = split(current_exp, delim);
    cerr<<"child_exp0-1:"<<child_exp[0]<<" "<<child_exp[1]<<"\n";
    if (sign == '-')
        modifyToNOT(child_exp, delim, CNF_map);
    string connector;
    switch (delim)
    {
    case '.':
    {
        connector='.';
        break;
    }
    case '+':
    {
        connector = '+';
        break;
    }
    default:
        cerr<<delim<<"\n";
        assert(false);
    }

    string left, right;
    if (isInteger(child_exp[0]))
    {
        char sign;
        string new_exp;
        if (child_exp[0][0]=='-')
        { 
            sign = '-';
            new_exp = CNF_map[stoi(child_exp[0].substr(1, child_exp[0].size()-1))];
            cerr<<"new_exp:"<<new_exp<<"\n";
        }
        else 
        {
            sign = '+';
            new_exp = CNF_map[stoi(child_exp[0])];
        }
        cerr<<"sign:new_exp::"<<sign<<":"<<new_exp<<"\n";
        resolveNOT(new_exp);
        while (isInteger(new_exp))
        {
            if (new_exp[0] == '-')
            {
                string substring = new_exp.substr(1, substring.size() - 1);
                new_exp = '-' + CNF_map[stoi(substring)];
                resolveNOT(new_exp);
            }
            else
            {
                new_exp = CNF_map[stoi(new_exp)];
                resolveNOT(new_exp);
            }
        }
        if (new_exp.size()<=2)
        {
            if (sign=='-')
            {
                new_exp='-'+new_exp;
                resolveNOT(new_exp);
            }
            left = new_exp;
        }
        else if (sign == '-')
        {
            //new_exp = new_exp.substr(1, current_exp.size() - 1);
            //left = detect_split_process('-', new_exp, CNF_map);
            left = "-("+new_exp+')';           
        }
        else
        {
            left = '('+new_exp+')';
        }
    }
    else left = child_exp[0];
    cerr<<"left:"<<left<<"\n";
    if (isInteger(child_exp[1]))
    {
        char sign;
        string new_exp;
        if (child_exp[1][0]=='-')
        { 
            sign = '-';
            new_exp = CNF_map[stoi(child_exp[1].substr(1, child_exp[1].size()-1))];
            cerr<<"new_exp:"<<new_exp<<"\n";
        }
        else 
        {
            sign = '+';
            new_exp = CNF_map[stoi(child_exp[1])];
        }
        cerr<<"sign.new_exp:"<<sign<<"."<<new_exp<<"\n";
        resolveNOT(new_exp);
        while (isInteger(new_exp))
        {
            if (new_exp[0] == '-')
            {
                string substring = new_exp.substr(1, substring.size() - 1);
                new_exp = '-' + CNF_map[stoi(substring)];
                resolveNOT(new_exp);
            }
            else
            {
                new_exp = CNF_map[stoi(new_exp)];
                resolveNOT(new_exp);
            }
        }
        if (new_exp.size()<=2)
        {
            if (sign=='-')
            {
                new_exp='-'+new_exp;
                resolveNOT(new_exp);
            }
            right = new_exp;
        }
        else if (sign == '-')
        {
            //new_exp = new_exp.substr(1, current_exp.size() - 1);
            //left = detect_split_process('-', new_exp, CNF_map);
            right = "-("+new_exp+')';           
        }
        else
        {
            right = '('+new_exp+')';
        }
    }
    else right = child_exp[1];
    cerr<<"right:"<<right<<"\n";
    string result;
    if (left.size()==1)
        result=left+connector;
    else
        result =left+connector;
    if (right.size()==1)
        result+=right;
    else
        result+=right;
    vector<string> results = MorganDist(result, CNF_map);
    if (results.size()==0)
        return;
    else
    {
        for (auto&result:results)
        {
            convert_process('+', result, CNF_map);
        }
    } 
}

string detect_split_process(char sign, string &current_exp, unordered_map<int, string> &exp_map)
{
    cerr << "detect_split_process called:" << sign << " " << current_exp << "\n";
    assert(current_exp[0] != '-');
    char delim;
    for (int i = 0; i < current_exp.size(); i++)
    {
        if (current_exp[i] == '<')
        {
            delim = '<';
            break;
        }
        else if (current_exp[i] == '.')
        {
            delim = '.';
            break;
        }
        else if (current_exp[i] == '+')
        {
            delim = '+';
            break;
        }
        else if (current_exp[i] == '=')
        {
            delim = '=';
            break;
        }
    }
    vector<string> child_exp = split(current_exp, delim);
    cerr<<"child_exp0-1:"<<child_exp[0]<<" "<<child_exp[1]<<"\n";
    if (sign == '-')
        modifyToNOT(child_exp, delim, exp_map);
    string connector;
    switch (delim)
    {
    case '<':
    {
        Implies(child_exp);
        connector='+';
        break;
    }
    case '.':
    {
        connector='.';
        break;
    }
    case '+':
    {
        connector = '+';
        break;
    }
    case '=':
    {
        int map_size = exp_map.size();
        exp_map[map_size]=child_exp[0] + '<' + child_exp[1];
        exp_map[map_size+1]=child_exp[1] + '<' + child_exp[0];
        child_exp[0]=to_string(map_size);
        child_exp[1]=to_string(map_size+1);
        connector = '.';
        break;
    }
    default:
        cerr<<delim<<"\n";
        assert(false);
    }

    string left, right;
    if (isInteger(child_exp[0]))
    {
        char sign;
        string new_exp;
        if (child_exp[0][0]=='-')
        { 
            sign = '-';
            new_exp = exp_map[stoi(child_exp[0].substr(1, child_exp[0].size()-1))];
            cerr<<"new_exp:"<<new_exp<<"\n";
        }
        else 
        {
            sign = '+';
            new_exp = exp_map[stoi(child_exp[0])];
        }
        cerr<<"sign.new_exp:"<<sign<<"."<<new_exp<<"\n";
        resolveNOT(new_exp);
        while (isInteger(new_exp))
        {
            if (new_exp[0] == '-')
            {
                string substring = new_exp.substr(1, substring.size() - 1);
                new_exp = '-' + exp_map[stoi(substring)];
                resolveNOT(new_exp);
            }
            else
            {
                new_exp = exp_map[stoi(new_exp)];
                resolveNOT(new_exp);
            }
        }
        if (new_exp.size()<=2)
        {
            if (sign=='-')
            {
                new_exp='-'+new_exp;
                resolveNOT(new_exp);
            }
            left = new_exp;
        }
        else if (sign == '-')
        {
            //new_exp = new_exp.substr(1, current_exp.size() - 1);
            left = detect_split_process('-', new_exp, exp_map);
        }
        else
        {
            left = detect_split_process('+', new_exp, exp_map);
        }
    }
    else left = child_exp[0];
    cerr<<"left:"<<left<<"\n";
    if (isInteger(child_exp[1]))
    {
        char sign;
        string new_exp;
        if (child_exp[1][0]=='-')
        { 
            sign = '-';
            new_exp =exp_map[stoi(child_exp[1].substr(1, child_exp[1].size()-1))];
        }
        else 
        {
            sign = '+';
            new_exp = exp_map[stoi(child_exp[1])];
        }
        resolveNOT(new_exp);
        while (isInteger(new_exp))
        {
            if (new_exp[0] == '-')
            {
                string substring = new_exp.substr(1, substring.size() - 1);
                new_exp = '-' + exp_map[stoi(substring)];
                resolveNOT(new_exp);
            }
            else
            {
                new_exp = exp_map[stoi(new_exp)];
                resolveNOT(new_exp);
            }
        }
        if (new_exp.size()<=2)
        {
            if (sign=='-')
            {
                new_exp='-'+new_exp;
                resolveNOT(new_exp);
            }
            right = new_exp;
        }
        else if (sign == '-')
        {
            //new_exp = new_exp.substr(1, current_exp.size() - 1);
            right = detect_split_process('-', new_exp, exp_map);
        }
        else
        {
            right = detect_split_process('+', new_exp, exp_map);
        }    
    }
    else right = child_exp[1];
    cerr<<"right:"<<right<<"\n";
    string result;
    if (left.size()==1)
        result=left+connector;
    else
        result ='('+left+')'+connector;
    if (right.size()==1)
        result+=right;
    else
        result+='('+right+')';
    return result; 
}

void Print(unordered_map<int, string> exp_map)
{
    for (auto &elem : exp_map)
    {
        cerr << elem.first << " " << elem.second << "\n";
    }
}

string ConvertCNF(string expression, unordered_map<int, string>& exp_map)
{
    string current_exp = exp_map[stoi(expression)];
    cerr<<"current_exp:"<<current_exp<<"\n";
    return detect_split_process('+', current_exp, exp_map);
}

string FinalConvert(string expression, unordered_map<int, string>& CNF_map)
{
    string current_exp = CNF_map[stoi(expression)];
    cerr<<"current_exp:"<<current_exp<<"\n";
    convert_process('+', current_exp, CNF_map);
    return current_exp;
}



int main(int argc, char **argv)
{
    unordered_map<int, string> exp_map;
    unordered_map<int, string> CNF_map;
    string test = "p+q.(-r)<p";
    string expression = Parser(test, exp_map);
    cerr << expression << "\n";
    Print(exp_map);
    string CNFstring = ConvertCNF(expression, exp_map);
    cerr<<"CNF:"<<CNFstring<<"\n";
    string CNF_parsed = Parser(CNFstring, CNF_map);
    Print(CNF_map);
    string final_form = FinalConvert(CNF_parsed, CNF_map);
    cerr<<"CNF_parsed:"<<CNF_parsed<<"\n";
    cerr<<"final form:"<<final_form<<"\n";
    return 0;
}
