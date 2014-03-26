#include "bounder.h"

namespace vc_solver {
double bounder::lower_bound() {
  return max(lower_bound_flow(), lower_bound_clique());
}

double bounder::lower_bound_flow() {
  return i_.solution_weight() + i_.flow().value() / 2.0;
}

double bounder::lower_bound_clique() {
  vector<int> clique(n(), -1), root, sz, out, tmp;
  vector<double> sum_w, max_w;

  for (int v = n() - 1; v >= 0; --v) {
    if (value(v) != 1) continue;

    for (int u : adj(v)) if (clique[u] != -1) tmp[clique[u]] = 0;

    // Find the clique to add |v|
    int max_c = root.size(), max_sz = 0, min_out = 0;
    for (int u : adj(v)) {
      int c = clique[u];
      if (c == -1) continue;
      if (++tmp[c] == sz[c]) {
        // |v| can be added to the clique |c|
        if (max_sz < sz[c] || (max_sz == sz[c] && min_out > out[c])) {
          max_c = c;
          max_sz = sz[c];
          min_out = out[c];
        }
      }
      if (u == root[c]) --out[c];
    }

    clique[v] = max_c;
    if (max_c == (int)root.size()) {
      root.push_back(v);
      sz.push_back(1);
      {
        int o = 0;
        for (int u : adj(v)) {
          if (value(u) == 1 && clique[u] == -1) ++o;
        }
        out.push_back(o);
      }
      sum_w.push_back(weight(v));
      max_w.push_back(weight(v));
      tmp.push_back(0);
    }
    else {
      ++sz[max_c];
      sum_w[max_c] += weight(v);
      max_w[max_c] = max(max_w[max_c], weight(v));
    }
  }

  double res = i_.solution_weight();
  rep (c, sum_w.size()) res += sum_w[c] - max_w[c];
  return res;
}
}  // namespace vc_solver
