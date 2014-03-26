#include "common.h"
#include "pre_reducer.h"

namespace vc_solver {
double pre_reducer::stat_time_total_ = 0;

pre_reducer::pre_reducer(const vector<pair<int, int> >& original_edges,
                         const vector<double>& original_weight) {
  original_edges_ = original_edges;
  original_weight_ = original_weight;

  V_ = original_weight.size();
  adj_.assign(V_, vector<int>());
  for (auto e : original_edges) {
    adj_[e.first ].emplace_back(e.second);
    adj_[e.second].emplace_back(e.first);
  }
  weight_ = original_weight;

  status_.assign(V_, NORMAL);
}

void pre_reducer::reduce() {
  rep (iteration, 10) {
    modified_ = false;
    reduce_low_degree();
    reduce_domination();
    if (!modified_) break;
  }

  JLOG_ADD_OPEN("reduction") {
    JLOG_PUT("num_deleted_vs", count(all(status_), DELETED));
    JLOG_PUT("num_fixed_vs", count(all(status_), FIXED));
  }
}

void pre_reducer::get_reduced_problem(vector<pair<int, int> >& reduced_edges,
                                      vector<double>& reduced_weight) {
  int red_num_v = 0;
  v_red2org_.clear();
  v_org2red_.assign(V_, -1);

  rep (ov, V_) {
    if (status_[ov] == NORMAL) {
      v_red2org_.emplace_back(ov);
      v_org2red_[ov] = red_num_v++;
    }
  }

  reduced_weight.resize(red_num_v);
  rep (rv, red_num_v) {
    reduced_weight[rv] = original_weight_[v_red2org_[rv]];
  }

  reduced_edges.clear();
  for (auto p : original_edges_) {
    if (v_org2red_[p.first] >= 0 && v_org2red_[p.second] >= 0) {
      reduced_edges.emplace_back(v_org2red_[p.first],
                                 v_org2red_[p.second]);
    }
  }
}

double pre_reducer::get_fixed_weight() {
  double w = 0;
  rep (v, V_) if (status_[v] == FIXED) w += weight_[v];
  return w;
}

double pre_reducer::get_original_solution(const vector<int>& reduced_solution,
                                      vector<int>& original_solution) {
  double w = 0;
  original_solution.clear();

  for (int rv : reduced_solution) {
    int ov = v_red2org_[rv];
    original_solution.emplace_back(ov);
    w += original_weight_[ov];
  }

  rep (ov, V_) {
    if (status_[ov] == FIXED) {
      original_solution.emplace_back(ov);
      w += original_weight_[ov];
    }
  }

  return w;
}

void pre_reducer::reduce_low_degree() {
  STAT_TIMER(stat_time_total_) {
    vector<int> deg(V_, 0);

    rep (v, V_) {
      if (status_[v] != NORMAL) continue;
      for (int u : adj_[v]) ++deg[u];
    }

    // TODO: degree 2
    queue<int> que;
    rep (v, V_) {
      if (deg[v] <= 1 && status_[v] == NORMAL) que.push(v);
    }

    while (!que.empty()) {
      int v = que.front();
      que.pop();
      if (status_[v] != NORMAL) continue;

      if (deg[v] == 0) {
        status_[v] = DELETED;
        modified_ = true;
      } else if (deg[v] == 1) {
        // Find the only one neighbor vertex
        int u = -1;
        for (int x : adj_[v]) {
          if (status_[x] == NORMAL) {
            assert(u == -1);
            u = x;
          }
        }
        assert(u != -1);
        if (weight_[u] > weight_[v]) continue;

        // Fix |u| and remove edges from |u|
        for (int x : adj_[u]) {
          if (--deg[x] <= 1 && status_[v] == NORMAL) que.push(x);
        }
        fix_vertex(u);
        status_[v] = DELETED;
        modified_ = true;
      } else {
        assert(false);
      }
    }
  }
}

void pre_reducer::reduce_domination() {
  STAT_TIMER(stat_time_total_) {
    rep (v, V_) sort(all(adj_[v]));

    vector<bool> a(V_, false);

    rep (v, V_) {
      if (status_[v] != NORMAL) continue;

      a[v] = true;
      for (int u : adj_[v]) a[u] = true;

      for (int u : adj_[v]) {
        if (status_[u] != NORMAL) continue;

        if (weight_[v] > weight_[u] ||
            adj_[v].size() < adj_[u].size()) continue;

        for (int x : adj_[u]) if (!a[x]) goto dmp;

        // |v| dominates |u|
        status_[v] = FIXED;
        modified_ = true;
        break;

        dmp:;
      }

      a[v] = false;
      for (int u : adj_[v]) a[u] = false;
    }

    purify_edges();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Graph manipulation
///////////////////////////////////////////////////////////////////////////////

void pre_reducer::purify_edges() {
  rep (v, V_) {
    if (status_[v] != NORMAL) {
      adj_[v].clear();
    } else {
      adj_[v].erase(remove_if(all(adj_[v]),
                              [&](int u) { return status_[u] != NORMAL; }),
                    adj_[v].end());
    }
  }
}

void pre_reducer::remove_edge(int u, int v) {
  modified_ = true;

  // TODO: slow?
  adj_[u].erase(remove(all(adj_[u]), v), adj_[u].end());
  adj_[v].erase(remove(all(adj_[v]), u), adj_[v].end());
}

void pre_reducer::fix_vertex(int v) {
  modified_ = true;

  assert(status_[v] == NORMAL);
  status_[v] = FIXED;

  for (int u : adj_[v]) {
    adj_[u].erase(remove(all(adj_[u]), v), adj_[u].end());
  }
  adj_[v].clear();
}
}  // namespace vc_solver
