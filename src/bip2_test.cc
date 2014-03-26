#include <gtest/gtest.h>
#include "common.h"
#include "bip2.h"

namespace {
void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9) << a << " vs " << b;
}

void test(const vector<pair<int, int> > &edges,
          const vector<double> &weight) {
  double ans = solve_vc_naive(edges, weight);

  vector<int> vc;
  double res = solve_vc(edges, weight, vc);
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

TEST(solve_vc, random_unweighted) {
  const int max_v = 16, max_e = 100, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w(num_v, 1);
    test(e, w);
  }
}

TEST(solve_vc, random_weighted_integer) {
  const int max_v = 16, max_e = 100, max_w = 5, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_integer(num_v, max_w, w);
    test(e, w);
  }
}

TEST(solve_vc, random_weighted_non_integer) {
  const int max_v = 16, max_e = 100, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_non_integer(num_v, w);
    test(e, w);
  }
}
