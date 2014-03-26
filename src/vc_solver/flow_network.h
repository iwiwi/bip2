#ifndef FLOW_NETWORK_H_
#define FLOW_NETWORK_H_

#include "common.h"

namespace vc_solver {
class extreme_min_cut;

// This class represents the 4-layer flow network
class flow_network {
  friend class extreme_min_cut;

 public:
  struct edge_t {
    int to;
    double cap;
    int rev;
    edge_t(int t, double c, int r) : to(t), cap(c), rev(r) {}
  };

  // Edge edit history
  struct eh_t {
    int v, i;
    double cap;  // adj[v][i].cap = cap
    eh_t(int v, int i, double c) : v(v), i(i), cap(c) {}
  };

  void construct(const vector<pair<int, int>> &edges,
                 const vector<double> &weight);

  // Max-flow: returns increase
  double maximize_dinic();
  double maximize_ff();  // Ford-Fulkerson
  double value() { return F; }

  // Matching (for unweighted instances)
  int matching_out(int i);

  // Operations: returns decrease
  double remove(int i, vector<eh_t> &edit_history);
  void revert(const vector<eh_t> &edit_history);

  // Debug
  void print();

 private:
  // Network

  int V, S, T;
  double F;
  vector<vector<edge_t> > adj_;

  void add_edge(int u, int v, double c) {
    adj_[u].emplace_back(v, c, adj_[v].size());
    adj_[v].emplace_back(u, 0, adj_[u].size() - 1);
  }

  edge_t &rev(const edge_t &e) { return adj_[e.to][e.rev]; }
  int lefv(int i) { return 2 + i * 2; }
  int rigv(int i) { return 2 + i * 2 + 1; }

  int layer(int v) {
    return v == S ? 0 : v == T ? 3 : v % 2 == 0 ? 1 : 2;
  }

  int ff_id_;
  vector<int> ff_vis_;
  double ff_augment(int v, double f);

  vector<int> dinic_level_, dinic_done_;
  bool dinic_levelize();
  double dinic_augment(int v, double f);

  double cancel(int v);
  double cancel_forward(int v, double f);
  double cancel_backward(int v, double f);
};
}  // namespace vc_solver

#endif
