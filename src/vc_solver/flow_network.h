#ifndef FLOW_NETWORK_H_
#define FLOW_NETWORK_H_

#include "common.h"

// This class represents the 4-layer flow network
class flow_network {
 public:
  struct e_t {
    int to, cap, rev;
    e_t(int t, int c, int r) : to(t), cap(c), rev(r) {}
  };

  // Edge edit history
  struct eh_t {
    int v, i, cap;  // adj[v][i].cap = cap
    eh_t(int v, int i, int c) : v(v), i(i), cap(c) {}
  };

  int V, S, T, F;
  vector<vector<e_t> > adj;

  flow_network(int v = 0, int s = 0, int t = 0) :
    V(v), S(s), T(t), F(0), adj(V), ff_id_(0), ff_vis_(v, -1) {}

  void init(int v, int s, int t) {
    V = v;
    S = s;
    T = t;
    F = 0;
    adj.assign(V, vector<e_t>());

    ff_id_ = 0;
    ff_vis_.assign(v, -1);
  }

  void add_edge(int u, int v, int c) {
    adj[u].emplace_back(v, c, adj[v].size());
    adj[v].emplace_back(u, 0, adj[u].size() - 1);
  }

  e_t &rev(const e_t &e) {
    return adj[e.to][e.rev];
  }

  // Returns increase
  int maximize_dinic();
  int maximize_ff();  // Ford-Fulkerson

  // Returns decrease
  int cancel(int v);
  int remove(int v, vector<eh_t> &edit_history);

  void revert(const vector<eh_t> &edit_history);

  // Debug
  void print();

 private:

  int get_layer(int v) {
    return v == S ? 0 : v == T ? 3 : v % 2 == 0 ? 1 : 2;
  }

  int ff_id_;
  vector<int> ff_vis_;
  int ff_augment(int v, int f);

  vector<int> dinic_level_, dinic_done_;
  bool dinic_levelize();
  int dinic_augment(int v, int f);

  int cancel_forward(int v, int f);
  int cancel_backward(int v, int f);
};

#endif
