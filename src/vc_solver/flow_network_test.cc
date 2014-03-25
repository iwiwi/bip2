#include <gtest/gtest.h>
#include "flow_network.h"

namespace {
void generate_random_instance(int max_v, int max_e, int max_w,
                              vector<pair<int, int>> &edges, vector<int> &weight) {
  int num_v = 1 + rand() % max_v;
  int num_e = min(1 + rand() % max_e, num_v * (num_v - 1) / 2);

  set<pair<int, int>> e;
  while ((int)e.size() < num_e) {
    int u = rand() % num_v, v = rand() % num_v;
    if (u == v) continue;
    if (u > v) swap(u, v);
    e.insert({u, v});
  }
  edges.assign(e.begin(), e.end());

  weight.resize(num_v);
  rep (i, num_v) weight[i] = 1 + rand() % max_w;
}

void construct_network(const vector<pair<int, int>> &edges,
                          const vector<int> &weight,
                          flow_network &f) {
  int n = weight.size();
  f.init(n * 2 + 2, n * 2, n * 2 + 1);
  rep (i, n) {
    f.add_edge(f.S, i * 2, weight[i]);
    f.add_edge(i * 2 + 1, f.T, weight[i]);
  }
  rep (i, edges.size()) {
    int a = edges[i].first, b = edges[i].second;
    f.add_edge(a * 2, b * 2 + 1, INT_MAX);
    f.add_edge(b * 2, a * 2 + 1, INT_MAX);
  }
}
}  // namespace

TEST(flow_network, maximize) {
  const int max_v = 30, max_e = 100, max_w = 100, num_instances = 100;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    vector<int> w;
    generate_random_instance(max_v, max_e, max_w, e, w);

    flow_network fn1, fn2;

    construct_network(vector<pair<int, int>>(e.begin(), e.end()), w, fn1);
    construct_network(vector<pair<int, int>>(e.begin(), e.end()), w, fn2);
    int f1 = fn1.maximize_ff();
    int f2 = fn2.maximize_dinic();
    EXPECT_EQ(f1, f2);
    EXPECT_EQ(f1, fn1.F);
    EXPECT_EQ(f2, fn2.F);
  }
}

TEST(flow_network, cancel) {
  const int max_v = 30, max_e = 100, max_w = 100, num_instances = 100;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    vector<int> w;
    generate_random_instance(max_v, max_e, max_w, e, w);
    int n = w.size();

    flow_network fn;
    construct_network(e, w, fn);
    int f0 = fn.maximize_dinic();
    EXPECT_EQ(f0, fn.F);

    rep (v, n * 2) {
      int d1 = fn.cancel(v);
      int f1 = f0 - d1;
      EXPECT_EQ(f1, fn.F);

      int d2 = fn.maximize_ff();
      int f2 = f1 + d2;
      EXPECT_EQ(f2, fn.F);
      EXPECT_EQ(f0, f2);
    }
  }
}

TEST(flow_network, remove) {
  const int max_v = 30, max_e = 100, max_w = 100, num_instances = 100;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    vector<int> w;
    generate_random_instance(max_v, max_e, max_w, e, w);
    int n = w.size();

    flow_network fn1;
    construct_network(e, w, fn1);
    int f0 = fn1.maximize_dinic(), f = f0;
    ASSERT_EQ(f0, fn1.F);

    rep (v, n * 2) {
      vector<flow_network::eh_t> eh;
      int d1 = fn1.remove(v, eh);
      f -= d1;
      ASSERT_EQ(f, fn1.F);
      int d2 = fn1.maximize_ff();
      f += d2;
      ASSERT_EQ(f, fn1.F);

      // Construct a network without |v| from scratch
      flow_network fn2;
      construct_network(e, w, fn2);
      rep (u, fn2.V) {
         for (flow_network::e_t &t : fn2.adj[u]) {
           if (t.to == v) t.cap = 0;
         }
      }
      fn2.maximize_dinic();
      ASSERT_EQ(fn1.F, fn2.F);

      fn1.revert(eh);
    }
  }
}
