#include <gtest/gtest.h>
#include "common.h"
#include "instance.h"
using namespace vc_solver;

namespace {
void check_degree(instance &i) {
  rep (u, i.n()) {
    if (i.value(u) != 1) continue;
    int d = 0;
    for (int x : i.adj(u)) {
      ASSERT_NE(i.value(x), 0);
      if (i.value(x) == 1) ++d;
    }
    ASSERT_EQ(d, i.degree(u));
  }
}
}  // namespace

TEST(instance, degree_initial) {
  const int kNumTests = 100, kMaxVertices = 1000, kMaxEdges = 10000;
  instance i;

  rep (t, kNumTests) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(kMaxVertices, kMaxEdges, es);
    vector<double> ws;
    generate_random_weight_non_integer(num_vs, ws);

    i.init(es, ws);
    check_degree(i);
  }
}

TEST(instance, degree_fix) {
  const int kNumTests = 100, kMaxVertices = 1000, kMaxEdges = 10000;
  instance i;

  rep (t, kNumTests) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(kMaxVertices, kMaxEdges, es);
    vector<double> ws;
    generate_random_weight_non_integer(num_vs, ws);

    i.init(es, ws);
    check_degree(i);

    vector<int> ord(num_vs);
    iota(all(ord), 0);
    random_shuffle(all(ord));

    for (int v : ord) {
      if (i.value(v) != 1) continue;
      i.fix(v, (rand() >> 10) & 2);
      check_degree(i);
    }
  }
}

TEST(instance, degree_revert) {
  const int kNumTests = 100, kMaxVertices = 1000, kMaxEdges = 10000;
  instance i;

  rep (t, kNumTests) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(kMaxVertices, kMaxEdges, es);
    vector<double> ws;
    generate_random_weight_non_integer(num_vs, ws);

    i.init(es, ws);
    check_degree(i);

    vector<int> ord(num_vs);
    iota(all(ord), 0);
    random_shuffle(all(ord));
    vector<int> revs;

    for (int v : ord) {
      revs.push_back(i.revision());
      if (i.value(v) != 1) continue;
      i.fix(v, (rand() >> 10) & 2);
      check_degree(i);
    }
    reverse(all(revs));
    for (int r : revs) {
      i.revert(r);
      check_degree(i);
    }
  }
}
