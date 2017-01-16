#include "nfa.h"

vector<string> state_name;

vector<NState> nsv_sort(vector<NState> ns) {
  vector<NState> result;

  for (auto &n : ns)
    result.push_back(n);

  NState t;
  int k;
  for (int i = 0; i < result.size() - 1; i++) {
    k = i;
    for (int j = i + 1; j < result.size(); j++) {
      if (stoi(result[j].name) < stoi(result[k].name))
        k = j;
    }
    t = result[k];
    result[k] = result[i];
    result[i] = t;
  }

  return result;
}

void NState::init(string &s) {
  auto pos = find(state_name.begin(), state_name.end(), s);
  // 如果s是新的状态，加入队列，初始化from
  if (pos == state_name.end()) {
    state_name.push_back(s);
    int id = state_name.size() - 1;
    this->init(id, s);
    // 否则直接初始化from
  } else {
    int id = distance(state_name.begin(), pos);
    this->init(id, s);
  }
}

void NFA::get_terminals(const vector<Edge> &edges) {
  for (auto &e : edges) {
    // 如果该终结符不存在，压入队列
    if (find(this->terminals.begin(), this->terminals.end(), e.terminal) ==
            this->terminals.end() &&
        e.terminal != '&')
      this->terminals.push_back(e.terminal);
  }
  cout << "该图中所有的终结符：" << endl;
  for (auto &t : this->terminals)
    cout << t << " ";
  cout << endl;
}

void NFA::get_states() {
  int i = 0;
  for (auto &s : state_name) {
    NState s_temp;
    s_temp.init(i, s);
    this->states.push_back(s_temp);
    i++;
  }

  cout << "请输入起始状态id：" << endl;
  int id;
  cin >> id;
  cout << "id: " << id << endl;

  auto it = states.begin() += id;
  it->flag = Start;

  cout << "请输入中止状态个数：" << endl;
  int count;
  cin >> count;
  cout << "请输入中止状态id：" << endl;
  int flag = 1;
  for (; count > 0; count--) {
    cin >> id;
    it = states.begin() + int(id);
    it->flag = End;
  }
}

// 计算状态总数
void NFA::get_states_num() { this->states_num = state_name.size(); }

// 计算边总数
void NFA::get_edges_num(vector<Edge> &edges) { this->edges_num = edges.size(); }

// 从文件读取NFA边信息
vector<Edge> NFA::readedges() {
  vector<Edge> edges;
  ifstream file("edges.txt");

  if (file.fail()) {
    cerr << "记录边信息的文件不存在！" << endl;
    assert(!file.fail());
  }

  string s;

  while (!file.eof()) {

    NState from, to;
    file >> s;
    from.init(s);

    file >> s;
    char t = s[0];

    file >> s;
    to.init(s);

    Edge e(from, t, to);
    edges.push_back(e);
  }
  edges.pop_back();

  this->get_terminals(edges);
  this->get_edges_num(edges);

  cout << "NFA中所有的边如下：" << endl;
  for (auto &e : edges)
    cout << e.nfrom.name << " " << e.terminal << " " << e.nto.name << endl;

  return edges;
}

// 将边转换成状态转换表
void NFA::edges2NFA() {
  edges = readedges();
  for (auto &e : edges)
    Ntran[e.nfrom.id][e.nto.id] = e.terminal;
}

// 判断是NFA还是DFA
int NFA::isNFA() {
  for (auto it1 = edges.begin(); it1 != edges.end(); it1++) {
    if (it1->terminal == '&')
      return 1;
    for (auto it2 = it1 + 1; it2 != edges.end(); it2++)
      if (it1->nfrom.id == it2->nfrom.id && it1->terminal == it2->terminal)
        return 1;
  }
  return 0;
}

// 计算能从T中某个状态出发，通过标号为a的转换到达的NFA状态的集合
vector<NState> NFA::move(vector<NState> T, char terminal) {
  // 能到达的状态的int集合
  vector<int> destination;
  // 能到达的状态的State集合
  vector<NState> destination_state;
  auto &di = destination;
  auto &ds = destination_state;

  // 在状态转换表中查找通过terminal能够到达的点的集合
  for (auto it = T.begin(); it != T.end(); it++) {
    for (int i = 0; i < edges_num; i++) {
      if (Ntran[it->id][i] == terminal &&
          find(destination.begin(), destination.end(), i) == destination.end())
        destination.push_back(i);
    }
  }

  for (int i = 0; i < destination.size(); i++) {
    ds.push_back(*(states.begin() + destination[i]));
  }

  return ds;
}

// 计算ε-closure
vector<NState> NFA::closure(vector<NState> T) {
  // 将T中的全部状态压入栈中，将ε-closure(T)初始化为T
  vector<int> ss, sv, cv; //状态栈，所有状态的int集合
  vector<NState> closure; //返回的closure集，closure集的int集合
  for (auto &s : states)
    sv.push_back(s.id);

  for (auto &s : closure)
    cv.push_back(s.id);

  for (auto &s : T) {
    ss.push_back(s.id);
    closure.push_back(s);
  }

  while (!ss.empty()) {
    // 将栈顶元素弹出栈中
    auto t = ss.begin() + ss.size() - 1;
    ss.pop_back();

    // 查找每一个从t出发有一个标号为ε的转换到达状态u
    for (int i = 0; i < edges_num; i++) {
      if (Ntran[*t][i] == '&' && find(begin(cv), end(cv), sv[i]) == end(cv)) {
        closure.push_back(*(states.begin() + i));
        ss.push_back(i);
        cv.push_back(i);
      }
    }
  }
  return closure;
}

// NFA状态转换为DFA状态
DState NState2DState(vector<NState> nstates) {
  DState dstate;
  for (auto &ns : nstates)
    dstate.states.push_back(ns);
  return dstate;
}

// DFA状态转换为NFA状态
vector<NState> DState2NState(DState dstates) {
  vector<NState> nstate;
  for (auto &ds : dstates.states)
    nstate.push_back(ds);
  return nstate;
}

// 判断Dstates中状态是否有未标记状态
int nflaged(DFA dfa) {
  for (auto &s : dfa.states)
    if (s.flag == 0) {
      return 1;
    }
  return 0;
}

// 找到第一个为标记的状态
int find_nflaged(DFA dfa) {
  for (auto it = dfa.states.begin(); it != dfa.states.end(); it++)
    if (it->flag == 0)
      return distance(dfa.states.begin(), it);
}

vector<int> nv_dv_compare(vector<DState> ds, vector<NState> nv) {
  vector<int> result;

  int flag[ds.size()];
  for (int i = 0; i < ds.size(); i++) {
    int count = 0;
    for (int j = 0; j < min(ds[i].states.size(), nv.size()); j++) {
      //如果元素相等，count++;
      if (ds[i].states[j].id == nv[j].id &&
          ds[i].states[j].flag == nv[j].flag &&
          ds[i].states[j].name == nv[j].name)
        count++;
    }
    // 如果当前vector中存在元素不相等，或者元素数量不相等
    // 则两个状态集不相等，flag设为0
    // 否则两个状态集相等，flag设为1
    if (count != nv.size() || ds[i].states.size() != nv.size())
      flag[i] = 0;
    else {
      flag[i] = 1;
    }
  }
  int count, pos;
  count = 0;
  for (int i = 0; i < ds.size(); i++) {
    count += flag[i];
    if (flag[i] == 1) {
      pos = i;
    }
  }

  // 如果不存在相等的元素
  if (count == 0)
    result.push_back(0);
  else
    result.push_back(1);
  result.push_back(pos);

  return result;
}

// 子集构造法
DFA NFA::subset_construction() {
  DFA dfa;
  DState ds;
  vector<NState> temp;

  // 计算开始符号的closure，并压入栈中
  temp.push_back(*states.begin());
  ds = NState2DState(nsv_sort(closure(temp)));
  dfa.states.push_back(ds);

  // 判断是否存在未标记的状态T
  while (nflaged(dfa)) {
    int b = find_nflaged(dfa);
    auto it = dfa.states.begin() + b;
    it->flag = 1;

    for (int i = 0; i < terminals.size(); i++) {
      vector<NState> U =
          nsv_sort(closure(move(DState2NState(dfa.states[b]), terminals[i])));

      // 如果没有相等的状态集
      vector<int> flag = nv_dv_compare(dfa.states, U);
      if (!(flag[0])) {
        dfa.states.push_back(NState2DState(U));
      }

      flag = nv_dv_compare(dfa.states, U);
      Edge e(b, terminals[i], flag[1]);
      dfa.Dtran.push_back(e);
    }
  }
  cout << "Dfa集合中所有状态：" << endl;
  for (auto &u : dfa.states) {
    for (auto &a : u.states)
      cout << a.name << " ";
    cout << endl;
  }

  cout << "Dfa集合中边如下：" << endl;
  for (int i = 0; i < dfa.Dtran.size(); i++) {
    cout << dfa.Dtran[i].dfrom << " " << dfa.Dtran[i].terminal << " "
         << dfa.Dtran[i].dto << endl;
  }

  return dfa;
}
