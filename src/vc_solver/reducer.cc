#include "reducer.h"

namespace vc_solver {
void reducer::reduce() {
  reduce_degree1();
  reduce_satellite();
  reduce_emc();
}

void reducer::reduce_without_flow() {
  reduce_degree1();
  reduce_satellite();
}

void reducer::reduce_emc() {
  i_.flow().maximize_ff();  // TODO: remove?
  emc_.compute();
  rep (v, n()) {
    if (value(v) == 1 && emc_.value(v) != 1) {
      i_.fix(v, emc_.value(v));
    }
  }
}

void reducer::reduce_degree1() {
  if (!i_.is_unweighted()) return;  // TODO

  queue<int> que;
  rep (v, n()) {
    if (value(v) == 1 && degree(v) <= 1) {
      que.push(v);
    }
  }

  while (!que.empty()) {
    int v = que.front();
    que.pop();
    if (value(v) != 1) continue;
    assert(degree(v) <= 1);

    if (degree(v) == 0) {
      i_.fix(v, 0);
    }
    else {
      int u = -1;
      for (int x : adj(v)) if (value(x) == 1) u = x;
      assert(u != -1);

      i_.fix(u, 2);
      i_.fix(v, 0);
      for (int x : adj(u)) {
        if (value(x) == 1 && degree(x) == 1) {
          que.push(x);
        }
      }
    }
  }

  rep (v, n()) if (value(v) == 1) {
    assert(degree(v) > 1);
  }
}

void reducer::reduce_satellite() {
  if (!i_.is_unweighted()) return;

  vector<int> covered(n(), -1);
  rep (v, n()) {
    if (value(v) != 1) continue;

    covered[v] = v;
    for (int u : adj(v)) covered[u] = v;

    for (int u : adj(v)) {
      if (value(u) != 1) continue;
      int out = 0, s = -1;
      for (int w : adj(u)) {
        if (value(w) == 1 && covered[w] != v) {
          ++out;
          s = w;
        }
      }
      if (out == 0) {  // |u| is dominated by |v|
        i_.fix(v, 2);
        modified_ = true;
        break;
      } else if (out == 1) {  // |v|-|u|-|s| is a satellite
        for (int z : adj(s)) covered[z] = v;
      }
    }
  }
}
}  // namespace vc_solver
