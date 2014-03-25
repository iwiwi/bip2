#include "solver.h"

namespace vc_solver {
const double solver::INF = 1E60;

solver::solver() {
}

double solver::solve(const vector<pair<int, int> >& edges,
                     const vector<double>& weight, vector<int>& vc) {
  i_.init(edges, weight);

  best_solution_.clear();
  best_solution_weight_ = INF;

  dfs();

  vc = best_solution_;
  return best_solution_weight_;
}

void solver::dfs() {
  if (i_.solution_weight() >= best_solution_weight_) return;

  rep (v, i_.n()) {
    if (i_.value(v) == 1) {
      int r = i_.revision();

      i_.fix(v, 0);
      dfs();
      i_.revert(r);

      i_.fix(v, 2);
      dfs();
      i_.revert(r);

      return;
    }
  }

  best_solution_weight_ = i_.solution(best_solution_);
}

}  // namespace vc_solver
