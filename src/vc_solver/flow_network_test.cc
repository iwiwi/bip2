#include <gtest/gtest.h>
#include "flow_network.h"
using namespace vc_solver;

namespace {
inline void ASSERT_EQ_DOUBLE(double a, double b) {
  ASSERT_TRUE(fabs(b - a) < 1E-9);
}
}  // namespace

TEST(flow_network, maximize) {
  const int max_v = 30, max_e = 100, num_instances = 100;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_vs = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_non_integer(num_vs, w);

    flow_network fn1, fn2;
    fn1.construct(vector<pair<int, int>>(e.begin(), e.end()), w);
    fn2.construct(vector<pair<int, int>>(e.begin(), e.end()), w);
    double f1 = fn1.maximize_ff();
    double f2 = fn2.maximize_dinic();
    ASSERT_EQ_DOUBLE(f1, f2);
    ASSERT_EQ_DOUBLE(f1, fn1.flow());
    ASSERT_EQ_DOUBLE(f2, fn2.flow());
  }
}

TEST(flow_network, remove) {
  const int max_v = 30, max_e = 100, num_instances = 100;

  rep (t, num_instances) {
    vector<pair<int, int>> e;
    int num_vs = generate_random_graph(max_v, max_e, e);
    vector<double> w;
    generate_random_weight_non_integer(num_vs, w);

    flow_network fn1;
    fn1.construct(e, w);
    double f0 = fn1.maximize_dinic(), f = f0;
    ASSERT_EQ_DOUBLE(f0, fn1.flow());

    rep (v, num_vs) {
      vector<flow_network::eh_t> eh;
      double d1 = fn1.remove(v, eh);
      f -= d1;
      ASSERT_EQ_DOUBLE(f, fn1.flow());
      double d2 = fn1.maximize_ff();
      f += d2;
      ASSERT_EQ_DOUBLE(f, fn1.flow());

      // Construct a network without |v| from scratch
      {
        vector<flow_network::eh_t> eh2;
        flow_network fn2;
        fn2.construct(e, w);
        fn2.remove(v, eh2);
        fn2.maximize_dinic();
        ASSERT_EQ_DOUBLE(fn1.flow(), fn2.flow());
      }

      fn1.revert(eh);
    }
  }
}
