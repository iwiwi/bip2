#include "extreme_min_cut.h"

namespace vc_solver {
void extreme_min_cut::compute() {
  // Compute the initial min-cut
  left_.assign(flow_.V, false);
  left_dfs(flow_.S);

  // Push tail SCCs
  scc();

  for (int i = (int)sccs_.size() - 1; i >= 0; --i) {
    vector<int> &scc = sccs_[i];
    for (int v : scc) left_[v] = true;  // Temporary!

    // Check if this SCC is currently a tail
    for (int v : scc) {
      for (const flow_network::edge_t &e : flow_.adj_[v]) {
        if (e.cap > 0 && !left_[e.to]) goto dmp;
      }
    }

    // Check if pushing this SCC is valid
    rep (i, scc.size()) {
      int v = scc[i];
      if (v == flow_.T) goto dmp;
      if (left_[v ^ 1]) goto dmp;
    }

    // Pushing this SCC is legal!
    continue;
    dmp:;

    // Pushing this SCC is illegal, cancel
    for (int v : scc) left_[v] = false;
  }
}

void extreme_min_cut::left_dfs(int v) {
  if (left_[v]) return;
  left_[v] = true;
  for (const flow_network::edge_t &e : flow_.adj_[v]) {
    if (e.cap > 0) left_dfs(e.to);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Computing strongly connected components in the residual graph
///////////////////////////////////////////////////////////////////////////////

void extreme_min_cut::scc() {
  scc_vis_.assign(flow_.V, false);
  scc_ord_.clear();
  for (int v = 0; v < flow_.V; v++) scc_dfs1(v);
  assert((int)scc_ord_.size() == flow_.V);

  scc_vis_.assign(flow_.V, false);
  sccs_.clear();
  for (int i = flow_.V - 1; i >= 0; i--) {
    int v = scc_ord_[i];
    if (!scc_vis_[v]) {
      sccs_.emplace_back();
      scc_dfs2(v);
    }
  }
}

void extreme_min_cut::scc_dfs1(int v) {
  if (scc_vis_[v]) return;
  scc_vis_[v] = true;
  for (const flow_network::edge_t &e : flow_.adj_[v]) if (e.cap > 0) scc_dfs1(e.to);
  scc_ord_.pb(v);
}

void extreme_min_cut::scc_dfs2(int v) {
  if (scc_vis_[v]) return;
  scc_vis_[v] = true;
  sccs_.back().emplace_back(v);
  for (const flow_network::edge_t &e : flow_.adj_[v]) if (flow_.rev(e).cap > 0) scc_dfs2(e.to);
}
}  // namespace vc_solver
