#include "instance.h"

namespace vc_solver {
void instance::init(const vector<pair<int, int>> &original_edges,
                    const vector<double>& original_weight) {
  original_n_ = original_weight.size();
  unweighted_ = (count(all(original_weight), 1) == original_n_);
  n_ = original_weight.size();

  // Purify edges
  vector<int> self_loop_vs;
  vector<pair<int, int>> purified_es;
  for (const auto &e : original_edges) {
    if (e.first == e.second) self_loop_vs.push_back(e.first);
    else purified_es.emplace_back(min(e.first, e.second),
                                  max(e.first, e.second));
  }
  sort(all(purified_es));
  purified_es.erase(unique(all(purified_es)), purified_es.end());

  // Degree
  vector<int> original_to_deg(n_, 0);
  for (const auto &e : purified_es) {
    ++original_to_deg[e.first];
    ++original_to_deg[e.second];
  }

  // Sort vertices by degree
  vector<int> original_to_new(n_);
  iota(all(original_to_new), 0);
  sort(all(original_to_new), [&](int u, int v) { return original_to_deg[u] > original_to_deg[v]; });
  vector<int> new_to_original(n_);
  rep (ov, n_) new_to_original[original_to_new[ov]] = ov;

  // Construct graph
  adj_.assign(n_, vector<int>());
  for (const auto &e : purified_es) {
    adj_[original_to_new[e.first ]].push_back(original_to_new[e.second]);
    adj_[original_to_new[e.second]].push_back(original_to_new[e.first ]);
  }

  weight_.resize(n_);
  rep (nv, n_) weight_[nv] = original_weight[new_to_original[nv]];

  original_vs_.assign(n_, vector<int>(1));
  rep (nv, n_) original_vs_[nv][0] = new_to_original[nv];

  deg_.resize(n_);
  rep (nv, n_) deg_[nv] = adj_[nv].size();

  // Process self-loops
  for (int ov : self_loop_vs) {
    fix(original_to_new[ov], 2);
  }

  // Prepare
  fixed_count_.assign(n_, 0);
  fixed_value_.resize(n_);
  solution_weight_ = 0.0;
  graph_edit_history_.clear();
  {
    vector<pair<int, int>> new_edges(purified_es.size());
    rep (i, purified_es.size()) {
      new_edges[i].first  = original_to_new[purified_es[i].first ];
      new_edges[i].second = original_to_new[purified_es[i].second];
    }
    flow_.construct(new_edges, weight_);
    flow_edit_history_.assign(n_, vector<flow_network::eh_t>());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Solution
///////////////////////////////////////////////////////////////////////////////

double instance::solution(vector<int>& vc) {
  vc.clear();
  double w = 0;
  rep (v, n_) {
    assert(value(v) != 1);
    if (value(v) == 2) {
      vc.insert(vc.end(), original_vs_[v].begin(), original_vs_[v].end());
      w += weight_[v];
    }
  }
  return w;
}

void instance::revert(int revision) {
  while ((int)graph_edit_history_.size() > revision) {
    pair<int, int> e = graph_edit_history_.back();
    graph_edit_history_.pop_back();

    if (e.second == -1) {
      // Vertex
      int i = e.first;
      assert(fixed_count_[i] == 1);
      fixed_count_[i] = 0;

      if (fixed_value_[i] == 0) {
        // Yes, we ignore here
      } else if (fixed_value_[i] == 2) {
        flow_.revert(flow_edit_history_[i]);
        flow_edit_history_[i].clear();
        solution_weight_ -= weight_[i];
        for (int u : adj_[i]) ++deg_[u];
      } else {
        assert(false);
      }
    } else {
      // Edge
      assert(false);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Operations
///////////////////////////////////////////////////////////////////////////////

int instance::fix2(int i) {
  int r = graph_edit_history_.size();
  if (fixed_count_[i] == 0) {
    fixed_count_[i] = 1;
    fixed_value_[i] = 2;
    graph_edit_history_.emplace_back(i, -1);


    assert(flow_edit_history_[i].empty());
    flow_.remove(i, flow_edit_history_[i]);
    solution_weight_ += weight_[i];
    for (int u : adj_[i]) --deg_[u];
  } else {
    assert(fixed_value_[i] == 2);
  }
  return r;
}

int instance::fix0(int i) {
  int r = graph_edit_history_.size();
  if (fixed_count_[i] == 0) {
    fixed_count_[i] = 1;
    fixed_value_[i] = 0;
    graph_edit_history_.emplace_back(i, -1);
    for (int j : adj_[i]) fix2(j);
  } else {
    assert(fixed_value_[i] == 0);
  }
  return r;
}

int instance::fix(int i, int value) {
  if (value == 0) return fix0(i);
  else if (value == 2) return fix2(i);
  else assert(false);
}
}  // namespace vc_solver
