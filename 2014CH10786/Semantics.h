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

struct Node
{
    bool atom = false;
    string expression;
    vector<string> rules;
    vector<Node *> children;
};

Node *CreateTree(string expression, unordered_map<int, string> &exp_map);
void Process_rule(struct Node *node, string cur_rule, unordered_map<int, string> &exp_map);
void detect_split_process(char sign, string &current_exp, Node *parent, unordered_map<int, string> &exp_map);
