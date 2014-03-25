#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <cassert>
#include <climits>
#include <google/gflags.h>
#include "jlog.h"
using namespace std;

#define all(c) (c).begin(), (c).end()
#define iter(c) __typeof((c).begin())
#define cpresent(c, e) (find(all(c), (e)) != (c).end())
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define tr(c, i) for (iter(c) i = (c).begin(); i != (c).end(); ++i)
#define pb(e) push_back(e)
#define mp(a, b) make_pair(a, b)

#define CHECK(expr)                                                     \
  if (expr) {                                                           \
  } else {                                                              \
    fprintf(stderr, "CHECK Failed (%s:%d): %s\n",                       \
            __FILE__, __LINE__, #expr);                                 \
    exit(EXIT_FAILURE);                                                 \
  }

///////////////////////////////////////////////////////////////////////////////
// Load
///////////////////////////////////////////////////////////////////////////////

int load_graph(FILE *fp, vector<pair<int, int> > &es);
int load_graph(const char *file, vector<pair<int, int> > &es);

///////////////////////////////////////////////////////////////////////////////
// Utility
///////////////////////////////////////////////////////////////////////////////

void compute_connected_components(int n,
                                  const vector<pair<int, int>> &edges,
                                  vector<vector<int>> &cc_vertices,
                                  vector<vector<pair<int, int>>> &cc_edges);

///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

double solve_vc_naive(const vector<pair<int, int> > &edges,
                   const vector<double> &weight,
                   vector<int> &vc);
double solve_vc_naive(const vector<pair<int, int> > &edges,
                   const vector<double> &weight);

int generate_random_graph(int max_v, int max_e, vector<pair<int, int>> &edges);

void generate_random_weight_integer(int num_v, int max_w, vector<double> &weight);
void generate_random_weight_non_integer(int num_v, vector<double> &weight);

bool is_vc(int num_vs, const vector<pair<int, int> > &edges,
           const vector<int> &vc);

///////////////////////////////////////////////////////////////////////////////
// Macro for statistics
///////////////////////////////////////////////////////////////////////////////

double get_current_time_sec();

class stat_timer {
 public:
  stat_timer(double &timer)
      : timer_(&timer) {
    *timer_ -= get_current_time_sec();
  }

  ~stat_timer(){
    *timer_ += get_current_time_sec();
  }

  operator bool() {
    return false;
  }

 private:
  double *timer_;
};

#define STAT_TIMER(timer)                            \
  if (stat_timer t__ = stat_timer(timer)); else

#endif
