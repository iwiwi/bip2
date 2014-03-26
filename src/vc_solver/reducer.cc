#include "reducer.h"

namespace vc_solver {
void reducer::reduce() {
  do {
    modified_ = false;
    reduce_degree1();
    reduce_satellite();
    reduce_unconfined();
    reduce_emc();
  } while (modified_);
}

void reducer::reduce_without_flow() {
  do {
    modified_ = false;
    reduce_degree1();
    reduce_satellite();
    reduce_unconfined();
  } while (modified_);
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
      modified_ = true;
      for (int x : adj(u)) {
        if (value(x) == 1 && degree(x) == 1) {
          que.push(x);
        }
      }
    }
  }

//  rep (v, n()) if (value(v) == 1) assert(degree(v) > 1);
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

void reducer::reduce_unconfined() {
  if (!i_.is_unweighted()) return;

  vector<int> in_s(n(), -1), in_ns(n(), -1), ns_es(n(), 0);
  rep (v, n()) {
    if (value(v) != 1) continue;

    vector<int> cq, nq;  // Queues
    in_s[v] = v;
    for (int u : adj(v)) {
      in_ns[u] = v;
      ns_es[u] = 1;
      cq.push_back(u);
    }

    bool update;
    do {
      update = false;

      for (int u : cq) {
        if (value(u) != 1) continue;
        if (in_s[u] == v || ns_es[u] > 1) continue;
        assert(ns_es[u] == 1);
        nq.push_back(u);

        int o = 0, x = -1;
        for (int w : adj(u)) {
          if (value(w) != 1) continue;
          if (in_s[w] == v) continue;
          if (in_ns[w] != v) {
            if (++o >= 2) break;
            x = w;
          }
        }

        if (o == 0) {
          //          printf("%d:", v); for (int w : adj_[v]) printf(" %d(%d)", w, get_current_value(w)); puts("");
          //          printf("%d:", u); for (int w : adj_[u]) printf(" %d(%d)", w, get_current_value(w)); puts("");
          //          puts("UNCONFINED");
          i_.fix(v, 2);
          modified_ = true;
          update = false;
          break;
        } else if (o == 1) {
          update = true;
          in_s[x] = v;
          for (int z : adj(x)) {
            if (in_ns[z] == v) {
              ++ns_es[z];
            } else {
              in_ns[z] = v;
              ns_es[z] = 1;
              nq.push_back(z);
            }
          }
        }
      }
      cq.swap(nq);
      nq.clear();
    } while (update);
  }
}
}  // namespace vc_solver
