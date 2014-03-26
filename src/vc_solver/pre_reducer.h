#ifndef VC_SOLVER_PRE_REDUCER_H_
#define VC_SOLVER_PRE_REDUCER_H_

#include "common.h"

namespace vc_solver {
class pre_reducer {
 public:
  pre_reducer(const vector<pair<int, int>> &original_edges,
             const vector<double> &original_weight);

  void reduce();

  void get_reduced_problem(vector<pair<int, int>> &reduced_edges,
                           vector<double> &reduced_weight);
  double get_fixed_weight();

  double get_original_solution(const vector<int> &reduced_solution,
                               vector<int> &original_solution);

  void reduce_low_degree();
  void reduce_domination();

  // Statistics
  static double stat_time_total_;

 private:
  vector<pair<int, int>> original_edges_;
  vector<double> original_weight_;

  int V_;
  vector<vector<int>> adj_;
  vector<double> weight_;

  enum status_t { NORMAL, DELETED, FIXED };
  vector<status_t> status_;
  bool modified_;

  vector<int> v_org2red_, v_red2org_;

  void remove_edge(int u, int v);
  void fix_vertex(int v);
  void purify_edges();
};
}

#endif  // VC_SOLVER_PRE_REDUCER_H_
