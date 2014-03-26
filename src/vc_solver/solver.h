#ifndef VC_SOLVER_SOLVER_H_
#define VC_SOLVER_SOLVER_H_

#include "common.h"
#include "instance.h"
#include "reducer.h"
#include "bounder.h"

namespace vc_solver {
class solver {
 public:
  solver(int depth = 0) : i_(), r_(i_), b_(i_), depth_(depth) {};

  double solve(const vector<pair<int, int> > &edges,
               const vector<double> &weight, vector<int> &vc);

 private:
  instance i_;
  reducer r_;
  bounder b_;
  int depth_;

  double best_solution_weight_;
  vector<int> best_solution_;

  // Branch-and-bound
  void dfs();
  int select_branching_vertex();
  bool components();

  int n() { return i_.n(); }
  const vector<int> &adj(int v) { return i_.adj(v); }
  double weight(int v) { return i_.weight(v); }
  int value(int v) { return i_.value(v); }
  int degree(int v) { return i_.degree(v); }
};
}

#endif  // VC_SOLVER_H_
