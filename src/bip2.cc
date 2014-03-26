#include "bip2.h"
#include "vc_solver/pre_reducer.h"
#include "vc_solver/solver.h"

using namespace vc_solver;

double solve_vc(const vector<pair<int, int> > &edges,
                const vector<double> &weight, vector<int> &vc) {
  // Pre-reduce
  pre_reducer r(edges, weight);
  r.reduce();
  vector<pair<int, int>> reduced_edges;
  vector<double> reduced_weight;
  r.get_reduced_problem(reduced_edges, reduced_weight);

  // Decompose connected components
  vector<vector<int>> cc_vertices;
  vector<vector<pair<int, int>>> cc_edges;
  compute_connected_components(reduced_weight.size(), reduced_edges,
                               cc_vertices, cc_edges);

  // Sort the connected components in decreasing order of size.
  // This is to exploit the bound given by |known_best|.
  vector<int> cc_ord(cc_vertices.size());
  iota(all(cc_ord), 0);
  sort(all(cc_ord),
       [&](int a, int b) { return cc_vertices[a].size() < cc_vertices[b].size(); });

  // Solve each connected component
  solver s;
  vector<int> reduced_vc;
  rep (k, cc_ord.size()) {
    int i = cc_ord[k];
    const vector<int> &vs = cc_vertices[i];
    vector<double> ws(vs.size());
    rep (j, vs.size()) ws[j] = reduced_weight[vs[j]];

    vector<int> vc;
    s.solve(cc_edges[i], ws, vc);
    for (int j : vc) reduced_vc.emplace_back(vs[j]);
  }

  // Restor the original solution
  return r.get_original_solution(reduced_vc, vc);
}
