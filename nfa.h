#ifndef DFA_H
#define DFA_H

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

#define N 100

enum situation { End, Start };
typedef enum situation situation;

struct nfa_state {
  int id = 0, flag = 0; //标识id，是否被标记flag1，初始/终结状态flag2
  string name;          //状态别名

  nfa_state() = default;
  void init(int Id, string Name) {
    id = Id;
    name = Name;
  }
  void states_sort();
  void init(string &);
  void init(nfa_state other) {
    this->id = other.id;
    this->flag = other.flag;
    this->name = other.name;
  }
};
typedef struct nfa_state NState;

struct dfa_state {
  vector<NState> states;
  int flag = 0;
  string name;

  dfa_state() = default;
};
typedef struct dfa_state DState;

struct edge {
  NState nfrom;
  int dfrom;
  char terminal;
  NState nto;
  int dto;

  edge(NState &From, char &Terminal, NState &To) {
    nfrom = From;
    terminal = Terminal;
    nto = To;
  }
  edge(int &From, char &Terminal, int &To) {
    dfrom = From;
    terminal = Terminal;
    dto = To;
  }
};
typedef struct edge Edge;

// DFA结构体定义
struct DFA {
  vector<char> terminals;
  vector<DState> states;
  vector<Edge> Dtran;

  int states_num;
  DFA() = default;
};

// NFA结构体定义
struct NFA {
  vector<char> terminals; //终结符表
  vector<NState> states;  //状态集合
  vector<Edge> edges;     //边集合
  char Ntran[N][N];       //状态转换图

  int states_num, edges_num;
  NFA() {
    edges2NFA();
    get_states_num();
    get_states();
  }

  vector<Edge> readedges();
  void get_terminals(const vector<Edge> &);
  void get_states();
  void get_states_num();
  void get_edges_num(vector<Edge> &);
  void edges2NFA();
  int isNFA();
  vector<NState> move(vector<NState>, char);
  vector<NState> closure(vector<NState>);
  DFA subset_construction();
};
DState NState2DState(vector<NState>);

#endif
