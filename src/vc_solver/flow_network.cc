#include "flow_network.h"

namespace vc_solver {
void flow_network::construct(const vector<pair<int, int>> &edges,
                             const vector<double> &weight) {
  int n = weight.size();

  V = n * 2 + 2;
  S = 0;
  T = 1;
  F = 0;
  adj_.assign(V, vector<edge_t>());

  ff_id_ = 0;
  ff_vis_.assign(V, -1);

  rep (i, n) {
    add_edge(S, lefv(i), weight[i]);
    add_edge(rigv(i), T, weight[i]);
  }
  rep (i, edges.size()) {
    int a = edges[i].first, b = edges[i].second;
    add_edge(lefv(a), rigv(b), INF_DOUBLE);
    add_edge(lefv(b), rigv(a), INF_DOUBLE);
  }
}

double flow_network::ff_augment(int v, double f) {
  if (v == T || f < EPS_DOUBLE) return f;
  ff_vis_[v] = ff_id_;
  for (edge_t &e : adj_[v]) {
    if (ff_vis_[e.to] == ff_id_ || e.cap < EPS_DOUBLE) continue;
    double t = ff_augment(e.to, min(f, e.cap));
    if (t > EPS_DOUBLE) {
      e.cap -= t;
      rev(e).cap += t;
      return t;
    }
  }
  return 0;
}

double flow_network::maximize_ff() {
  double f = 0, t;
  for (++ff_id_; (t = ff_augment(S, INF_DOUBLE)) > EPS_DOUBLE; ++ff_id_) {
    f += t;
    F += t;
  }
  return f;
}

bool flow_network::dinic_levelize() {
  dinic_level_.assign(V, -1);
  queue<int> que;
  dinic_level_[S] = 0;
  que.push(S);
  while (!que.empty()) {
    int v = que.front();
    que.pop();
    rep (i, adj_[v].size()) {
      edge_t &e = adj_[v][i];
      if (e.cap < EPS_DOUBLE || dinic_level_[e.to] != -1) continue;
      dinic_level_[e.to] = dinic_level_[v] + 1;
      que.push(e.to);
    }
  }
  return dinic_level_[T] != -1;
}

double flow_network::dinic_augment(int v, double f) {
  if (v == T || f < EPS_DOUBLE) return f;
  for (; dinic_done_[v] < (int)adj_[v].size(); dinic_done_[v]++) {
    edge_t &e = adj_[v][dinic_done_[v]];
    if (dinic_level_[e.to] <= dinic_level_[v]) continue;
    double t = dinic_augment(e.to, min(f, e.cap));
    if (t > EPS_DOUBLE) {
      e.cap -= t;
      rev(e).cap += t;
      return t;
    }
  }
  return 0;
}

double flow_network::maximize_dinic() {
  double f = 0, t;
  while (dinic_levelize()) {
    dinic_done_.assign(V, 0);
    while ((t = dinic_augment(S, INF_DOUBLE)) > EPS_DOUBLE) {
      f += t;
      F += t;
    }
  }
  return f;
}

int flow_network::matching_out(int i) {
  for (const flow_network::edge_t &e : adj_[lefv(i)]) {
    if (e.to == S) continue;
    const edge_t &r = rev(e);
    if (r.cap > 0) {
      return (e.to - 3) / 2;
    }
  }
  assert(false);
}

double flow_network::cancel_forward(int v, double f) {
  if (v == T || f < EPS_DOUBLE) return f;
  double c = 0;
  for (edge_t &e : adj_[v]) {
    if (layer(e.to) <= layer(v)) continue;
    double t = cancel_forward(e.to, min(f - c, rev(e).cap));
    e.cap += t;
    rev(e).cap -= t;
    c += t;
  }
  return c;
}

double flow_network::cancel_backward(int v, double f) {
  if (v == S || f < EPS_DOUBLE) return f;
  double c = 0;
  for (edge_t &e : adj_[v]) {
    if (layer(e.to) >= layer(v)) continue;
    double t = cancel_backward(e.to, min(f - c, e.cap));
    e.cap -= t;
    rev(e).cap += t;
    c += t;
  }
  return c;
}

double flow_network::cancel(int v) {
  double f1 = cancel_forward(v, INF_DOUBLE);
  double f2 = cancel_backward(v, INF_DOUBLE);
  assert(fabs(f1 - f2) < EPS_DOUBLE);
  F -= f1;
  return f1;
}

double flow_network::remove(int k, vector<eh_t> &edit_history) {
  double f = 0.0;
  for (int v : {lefv(k), rigv(k)}) {
    f += cancel(v);
    rep (i, adj_[v].size()) {
      edge_t &e = adj_[v][i];
      edge_t &r = rev(e);
      if (r.cap > EPS_DOUBLE) {
        assert(e.cap < EPS_DOUBLE);
        edit_history.emplace_back(e.to, e.rev, r.cap);
        r.cap = 0;
      }
    }
  }
  return f;
}

void flow_network::revert(const vector<eh_t> &edit_history) {
  for (const eh_t &eh : edit_history) {
    assert(adj_[eh.v][eh.i].cap < EPS_DOUBLE);
    adj_[eh.v][eh.i].cap = eh.cap;
  }
}


void flow_network::print() {
  rep (v, V) {
    printf("%d:", v);
    for (auto e : adj_[v]) printf(e.cap ? " %d[%f]" : " (%d)", e.to, e.cap);
    puts("");
  }
  puts("");
}
}  // namespace vc_solver
