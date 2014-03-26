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
};
}  // namespace vc_solver
