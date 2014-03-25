#ifndef VC_INSTANCE_H_
#define VC_INSTANCE_H_

#include "common.h"

namespace vc_solver {
class instance {
 public:
  void init(const vector<pair<int, int>> &edges,
            const vector<double> &weight);
  bool is_unweighted() { return unweighted_; }

  // Graph
  int n() { return n_; }
  const vector<int> &adj(int v) { return adj_[v]; }
  double weight(int v) { return weight_[v]; }
  int degree(int v) { return deg_[v]; }

  // Solution
  int value(int v) {
    return fixed_count_[v] > 0 ? fixed_value_[v] : 1;
  }
  double solution_weight() { return solution_weight_; }
  double solution(vector<int> &vc);

  // Versioning
  int revision() { return graph_edit_history_.size(); }
  void revert(int revision);

  // Operations
  int fix(int v, int val);

 private:
  // Graph
  int original_n_;
  bool unweighted_;

  int n_;
  vector<vector<int>> adj_;
  vector<double> weight_;
  vector<vector<int>> original_vs_;  // Corresponding original vertices
  vector<int> deg_;  // Current degree

  // Solution
  // 0: not use, 2: use, -1: removed (by reduction)
  vector<int> fixed_count_, fixed_value_;
  double solution_weight_;

  // Versioning
  // * (u, -1) --- Fixing or creating vertex |u|
  // * (u, v)  --- Creating edge (u, v)
  vector<pair<int, int>> graph_edit_history_;

  // Operations
  int fix0(int v);
  int fix2(int v);
};
}  // namespace vc_solver

#endif  // VC_INSTANCE_H_
