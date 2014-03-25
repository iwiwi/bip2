#include "flow_network.h"

int flow_network::ff_augment(int v, int f) {
  if (v == T || f == 0) return f;
  ff_vis_[v] = ff_id_;
  for (e_t &e : adj[v]) {
    if (ff_vis_[e.to] == ff_id_ || e.cap == 0) continue;
    int t = ff_augment(e.to, min(f, e.cap));
    if (t > 0) {
      e.cap -= t;
      rev(e).cap += t;
      return t;
    }
  }
  return 0;
}

int flow_network::maximize_ff() {
  int f = 0, t;

  for (++ff_id_; (t = ff_augment(S, INT_MAX)) > 0; ++ff_id_) {
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
    int v = que.front(); que.pop();
    rep (i, adj[v].size()) {
      e_t &e = adj[v][i];
      if (!e.cap || dinic_level_[e.to] != -1) continue;
      dinic_level_[e.to] = dinic_level_[v] + 1;
      que.push(e.to);
    }
  }
  return dinic_level_[T] != -1;
}

int flow_network::dinic_augment(int v, int f) {
  if (v == T || f == 0) return f;
  for (; dinic_done_[v] < (int)adj[v].size(); dinic_done_[v]++) {
    e_t &e = adj[v][dinic_done_[v]];
    if (dinic_level_[e.to] <= dinic_level_[v]) continue;
    int t = dinic_augment(e.to, min(f, e.cap));
    if (t > 0) {
      e.cap -= t;
      rev(e).cap += t;
      return t;
    }
  }
  return 0;
}

int flow_network::maximize_dinic() {
  int f = 0, t;
  while (dinic_levelize()) {
    dinic_done_.assign(V, 0);
    while ((t = dinic_augment(S, INT_MAX)) > 0) {
      f += t;
      F += t;
    }
  }
  return f;
}

int flow_network::cancel_forward(int v, int f) {
  if (v == T || f == 0) return f;
  int c = 0;
  for (e_t &e : adj[v]) {
    if (get_layer(e.to) <= get_layer(v)) continue;
    int t = cancel_forward(e.to, min(f - c, rev(e).cap));
    e.cap += t;
    rev(e).cap -= t;
    c += t;
  }
  return c;
}

int flow_network::cancel_backward(int v, int f) {
  if (v == S || f == 0) return f;
  int c = 0;
  for (e_t &e : adj[v]) {
    if (get_layer(e.to) >= get_layer(v)) continue;
    int t = cancel_backward(e.to, min(f - c, e.cap));
    e.cap -= t;
    rev(e).cap += t;
    c += t;
  }
  return c;
}

int flow_network::cancel(int v) {
  int f1 = cancel_forward(v, INT_MAX);
  int f2 = cancel_backward(v, INT_MAX);
  assert(f1 == f2);
  F -= f1;
  return f1;
}

int flow_network::remove(int v, vector<eh_t> &edit_history) {
  int f = cancel(v);

  rep (i, adj[v].size()) {
    e_t &e = adj[v][i];
    e_t &r = rev(e);
    if (r.cap > 0) {
      assert(e.cap == 0);
      edit_history.emplace_back(e.to, e.rev, r.cap);
      r.cap = 0;
    }
  }
  return f;
}

void flow_network::revert(const vector<eh_t> &edit_history) {
  for (const eh_t &eh : edit_history) {
    assert(adj[eh.v][eh.i].cap == 0);
    adj[eh.v][eh.i].cap = eh.cap;
  }
}


void flow_network::print() {
  rep (v, V) {
    printf("%d:", v);
    for (auto e : adj[v]) printf(e.cap ? " %d[%d]" : " (%d)", e.to, e.cap);
    puts("");
  }
  puts("");
}
