#include "bounder.h"

namespace vc_solver {
double bounder::lower_bound() {
  return lower_bound_flow();
}

double bounder::lower_bound_flow() {
  return i_.solution_weight() + i_.flow().value() / 2.0;
}
}  // namespace vc_solver
