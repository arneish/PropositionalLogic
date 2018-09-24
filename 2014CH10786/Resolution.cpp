#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <set>
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
    cerr << "Parser:returning expression:" << expression << "\n";
    return expression;
}

void Implies(vector<string> &child_exp)
{
    cerr << "RULE 6: IMPLIES INVOKED ON " << child_exp[0] << "<" << child_exp[1] << "\n";
    child_exp[0] = '-' + child_exp[0];
    cerr << "OUTPUT:" << child_exp[0] << "+" << child_exp[1] << "\n";
}

void modifyToNOT(vector<string> &child_exp, char &delim, unordered_map<int, string> &exp_map)
{
    cerr << "modifyToNot called:" << child_exp[0] << "," << delim << "," << child_exp[1] << "\n";
    if (delim == '+')
    {
        cerr << "RULE 4: [NOT-ON-OR] INVOKED ON - " << child_exp[0] << "+" << child_exp[1] << "\n";
        child_exp[0] = '-' + child_exp[0];
        child_exp[1] = '-' + child_exp[1];
        delim = '.';
        cerr << "OUTPUT:" << child_exp[0] << delim << child_exp[1] << "\n";
    }
    else if (delim == '.')
    {
        cerr << "RULE 2: [NOT-ON-AND] INVOKED ON - " << child_exp[0] << "." << child_exp[1] << "\n";
        child_exp[0] = '-' + child_exp[0];
        child_exp[1] = '-' + child_exp[1];
        delim = '+';
        cerr << "OUTPUT:" << child_exp[0] << delim << child_exp[1] << "\n";
    }
    else if (delim == '<')
    {
        cerr << "RULE 6: [NOT-ON-IMPLIES] INVOKED ON - " << child_exp[0] << "<" << child_exp[1] << "\n";
        child_exp[1] = '-' + child_exp[1];
        delim = '.';
        cerr << "OUTPUT:" << child_exp[0] << delim << child_exp[1] << "\n";
    }
    else if (delim == '=')
    {
        cerr << "RULE 9: [NOT-ON-EQUALS] INVOKED!\n";
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

bool clearBracket(string &str)
{
    if (str[0] != '(')
        return false;
    int net_counter = 0;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '(')
        {
            net_counter += 1;
        }
        else if (str[i] == ')')
        {
            net_counter -= 1;
        }
        if (net_counter == 0 && i == str.size() - 1)
        {
            string new_str = str.substr(1, str.size() - 2);
            str = new_str;
            return true;
        }
        else if (net_counter == 0)
        {
            return false;
        }
    }
    return false;
}
vector<string> MorganDist(string &expression, unordered_map<int, string> &CNF_map)
{
    //deMorgan forms
    //-(_._)
    //clearBracket(expression);
    cerr << "In MorganDist with expression:" << expression << "\n";
    int minus = -1;
    int i = 0;
    for (i = 0; i < expression.size(); i++)
    {
        if (expression[i] == '-' && expression[i + 1] == '(')
        {
            minus = i;
        }
        break;
    }
    if (minus != -1)
    {
        string substring;
        char delim;
        while (expression[i + 2] != ')')
        {
            substring += expression[i + 2];
            if (expression[i + 2] == '.' || expression[i + 2] == '+')
                delim = expression[i + 2];
            i++;
        }
        cerr << "MD substring:" << substring << "\n";
        vector<string> tokens = split(substring, delim);
        modifyToNOT(tokens, delim, CNF_map);
        expression = expression.substr(0, minus) + tokens[0] + delim + tokens[1] + expression.substr(i + 3, expression.size() - (i + 2));
        cerr << "post minus expression:" << expression << "\n";
        //check for --r
    }
    cerr << "minus:" << minus << "\n";
    //distributive
    int plus = -1;
    for (i = 0; i < expression.size(); i++)
    {
        cerr << "expression[i]:" << expression[i] << "\n";
        if (expression[i] == '+' && (expression[i + 1] == '(' || expression[i - 1] == ')'))
        {
            plus = i;
            cerr << "plus=" << plus << "\n";
            break;
        }
    }
    if (plus != -1)
    {
        if (expression[i + 1] == '(')
        {
            string substring;
            char delim = '!';
            while (expression[i + 2] != ')')
            {
                substring += expression[i + 2];
                if (expression[i + 2] == '.')
                    delim = expression[i + 2];
                i++;
            }
            cerr << "MD substring:" << substring << "\n";
            if (delim == '.')
            {
                vector<string> tokens = split(substring, delim);
                expression = '(' + expression.substr(0, plus) + '+' + tokens[0] + ')' + delim + '(' + expression.substr(0, plus) + '+' + tokens[1] + ')' + expression.substr(i + 3, expression.size() - (i + 2));
                cerr << "expression plus right:" << expression << "\n";
            }
            else
                plus = -1;
        }
        else if (expression[i - 1] == ')')
        {
            string substring;
            char delim = '0';
            while (expression[i - 2] != '(')
            {
                substring = expression[i - 2] + substring;
                if (expression[i - 2] == '.')
                    delim = expression[i - 2];
                i--;
            }
            cerr << "MD substring:" << substring << "\n";
            if (delim == '.')
            {
                vector<string> tokens = split(substring, delim);
                expression = '(' + tokens[0] + expression.substr(plus, expression.size() - plus) + ')' + delim + '(' + tokens[1] + expression.substr(plus, expression.size() - plus) + ')';
                cerr << "expression plus left:" << expression << "\n";
            }
            else
                plus = -1;
        }
        //check for --r
    }
    cerr << "plus:" << plus << "\n";

    vector<string> result;
    //(1.3).6 || 6.(1.3) || (1.3).(6.7) || 1.2
    for (int i = 0; i < expression.size(); i++)
    {
        if (expression[i] == '.')
        {
            string left, right;
            left = expression.substr(0, i);
            right = expression.substr(i + 1, expression.size() - (i + 1));
            if (clearBracket(left))
            {
                result.push_back(left);
                clearBracket(right);
                result.push_back(right);
            }
            else if (clearBracket(right))
            {
                clearBracket(left);
                result.push_back(left);
                result.push_back(right);
            }
            if (result.size())
            {
                cerr << "result found!:" << left << "," << right << '\n';
                break;
            }
        }
    }
    if (result.size())
    {
        return result;
    }
    //search a split about (.)
    for (i = 0; i < expression.size(); i++)
    {
        if (expression[i] == '.')
        {
            break;
        }
    }
    if (i != expression.size())
    {
        string one = expression.substr(0, i);
        clearBracket(one);
        string two = expression.substr(i + 1, expression.size() - (i + 1));
        clearBracket(two);
        cerr << "one:" << one << " two:" << two << "\n";
        CNF_map[CNF_map.size()] = one;
        CNF_map[CNF_map.size()] = two;
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
    for (int i = 0; i < str.size(); i++)
    {
        if (isdigit(str[i]))
            return true;
    }
    return false;
}

vector<string> final_set;
vector<string> Qfinal_set;
bool query = false;

bool handleANDsplit(string &current_exp)
{
    for (int i = 0; i < current_exp.size(); i++)
    {
        if (current_exp[i] == '.')
        {
            //p.q
            string hANDs_1 = current_exp.substr(i + 1, current_exp.size() - (i + 1));
            string hANDs_2 = current_exp.substr(0, i);
            if (hANDs_1.size() <= 2 && hANDs_2.size() <= 2)
            {
                if (!query)
                {
                    final_set.push_back(hANDs_1);
                    final_set.push_back(hANDs_2);
                }
                else
                {
                    Qfinal_set.push_back(hANDs_1);
                    Qfinal_set.push_back(hANDs_2);
                }
                cerr << "handleANDsplit returning true!\n";
                return true;
            }
        }
    }
    return false;
}

void convert_process(char sign, string &current_exp, unordered_map<int, string> &CNF_map)
{
    cerr << "inside convert process:" << current_exp << "\n";
    if (!containsInteger(current_exp))
    {
        if (!handleANDsplit(current_exp))
        {
            cerr << "hANDs returned false!\n";
            cerr << "pushing to final_set:" << current_exp << "\n";
            if (!query)
                final_set.push_back(current_exp);
            else
                Qfinal_set.push_back(current_exp);
        }
        return;
    }
    assert(current_exp[0] != '-');

    //detect integer
    string new_exp;
    string substring;
    bool only_one = false;
    for (int i = 0; i < current_exp.size(); i++)
    {
        if (isdigit(current_exp[i]) && !only_one)
        {
            cerr << current_exp[i] << " " << int(current_exp[i]) - 48 << "\n";
            substring = CNF_map[int(current_exp[i]) - 48];
            cerr << "substring:" << substring << "\n";
            new_exp += '(' + substring + ')';
            only_one = true;
        }
        else
        {
            new_exp += current_exp[i];
        }
    }
    clearBracket(new_exp);
    cerr << "new_exp:" << new_exp << "\n";
    vector<string> results = MorganDist(new_exp, CNF_map);
    if (results.size() == 0)
        return;
    else
    {
        for (auto &result : results)
        {
            convert_process('+', result, CNF_map);
        }
    }
}

string detect_split_process(char sign, string &current_exp, unordered_map<int, string> &exp_map)
{
    cerr << "detect_split_process called:" << sign << " " << current_exp << "\n";
    assert(current_exp[0] != '-');
    clearBracket(current_exp);
    if (current_exp.size() == 1 && isalpha(current_exp[0]))
    {
        string result = current_exp;
        if (sign == '-')
            result = '-' + result;
        resolveNOT(result);
        return result;
    }
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
    cerr << "child_exp0-1:" << child_exp[0] << " " << child_exp[1] << "\n";
    if (sign == '-')
        modifyToNOT(child_exp, delim, exp_map);
    string connector;
    switch (delim)
    {
    case '<':
    {
        Implies(child_exp);
        connector = '+';
        break;
    }
    case '.':
    {
        connector = '.';
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
        exp_map[map_size] = child_exp[0] + '<' + child_exp[1];
        exp_map[map_size + 1] = child_exp[1] + '<' + child_exp[0];
        child_exp[0] = to_string(map_size);
        child_exp[1] = to_string(map_size + 1);
        connector = '.';
        break;
    }
    default:
        cerr << delim << "\n";
        assert(false);
    }

    string left, right;
    if (isInteger(child_exp[0]))
    {
        char sign;
        string new_exp;
        if (child_exp[0][0] == '-')
        {
            sign = '-';
            new_exp = exp_map[stoi(child_exp[0].substr(1, child_exp[0].size() - 1))];
            cerr << "new_exp:" << new_exp << "\n";
        }
        else
        {
            sign = '+';
            new_exp = exp_map[stoi(child_exp[0])];
        }
        cerr << "sign.new_exp:" << sign << "." << new_exp << "\n";
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
        if (new_exp.size() <= 2)
        {
            if (sign == '-')
            {
                new_exp = '-' + new_exp;
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
    else
        left = child_exp[0];
    cerr << "left:" << left << "\n";
    if (isInteger(child_exp[1]))
    {
        char sign;
        string new_exp;
        if (child_exp[1][0] == '-')
        {
            sign = '-';
            new_exp = exp_map[stoi(child_exp[1].substr(1, child_exp[1].size() - 1))];
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
        if (new_exp.size() <= 2)
        {
            if (sign == '-')
            {
                new_exp = '-' + new_exp;
                resolveNOT(new_exp);
            }
            right = new_exp;
        }
        else if (sign == '-')
        {
            right = detect_split_process('-', new_exp, exp_map);
        }
        else
        {
            right = detect_split_process('+', new_exp, exp_map);
        }
    }
    else
        right = child_exp[1];
    cerr << "right:" << right << "\n";
    string result;
    if (left.size() == 1)
        result = left + connector;
    else
        result = '(' + left + ')' + connector;
    if (right.size() == 1)
        result += right;
    else
        result += '(' + right + ')';
    return result;
}

void Print(unordered_map<int, string> exp_map)
{
    for (auto &elem : exp_map)
    {
        cerr << elem.first << " " << elem.second << "\n";
    }
}

string ConvertCNF(string expression, unordered_map<int, string> &exp_map)
{
    if (expression.size() == 1 && isalpha(expression[0]))
    {
        string result = expression;
        return result;
    }
    string current_exp = exp_map[stoi(expression)];
    while (isInteger(current_exp))
    {
        string new_exp;
        if (current_exp[0] == '-')
        {
            new_exp = current_exp.substr(1, current_exp.size() - 1);
            current_exp = '-' + exp_map[stoi(new_exp)];
        }
        else
        {
            new_exp = current_exp;
            current_exp = exp_map[stoi(new_exp)];
        }
        resolveNOT(current_exp);
    }
    cerr << "current_exp:" << current_exp << "\n";
    string new_exp;
    if (current_exp[0] == '-')
    {
        new_exp = current_exp.substr(1, current_exp.size() - 1);
        return detect_split_process('-', new_exp, exp_map);
    }
    return detect_split_process('+', current_exp, exp_map);
}

string FinalConvert(string expression, unordered_map<int, string> &CNF_map)
{
    cerr << "FinalConvert:" << expression << "\n";
    string current_exp;
    if (expression.size() == 1 && isalpha(expression[0]))
    {
        current_exp = expression;
    }
    else
        current_exp = CNF_map[stoi(expression)];
    cerr << "current_exp:" << current_exp << "\n";
    convert_process('+', current_exp, CNF_map);
    return current_exp;
}

bool Refute(string one, string two)
{
    if (one[0] == '-' && two[0] != '-')
    {
        one = one.substr(1, one.size() - 1);
        if (one == two)
            return true;
        else
            return false;
    }
    else if (two[0] == '-' && one[0] != '-')
    {
        two = two.substr(1, two.size() - 1);
        if (one == two)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

vector<string> Match(vector<string> d1, vector<string> d2)
{
    set<string> result;
    string new_clause;
    vector<string> all_possible;
    for (int i = 0; i < d1.size(); i++)
    {
        string literal1 = d1[i];
        string literal2;
        clearBracket(literal1);
        for (int j = 0; j < d2.size(); j++)
        {
            literal2 = d2[j];
            clearBracket(literal2);
            if (Refute(literal1, literal2))
            {
                for (int k = 0; k < d1.size(); k++)
                {
                    if (k != i)
                    {
                        string temp = d1[k];
                        clearBracket(temp);
                        result.insert(temp);
                    }
                }
                for (int k = 0; k < d2.size(); k++)
                {
                    if (k != j)
                    {
                        string temp = d2[k];
                        clearBracket(temp);
                        result.insert(temp);
                    }
                }
                for (auto &elem : result)
                {
                    new_clause += '(' + elem + ")+";
                }
                if (literal1[0] == '-')
                    literal1 = literal1.substr(1, literal1.size() - 1);
                new_clause = literal1 + "!@" + new_clause.substr(0, new_clause.size() - 1);
                all_possible.push_back(new_clause);
            }
        }
    }
    return all_possible;
}

void removeBracket(string &str)
{
    string result;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] != '(' && str[i] != ')')
        {
            result += str[i];
        }
    }
    str = result;
}
void ResolutionRefutation()
{
    vector<string> joint;
    joint.insert(joint.end(), final_set.begin(), final_set.end());
    joint.insert(joint.end(), Qfinal_set.begin(), Qfinal_set.end());
    assert(joint.size() == final_set.size() + Qfinal_set.size());
    cerr << "Joint initially:\n";
    for (auto &elem : joint)
    {
        cerr << elem << "\n";
        removeBracket(elem);
    }
    cerr << "After procesing initially for brackets!\n";
    for (auto &elem : joint)
    {
        cerr << elem << "\n";
    }

    int i = 0;
    int orig_size = joint.size();
    while (i < joint.size())
    {
        string clause1 = joint[i];
        for (int j = i + 1; j < joint.size(); j++)
        {
            string clause2 = joint[j];
            vector<string> disjuncts1 = split(clause1, '+');
            vector<string> disjuncts2 = split(clause2, '+');
            vector<string> new_clause_all = Match(disjuncts1, disjuncts2);
            if (new_clause_all.size())
            {
                for (auto &new_clause : new_clause_all)
                {
                    vector<string> tokens = split(new_clause, '!');
                    // for every following clause check this literal shouldn't conflict
                    bool isLiteralPrivate = true;
                    for (int m = j + 1; m < joint.size() && isLiteralPrivate; m++)
                    {
                        string clause_temp = joint[m];
                        vector<string> disjuncts_temp = split(clause_temp, '+');
                        for (auto &literal : disjuncts_temp)
                        {
                            if (tokens[0] == literal || ('-' + tokens[0] == literal))
                            {
                                isLiteralPrivate = false;
                                break;
                            }
                        }
                    }
                    if (isLiteralPrivate)
                    { //
                        new_clause = tokens[1];
                        new_clause = new_clause.substr(1, new_clause.size() - 1);
                        removeBracket(new_clause);
                        if (new_clause.size() == 0)
                        {
                            cerr << "TRUE!\n";
                            exit(0);
                        }
                        cerr << "new_clause:" << new_clause << "\n";
                        joint.push_back(new_clause);
                        joint.erase(joint.begin() + j);
                        joint.erase(joint.begin() + i);
                        break;
                    }
                }
            }
        }
        if (orig_size == joint.size())
        {
            i++;
        }
        else
            i = 0;
        orig_size = joint.size();
    }
    cerr << "printing joint at the end of process:\n";
    for (auto &elem : joint)
    {
        cerr << elem << "\n";
    }
    cerr << "FALSE!\n";
    exit(0);
}

int main(int argc, char **argv)
{
    unordered_map<int, string> exp_map;
    unordered_map<int, string> CNF_map;
    string test = argv[1];
    cerr << "test expression:" << test << "\n";
    string expression = Parser(test, exp_map);
    cerr << expression << "\n";
    Print(exp_map);
    string CNFstring = ConvertCNF(expression, exp_map);
    cerr << "PARSED:" << CNFstring << "\n";
    string CNF_parsed = Parser(CNFstring, CNF_map);
    Print(CNF_map);
    cerr << "here\n";
    string final_form = FinalConvert(CNF_parsed, CNF_map);
    cerr << "CNF_parsed:" << CNF_parsed << "\n";
    cerr << "final form:" << final_form << "\n";
    cerr << "***********************CNF final-set:\n";
    for (auto &elem : final_set)
    {
        cerr << elem << "\n";
    }

    //Query:
    query = true;
    unordered_map<int, string> Qexp_map;
    unordered_map<int, string> QCNF_map;
    string Qtest = argv[2];
    Qtest = "-(" + Qtest + ')';
    cerr << "\nNEGATION of query expression:" << Qtest << "\n";
    string Qexpression = Parser(Qtest, Qexp_map);
    cerr << Qexpression << "\n";
    Print(Qexp_map);
    string QCNFstring = ConvertCNF(Qexpression, Qexp_map);
    cerr << "QPARSED:" << QCNFstring << "\n";
    string QCNF_parsed = Parser(QCNFstring, QCNF_map);
    Print(QCNF_map);
    string Qfinal_form = FinalConvert(QCNF_parsed, QCNF_map);
    cerr << "QCNF_parsed:" << QCNF_parsed << "\n";
    cerr << "Qfinal form:" << Qfinal_form << "\n";
    cerr << "***********************QUERY CNF final-set:\n";
    for (auto &elem : Qfinal_set)
    {
        cerr << elem << "\n";
    }
    cerr<<"\n\n****************Evoking Resolution Refutation Procedure on CNF set***********\n";
    //Resolution Refutation
    ResolutionRefutation();
    return 0;
}
