#include <gtest/gtest.h>
#include "common.h"
#include "solver.h"

namespace {
void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9);
}

void test(const vector<pair<int, int> > &edges,
          const vector<double> &weight) {
  double ans = solve_vc_naive(edges, weight);

  vector<int> vc;
  vc_solver::solver s;
  double res = s.solve(edges, weight, vc);
  ASSERT_EQ_DOUBLE(ans, res);

  double sum = 0;
  for (int v : vc) sum += weight[v];
  ASSERT_EQ_DOUBLE(ans, sum);

  is_vc(weight.size(), edges, vc);
}

void test_unweighted(const vector<pair<int, int> > &edges,
                     int num_v) {
  test(edges, vector<double>(num_v, 1));
}
}  // namespace

TEST(solver, sample1) {
  vector<pair<int, int> > E {
    {1, 2}, {2, 3}, {3, 1}, {3, 4}, {3, 5}
  };

  test_unweighted(E, 6);
}

TEST(solver, sample0) {
  vector<pair<int, int> > E {
    {1, 2}, {1, 3}, {2, 3}
  };

  test_unweighted(E, 4);
}

TEST(solver, sample2) {
  vector<pair<int, int> > E {
    {1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 5}, {4, 5}, {4, 6}, {4, 7}, {5, 6}, {5, 7}
  };

  test_unweighted(E, 8);
}

TEST(solver, random_unweighted) {
  const int max_v = 16, max_e = 100, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w(num_v, 1);
    test(e, w);
  }
}

TEST(solver, random_weighted_integer) {
  const int max_v = 16, max_e = 100, max_w = 5, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_integer(num_v, max_w, w);
    test(e, w);
  }
}

TEST(solver, random_weighted_non_integer) {
  const int max_v = 16, max_e = 100, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_non_integer(num_v, w);
    test(e, w);
  }
}
