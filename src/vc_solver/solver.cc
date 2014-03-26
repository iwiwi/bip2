#include "solver.h"

namespace vc_solver {
double solver::solve(const vector<pair<int, int> >& edges,
                     const vector<double>& weight, vector<int>& vc) {
  i_.init(edges, weight);

  best_solution_.clear();
  best_solution_weight_ = INF_DOUBLE;

  i_.flow().maximize_dinic();
  double lp = i_.flow().value() / 2.0;

  r_.reduce();
  dfs();

  vc = best_solution_;

  return best_solution_weight_;
}

void solver::dfs() {
  int rev = i_.revision();
  r_.reduce();
  i_.flow().maximize_ff();

  double lp = i_.solution_weight() + i_.flow().value() / 2.0;
  if (lp >= best_solution_weight_) return;

  int v = select_branching_vertex();
  if (v == -1) {
    best_solution_weight_ = i_.solution(best_solution_);
  }
  else {
    for (int x : {0, 2}) {
      int rev2 = i_.fix(v, x);
      dfs();
      i_.revert(rev2);
    }
  }
  i_.revert(rev);
}

int solver::select_branching_vertex() {
  int best_v = -1, best_d = -1;
  rep (v, i_.n()) {
    if (i_.value(v) == 1 && i_.degree(v) > best_d) {
      best_v = v;
      best_d = i_.degree(v);
    }
  }
  return best_v;
}
}  // namespace vc_solver
