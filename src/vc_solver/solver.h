#ifndef VC_SOLVER_H_
#define VC_SOLVER_H_

#include "common.h"
#include "instance.h"

namespace vc_solver {
class solver {
 public:
  solver();

  double solve(const vector<pair<int, int> > &edges,
               const vector<double> &weight, vector<int> &vc);

 private:
  static const double INF;
  instance i_;

  double best_solution_weight_;
  vector<int> best_solution_;

  // Branch-and-bound
  void dfs();
};
}

#endif  // VC_SOLVER_H_
