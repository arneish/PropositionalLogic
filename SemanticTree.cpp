#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<string>
#include<unordered_map>
using namespace std;


void AbstractOut(string& expression, char op, unordered_map<int, string>& exp_map, int& map_counter)
{
    int i=0;
    string new_exp;
    while (i<expression.size())
    {
        if (expression[i]==op)
        {
            string temp_exp=expression.substr(i-1,3);
            new_exp.erase(new_exp.begin()+new_exp.size()-1);
            expression = new_exp+to_string(map_counter)+expression.substr(i+2, expression.size()-i-2);
            exp_map.insert(make_pair(map_counter++, temp_exp));
            new_exp.clear();
            i=0;
        }
        else
        {   
            new_exp+=expression[i];
            i++;
        }
    }
    //cerrcerr<<"Expression:"<<expression<<"\n";
}

string Parser(string& expression, unordered_map<int, string>& exp_map)
{
    /*Operators to support: AND, OR, NOT, EQUIVALECE, IMPLIES, BRACKETS
    */
    //1. Remove space if any
    string temp_exp;
    for (int i=0; i<expression.size(); i++)
    {
        if (expression[i]!=' ')
            temp_exp+=expression[i];
    } 
    expression = temp_exp;
    //2. Parse all present brackets as Expressions
    string new_exp;
    int i=0;
    int map_counter=exp_map.size();
    while(i<expression.size())
    {
        if (expression[i]==')')
        {
            expression.erase(expression.begin()+i);
            //find its closing
            temp_exp.clear();
            int j=i-1;
            while(expression[j]!='(')
            {
                temp_exp=expression[j]+temp_exp;
                expression.erase(expression.begin()+j);
                j--;
            }
            expression[j]=Parser(temp_exp, exp_map)[0];
            cerr<<expression[j]<<"\n";
            //cerr<<"before parser call:"<<expression<<"\n";
            
            map_counter++;
            //exp_map.insert(make_pair(map_counter++, temp_exp));
            i=j+1;
        }
        else
        {
            i++;
        }    
    }
    map_counter=exp_map.size();
    cerr<<expression<<"\n";
    //3. Rearrange remaining expression to brackets in priority order
    //P.O: NOT, AND, OR, IMPLIES/EQUIVALENCE
    //3.1: Remove NOT
    i=0;
    while (i<expression.size())
    {
        if (expression[i]=='-')
        {
            temp_exp.clear();
            temp_exp=expression.substr(i,2);
            expression[i]=to_string(map_counter)[0];
            exp_map.insert(make_pair(map_counter++, temp_exp));
            expression.erase(i+1, 1);
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

struct Node
{   
    string value;
    vector<Node*> children;
};

// bool Implies(string current_exp, vector<string>& result)
// {
//     result.push_back('-'+current_exp[0]);
//     result.push_back(current_exp[2]);
//     return true; //1--branch, 0--do not branch
// }

void CreateTree(string expression, unordered_map<int, string> exp_map)
{
    // Node node;
    // node.value = expression;
    // int counter = max_count;
    // while(counter!=0)
    // {
    //     bool branch;
    //     vector<string> result;
    //     string current_exp = exp_map[counter];
    //     if (current_exp[0]=='-')
    //     {

    //     }
    //     else if (current_exp[1]=='=')
    //     {

    //     }
    //     else if (current_exp[1]=='<')
    //     {
    //         branch = Implies(current_exp, result);
            

    //     }
    //     else if (current_exp[1]=='+')
    //     {

    //     }
    //     else if (current_exp[1]=='.')
    //     {

    //     }
    //     else
    //     {
    //         assert(false);
    //     }

    //     if (branch)
    //     {
    //         CreateTree(result[0], exp_map)
    //     }


    // }
}

void Print(unordered_map<int, string> exp_map)
{
    for (auto& elem: exp_map)
    {
        cerr<<elem.first<<" "<<elem.second<<"\n";
    }
}
int main(int argc, char** argv)
{
    unordered_map<int, string> exp_map;
    string test = "(p.q<r).(-p<s)";
    string expression = Parser(test, exp_map);
    cerr<<expression<<"\n";
    Print(exp_map);
    CreateTree(exp_map[exp_map.size()-1], exp_map);
    return 0;
}