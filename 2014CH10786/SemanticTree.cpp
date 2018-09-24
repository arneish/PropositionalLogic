#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <assert.h>
#include "Semantics.h"
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

void OR(vector<string> &child_exp)
{
    cerr<<"RULE 3: OR RULE INVOKED ON "<<child_exp[0]<<"+"<<child_exp[1]<<"\n";
    return;
}

void AND(vector<string> &child_exp)
{
    cerr<<"RULE 1: AND RULE INVOKED ON "<<child_exp[0]<<"."<<child_exp[1]<<"\n";
    return;
}

void Implies(vector<string> &child_exp)
{
    cerr<<"RULE 6: IMPLIES INVOKED ON "<<child_exp[0]<<"<"<<child_exp[1]<<"\n";
    child_exp[0] = '-' + child_exp[0];
    cerr<<"OUTPUT:"<<child_exp[0]<<"+"<<child_exp[1]<<"\n";
}

void Minus_EQUALS(vector<string> &child_exp)
{
    return;
}

void Minus_Implies(vector<string> &child_exp)
{
    child_exp[1] = '-' + child_exp[1];
}

void Minus_OR(vector<string> &child_exp)
{
    child_exp[0] = '-' + child_exp[0];
    child_exp[1] = '-' + child_exp[1];
}
void Minus_AND(vector<string> &child_exp)
{
    child_exp[0] = '-' + child_exp[0];
    child_exp[1] = '-' + child_exp[1];
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

void prepareBranching(vector<string> &child_exp, char &delim, unordered_map<int, string> &exp_map)
{
    if (delim == '<')
    {
        Implies(child_exp);
        delim = '+';
    }
    else if (delim == '+')
    {
        OR(child_exp);
    }
    else if (delim == '=')
    {
        int first = exp_map.size();
        exp_map[first] = child_exp[0]+'<'+child_exp[1];
        exp_map[first+1] = child_exp[1]+'<'+child_exp[0];
        child_exp[0] = first;
        child_exp[1] = first+1;
        delim = '.';
    }
}

void Process_rule(struct Node *node, string cur_rule, unordered_map<int, string> &exp_map)
{
    cerr << "Process_rule invoked: " << cur_rule << "\n";
    //1. Handle NOTs
    resolveNOT(cur_rule);
    //3. If Integer and expression starts with '-'
    string current_exp;
    if (cur_rule[0] == '-')
    {
        current_exp = exp_map[stoi(cur_rule.substr(1, cur_rule.size() - 1))];
        resolveNOT(current_exp);
    }
    else //4. If Integer and does NOT start with '-'
    {
        current_exp = exp_map[stoi(cur_rule)];
        resolveNOT(current_exp);
    }
    //5.
    while (isInteger(current_exp))
    {
        if (current_exp[0] == '-')
        {
            string substring = current_exp.substr(1, substring.size() - 1);
            current_exp = "-!" + exp_map[stoi(substring)];
            resolveNOT(current_exp);
        }
        else
        {
            current_exp = exp_map[stoi(current_exp)];
            resolveNOT(current_exp);
        }
    }
    //6. If current_exp is now not an integer and sized<=2 (because p.2 is also not an integer) then create and return
    if (!isInteger(current_exp) && current_exp.size() <= 2)
    {
        node->rules.push_back(current_exp);
        return;
    }
    //definitely expression is not an integer and sized more than 2:
    //splitting extracted mappings
    char delim;
    vector<string> child_exp;
    int i = 0;
    while (i < current_exp.size())
    {
        if (current_exp[i] == '<' || current_exp[i] == '+' || current_exp[i] == '.' || current_exp[i] == '=')
        {
            delim = current_exp[i];
            child_exp = split(current_exp, delim);
            if (current_exp[0]=='-'&&current_exp[1]=='!')
            {
                child_exp[0]=child_exp[0].substr(2, child_exp[0].size()-2);
                modifyToNOT(child_exp, delim, exp_map);
            }
            break;
        }
        else
            i++;
    }
    //if (-) then applying minus rules"
    if (cur_rule[0] == '-')
        modifyToNOT(child_exp, delim, exp_map);
    else
    {
        prepareBranching(child_exp, delim, exp_map);
    }
    //if (.) then pushing to parent node rules only
    if (delim == '.')
    {
        node->rules.insert(node->rules.end(), {child_exp[0], child_exp[1]});
        return;
    }
    if (node->children.size() == 0)
    {
        cerr << "creating children with expressions:" << child_exp[0] << " " << child_exp[1] << "\n";
        struct Node *child_left = CreateTree(child_exp[0], exp_map);
        struct Node *child_right = CreateTree(child_exp[1], exp_map);
        node->children = {child_left, child_right};
    }
    else
    {
        for (auto &child : node->children)
        {
            Process_rule(child, cur_rule, exp_map);
        }
    }
}

bool containsDigit(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (isdigit(str[i]))
        {
            return true;
        }
    }
    return false;
}

void detect_split_process(char sign, string &current_exp, Node *parent, unordered_map<int, string> &exp_map)
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
    if (sign == '-')
        modifyToNOT(child_exp, delim, exp_map);
    switch (delim)
    {
    case '<':
    {
        Implies(child_exp);
        struct Node *child_left_i = CreateTree(child_exp[0], exp_map);
        struct Node *child_right_i = CreateTree(child_exp[1], exp_map);
        parent->children = {child_left_i, child_right_i};
        break;
    }
    case '.':
    {
        AND(child_exp);
        parent->rules.push_back(child_exp[0]);
        parent->rules.push_back(child_exp[1]);
        int i = 0;
        cerr << "iterating over node rules:\n";
        while (i < parent->rules.size())
        {
            string cur_rule = parent->rules[i];
            cerr << "cur_rule:" << cur_rule << "\n";
            if (containsDigit(cur_rule))
            {
                parent->rules.erase(parent->rules.begin() + i);
                Process_rule(parent, cur_rule, exp_map);
            }
            else
            {
                i++;
            }
        }
        parent->atom = true;
        cerr << "Node rules iteration over. Designating this as atomic node. Rules are:";
        for (auto &rule : parent->rules)
        {
            cerr << rule << " ";
        }
        cerr << "\n";
        break;
    }
    case '+':
    {
        OR(child_exp);
        struct Node *child_left_p = CreateTree(child_exp[0], exp_map);
        struct Node *child_right_p = CreateTree(child_exp[1], exp_map);
        parent->children = {child_left_p, child_right_p};
        break;
    }
    case '=':
    {
        cerr<<"RULE 8: [EQUALS RULE] INVOKED ON :"<<child_exp[0]<<"="<<child_exp[1]<<"\n";
        int map_size = exp_map.size();
        exp_map[map_size]=child_exp[0] + '<' + child_exp[1];
        exp_map[map_size+1]=child_exp[1] + '<' + child_exp[0];
        parent->rules.push_back(to_string(map_size));
        parent->rules.push_back(to_string(map_size+1));
        cerr<<"OUTPUT:"<<exp_map[map_size]<<"."<<exp_map[map_size+1]<<"\n";
        int i = 0;
        while (i < parent->rules.size())
        {
            string cur_rule = parent->rules[i];
            if (containsDigit(cur_rule))
            {
                parent->rules.erase(parent->rules.begin() + i);
                Process_rule(parent, cur_rule, exp_map);
            }
            else
            {
                i++;
            }
        }
        parent->atom = true;
        cerr << "Node rules iteration over. Designating this as atomic node. Rules are:";
        for (auto &rule : parent->rules)
        {
            cerr << rule << " ";
        }
        cerr << "\n";
        break;
    }
    default:
        assert(false);
    }
}

Node *CreateTree(string expression, unordered_map<int, string> &exp_map)
{
    cerr << "CreateTree called for expression:" << expression << "\n";
    //1. Handle NOTs
    resolveNOT(expression);
    struct Node *node = new Node;

    //2. If not integer then create node, store rule and return node
    if (!isInteger(expression))
    {
        node->atom = true;
        node->expression = expression;
        node->rules.push_back(expression);
        cerr << "Atomic node created with rules:";
        for (auto &elem : node->rules)
        {
            cerr << elem << " ";
        }
        cerr << "\n";
        return node;
    }

    //3. If Integer and expression starts with '-'
    string current_exp;
    if (expression[0] == '-')
    {
        current_exp = '-' + exp_map[stoi(expression.substr(1, expression.size() - 1))];
        resolveNOT(current_exp);
    }
    else
    {
        current_exp = exp_map[stoi(expression)];
        resolveNOT(current_exp);
    }
    //5.
    while (isInteger(current_exp))
    {
        if (current_exp[0] == '-')
        {
            string substring = current_exp.substr(1, substring.size() - 1);
            current_exp = '-' + exp_map[stoi(substring)];
            resolveNOT(current_exp);
        }
        else
        {
            current_exp = exp_map[stoi(current_exp)];
            resolveNOT(current_exp);
        }
    }

    //6. If current_exp is now not an integer and sized<=2 (because p.2 is also not an integer) then create and return
    if (!isInteger(current_exp) && current_exp.size() <= 2)
    {
        node->atom = true;
        node->expression = current_exp;
        node->rules.push_back(current_exp);
        cerr << "Atomic node created with rules:";
        for (auto &elem : node->rules)
        {
            cerr << elem << " ";
        }
        cerr << "\n";
        return node;
    }

    //definitely expression is not an integer and sized more than 2:
    cerr << "current_exp:" << current_exp << "\n";
    if (current_exp[0] == '-')
    {
        string new_exp = current_exp.substr(1, current_exp.size() - 1);
        detect_split_process('-', new_exp, node, exp_map);
    }
    else
    {
        node->expression = current_exp;
        detect_split_process('+', current_exp, node, exp_map);
    }
    return node;
}

void Print(unordered_map<int, string> exp_map)
{
    for (auto &elem : exp_map)
    {
        cerr << elem.first << " " << elem.second << "\n";
    }
}

bool consistency_check(vector<string> accumulator)
{
    unordered_map<string, int> status; //atom to +1 (positive) or -1 (not)
    string new_rule;
    for (auto &rule : accumulator)
    {
        if (rule[0] == '-')
            new_rule = rule.substr(1, rule.size() - 1);
        else
            new_rule = rule;
        if (!status.count(new_rule))
        {
            if (rule[0] == '-')
                status[new_rule] = -1;
            else
                status[new_rule] = 1;
        }
        else
        {
            int existing_status = status[new_rule];
            int new_status;
            if (rule[0] == '-')
                new_status = -1;
            else
                new_status = 1;
            if (new_status != existing_status)
                return false;
        }
    }
    return true;
}
void TreeTraversal(Node *root, vector<string> accumulator, vector<vector<string>> &open, vector<vector<string>> &closed)
{
    if (root->atom)
    {
        accumulator.insert(accumulator.end(), root->rules.begin(), root->rules.end());
        if (!consistency_check(accumulator))
        {
            closed.push_back(accumulator);
            cerr << "Inconsistent path!\n";
            for (auto &elem : accumulator)
            {
                cerr << elem << " ";
            }
            cerr<<"\n";
            return;
        }
    }
    for (auto &child_node : root->children)
    {
        TreeTraversal(child_node, accumulator, open, closed);
    }
    if (root->children.size() == 0)
    {
        cerr << "consistent path found!\n";
        open.push_back(accumulator);
        for (auto &elem : accumulator)
        {
            cerr << elem << " ";
        }
        cerr << "\n";
    }
}
int main(int argc, char **argv)
{
    unordered_map<int, string> exp_map;
    string test = argv[1];
    string expression = Parser(test, exp_map);
    cerr << expression << "\n";
    Print(exp_map);
    struct Node *root = CreateTree(expression, exp_map);
    vector<string> accumulator;
    vector<vector<string>> open;
    vector<vector<string>> closed;
    cerr<<"\n\n***********Traversing the Semantic Tableaux Tree for all paths:************\n";
    TreeTraversal(root, accumulator, open, closed);
    if (closed.size()==0)
        cerr<<"CONSISTENCE:Expression is valid for ALL models!\n";
    else if (open.size()==0)
        cerr<<"INCONSISTENCE:No valid Model found!\n";
    return 0;
}