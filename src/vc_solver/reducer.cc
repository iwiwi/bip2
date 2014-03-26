#include "reducer.h"

namespace vc_solver {
void reducer::reduce() {
  reduce_degree1();
  reduce_emc();
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

  rep (v, n()) if (value(v) == 1) assert(degree(v) > 1);
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
}  // namespace vc_solver
