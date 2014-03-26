#include "solver.h"

namespace vc_solver {
double solver::solve(const vector<pair<int, int> >& edges,
                     const vector<double>& weight, vector<int>& vc) {
  i_.init(edges, weight);
  best_solution_.clear();
  best_solution_weight_ = INF_DOUBLE;
  if (false && components());
  else {

//    printf("%*sSOLVE: %d %d\n", depth_, "", n(), (int)edges.size());
    r_.reduce_without_flow();
    i_.flow().maximize_dinic();
    dfs();
  }

  vc = best_solution_;
  return best_solution_weight_;
}

void solver::dfs() {
  int rev = i_.revision();
  r_.reduce();
  i_.flow().maximize_ff();
  if (b_.lower_bound() >= best_solution_weight_) {
    i_.revert(rev);
    return;
  }
  if (components()) {
    i_.revert(rev);
    return;
  }

  int v = select_branching_vertex();
  if (v == -1) {
    best_solution_weight_ = i_.solution(best_solution_);
  }
  else {
    for (int x : {0, 2}) {
      int rev2 = i_.fix(v, x);
      ++depth_;
      dfs();
      --depth_;
      i_.revert(rev2);
    }
  }
  i_.revert(rev);
}

int solver::select_branching_vertex() {
  int best_v = -1, best_d = -1;
  rep (v, i_.n()) {
    if (i_.value(v) == 1 && i_.degree(v) > best_d) {
      best_v = v;
      best_d = i_.degree(v);
    }
  }
  return best_v;
}

bool solver::components() {
  // Count the number of alive vertices
  int num_total_vs = 0;
  rep (v, n()) if (value(v) == 1 && degree(v) > 0) ++num_total_vs;
  if (num_total_vs == 0) return false;

  // Check if really the graph is disconnected
  rep (s, n()) {
    if (value(s) != 1 || degree(s) == 0) continue;
    queue<int> que;
    vector<bool> vis(n());
    que.push(s);
    vis[s] = true;
    int num_component_vs = 0;
    while (!que.empty()) {
      int u = que.front();
      que.pop();
      ++num_component_vs;
      for (int x : adj(u)) {
        if (value(x) == 1 && !vis[x]) {
          vis[x] = true;
          que.push(x);
        }
      }
    }
    if (num_component_vs == num_total_vs) return false;
    else break;
  }

  // Decompose
  vector<int> new_id(n(), -1);
  vector<vector<int>> cc_vs;
  vector<vector<pair<int, int>>> cc_es;
  rep (s, n()) {
    if (value(s) != 1 || degree(s) == 0 || new_id[s] != -1) continue;
    cc_vs.emplace_back();
    cc_es.emplace_back();
    vector<int> &vs = cc_vs.back();
    vector<pair<int, int>> &es = cc_es.back();

    queue<int> que;
    que.push(s);
    new_id[s] = 0;
    vs.push_back(s);

    while (!que.empty()) {
      int v = que.front();
      que.pop();
      for (int u : adj(v)) {
        if (value(u) != 1) continue;
        if (new_id[u] != -1) {
          if (new_id[u] < new_id[v]) es.emplace_back(new_id[u], new_id[v]);
        } else {
          que.push(u);
          new_id[u] = vs.size();
          vs.emplace_back(u);
        }
      }
    }
  }

  // Solve each connected components
  vector<bool> in_vc(n(), false);
  int num_ccs = cc_vs.size();
  vector<int> cc_ord(num_ccs);
  iota(all(cc_ord), 0);
  sort(all(cc_ord), [&](int i, int j) { return cc_vs[i].size() < cc_vs[j].size(); });

  int hoge = 0;
  rep (i, num_ccs) hoge += cc_vs[i].size();
  assert(hoge < n());

  solver s(depth_);
  rep (k, num_ccs) {
    const vector<int> &vs = cc_vs[cc_ord[k]];
    const vector<pair<int, int>> &es = cc_es[cc_ord[k]];
    vector<double> ws(vs.size());
    rep (i, vs.size()) ws[i] = weight(vs[i]);
    vector<int> vc;
    s.solve(es, ws, vc);
    for (int i : vc) in_vc[vs[i]] = true;
  }
  int rev = i_.revision();
  rep (v, n()) if (value(v) == 1) i_.fix(v, in_vc[v] ? 2 : 0);
  if (i_.solution_weight() < best_solution_weight_) {
    best_solution_weight_ = i_.solution(best_solution_);
  }
  i_.revert(rev);

  return true;
}
}  // namespace vc_solver
