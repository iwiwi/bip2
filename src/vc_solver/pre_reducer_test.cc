#include <gtest/gtest.h>
#include "common.h"
#include "pre_reducer.h"

using testing::Types;
using namespace vc_solver;

namespace {
void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9) << a << " vs " << b;
}

struct reducer_controller_none {
  static void reduce(pre_reducer &r) {}
};

struct reducer_controller_domination {
  static void reduce(pre_reducer &r) {
    r.reduce_domination();
  }
};

struct reducer_controller_low_degree {
  static void reduce(pre_reducer &r) {
    r.reduce_low_degree();
  }
};

struct reducer_controller_all {
  static void reduce(pre_reducer &r) {
    r.reduce();
  }
};
}  // namespace

typedef Types<
    reducer_controller_none,
    reducer_controller_domination,
    reducer_controller_low_degree,
    reducer_controller_all
> Testers;

template<typename T>
class pre_reducer_tester : public testing::Test {};
TYPED_TEST_CASE(pre_reducer_tester, Testers);

TYPED_TEST(pre_reducer_tester, unweighted) {
  const int max_v = 15, max_e = 60, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w(num_v, 1);
    double ans = solve_vc_naive(e, w);

    pre_reducer reducer(e, w);
    TypeParam::reduce(reducer);

    vector<pair<int, int>> reduced_e;
    vector<double> reduced_w;
    reducer.get_reduced_problem(reduced_e, reduced_w);

    vector<int> reduced_vc;
    solve_vc_naive(reduced_e, reduced_w, reduced_vc);

    vector<int> vc2;
    ASSERT_EQ_DOUBLE(ans, reducer.get_original_solution(reduced_vc, vc2));

    {
      double sum = 0;
      for (int v : vc2) sum += w[v];
      ASSERT_EQ(ans, sum);
    }
  }
}

TYPED_TEST(pre_reducer_tester, weighted) {
  const int max_v = 15, max_e = 60, num_instances = 1000;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_v = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_non_integer(num_v, w);
    double ans = solve_vc_naive(e, w);

    pre_reducer reducer(e, w);
    TypeParam::reduce(reducer);

    vector<pair<int, int>> reduced_e;
    vector<double> reduced_w;
    reducer.get_reduced_problem(reduced_e, reduced_w);

    vector<int> reduced_vc;
    solve_vc_naive(reduced_e, reduced_w, reduced_vc);

    vector<int> vc2;
    ASSERT_EQ_DOUBLE(ans, reducer.get_original_solution(reduced_vc, vc2));

    {
      double sum = 0;
      for (int v : vc2) sum += w[v];
      ASSERT_EQ_DOUBLE(ans, sum);
    }
  }
}
