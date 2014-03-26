#include <gtest/gtest.h>
#include "common.h"
#include "reducer.h"
using namespace vc_solver;
using testing::Types;

namespace {
void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9);
}

struct reducer_controller_degree1 {
  static void reduce(reducer &r) {
    r.reduce_degree1();
  }
};

struct reducer_controller_emc {
  static void reduce(reducer &r) {
    r.reduce_emc();
  }
};

struct reducer_controller_all {
  static void reduce(reducer &r) {
    r.reduce();
  }
};
}  // namespace

typedef Types<
    reducer_controller_degree1,
    reducer_controller_emc,
    reducer_controller_all
> Testers;

template<typename T>
class reducer_test : public testing::Test {};
TYPED_TEST_CASE(reducer_test, Testers);

TYPED_TEST(reducer_test, reduction_unweighted) {
  const int max_v = 16, max_e = 100, num_instances = 1000;

  instance i;
  reducer r(i);

  rep (t, num_instances) {
    vector<pair<int, int>> es;
    int num_vs = generate_random_graph(max_v, max_e, es);
    vector<double> ws(num_vs, 1);
    double ans = solve_vc_naive(es, ws);

    i.init(es, ws);
    TypeParam::reduce(r);
    vector<int> vc;
    ASSERT_EQ_DOUBLE(ans, solve_vc_naive(i, vc));

    is_vc(num_vs, es, vc);
    {
      int sum = 0;
      for (int v : vc) sum += ws[v];
      ASSERT_EQ_DOUBLE(ans, sum);
    }
  }
}
