#ifndef VC_SOLVER_H_
#define VC_SOLVER_H_

#include "common.h"
#include "instance.h"
#include "reducer.h"

namespace vc_solver {
class solver {
 public:
  solver() : i_(), r_(i_) {};

  double solve(const vector<pair<int, int> > &edges,
               const vector<double> &weight, vector<int> &vc);

 private:
  instance i_;
  reducer r_;

  double best_solution_weight_;
  vector<int> best_solution_;

  // Branch-and-bound
  void dfs();
  int select_branching_vertex();
};
}

#endif  // VC_SOLVER_H_
