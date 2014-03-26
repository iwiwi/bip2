#ifndef VC_SOLVER_EXTREME_MIN_CUT_H_
#define VC_SOLVER_EXTREME_MIN_CUT_H_

#include "common.h"
#include "flow_network.h"

namespace vc_solver {
class extreme_min_cut {
 public:
  extreme_min_cut(flow_network &flow) : flow_(flow) {}

  void compute();

  int value(int i) {
    return left_[flow_.lefv(i)] ? 0 : left_[flow_.rigv(i)] ? 2 : 1;
  }

 private:
  flow_network &flow_;

  vector<bool> left_;
  void left_dfs(int v);

  // Strongly connected components (SCC) on the residual graph
  vector<bool> scc_vis_;
  vector<int> scc_ord_;
  vector<vector<int>> sccs_;
  void scc();
  void scc_dfs1(int v);
  void scc_dfs2(int v);
};
}  // namespace vc_solver

#endif  // VC_SOLVER_EXTREME_MIN_CUT_H_
