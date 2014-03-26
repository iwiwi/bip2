#include "instance.h"

namespace vc_solver {
class bounder {
 public:
  bounder(instance &i) : i_(i) {}

  double lower_bound();
  double lower_bound_flow();
  double lower_bound_clique();

 private:
  instance &i_;

  int n() { return i_.n(); }
  const vector<int> &adj(int v) { return i_.adj(v); }
  double weight(int v) { return i_.weight(v); }
  int value(int v) { return i_.value(v); }
  int degree(int v) { return i_.degree(v); }
};
}  // namespace vc_solver
