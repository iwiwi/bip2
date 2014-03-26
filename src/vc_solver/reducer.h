#ifndef VC_SOLVER_REDUCER_H_
#define VC_SOLVER_REDUCER_H_

#include "instance.h"
#include "extreme_min_cut.h"

namespace vc_solver {
class reducer {
 public:
  reducer(instance &i) : i_(i), emc_(i.flow()) {}

  void reduce();
  void reduce_without_flow();

  void reduce_degree1();
  void reduce_emc();

 private:
  instance &i_;
  extreme_min_cut emc_;

  int n() { return i_.n(); }
  const vector<int> &adj(int v) { return i_.adj(v); }
  double weight(int v) { return i_.weight(v); }
  int value(int v) { return i_.value(v); }
  int degree(int v) { return i_.degree(v); }
};
}  // namespace vc_solver

#endif  // VC_SOLVER_REDUCER_H_
