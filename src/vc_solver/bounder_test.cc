#include <gtest/gtest.h>
#include "common.h"
#include "bounder.h"
using namespace vc_solver;
using testing::Types;

namespace {
void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9);
}

struct bounder_controller_flow {
  static double lower_bound(bounder &b) {
    return b.lower_bound_flow();
  }
};

struct bounder_controller_all {
  static double lower_bound(bounder &b) {
    return b.lower_bound();
  }
};
}  // namespace

typedef Types<
    bounder_controller_flow,
    bounder_controller_all
> Testers;

template<typename T>
class bounder_test : public testing::Test {};
TYPED_TEST_CASE(bounder_test, Testers);

TYPED_TEST(bounder_test, unweighted) {
  const int max_v = 16, max_e = 100, num_instances = 1000;
  instance i;
  bounder b(i);
  rep (t, num_instances) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(max_v, max_e, es);
    vector<double> ws(num_vs, 1);
    double ans = solve_vc_naive(es, ws);

    i.init(es, ws);
    ASSERT_GT(ans + EPS_DOUBLE, TypeParam::lower_bound(b));
  }
}

TYPED_TEST(bounder_test, weighted_integer) {
  const int max_v = 16, max_e = 100, max_w = 5, num_instances = 1000;
  instance i;
  bounder b(i);
  rep (t, num_instances) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(max_v, max_e, es);
    vector<double> ws;
    generate_random_weight_integer(num_vs, max_w, ws);
    double ans = solve_vc_naive(es, ws);

    i.init(es, ws);
    ASSERT_GT(ans + EPS_DOUBLE, TypeParam::lower_bound(b));
  }
}

TYPED_TEST(bounder_test, weighted_non_integer) {
  const int max_v = 16, max_e = 100, num_instances = 1000;
  instance i;
  bounder b(i);
  rep (t, num_instances) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(max_v, max_e, es);
    vector<double> ws;
    generate_random_weight_non_integer(num_vs, ws);
    double ans = solve_vc_naive(es, ws);

    i.init(es, ws);
    ASSERT_GT(ans + EPS_DOUBLE, TypeParam::lower_bound(b));
  }
}
