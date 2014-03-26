#include "common.h"
#include "vc_solver/instance.h"

///////////////////////////////////////////////////////////////////////////////
// Load
///////////////////////////////////////////////////////////////////////////////

namespace {
inline bool readeof(FILE *fp) {
  for (;;) {
    int c = getc(fp);
    if (c == EOF) {
      return true;
    } else if (isspace(c)) {
      continue;
    } else {
      ungetc(c, fp);
      return false;
    }
  }
  assert(false);
}

inline long readlong(FILE *fp) {
  int c, s;
  long x;
  while (!isdigit(c = getc(fp)) && c != '-');
  if (c == '-') {
    s = -1;
    x = 0;
  } else {
    assert(isdigit(c));
    s = 1;
    x = c - '0';
  }
  while (isdigit(c = getc(fp))) {
    x = (x * 10 + (c - '0'));
  }
  return s * x;
}
}

int load_graph(FILE *fp, vector<std::pair<int, int> > &es) {
  JLOG_PUT_BENCHMARK("time.load") {
    int num_v = 0;
    es.clear();

    long l1a, l1b;
    l1a = readlong(fp);
    l1b = readlong(fp);

    while (!readeof(fp)) {
      int u, v;
      u = readlong(fp);
      v = readlong(fp);
      if (u == v) continue;

      es.pb(mp(u, v));
      num_v = max(num_v, max(u, v) + 1);
    }

    if ((long)es.size() == l1b) {
      return l1a;
    } else {
      es.pb(mp(l1a, l1b));
      return num_v;
    }
  }
  assert(false);
}

int load_graph(const char *file, vector<pair<int, int> > &es) {
  FILE *fp = fopen(file, "r");
  CHECK(fp != NULL);

  int r = load_graph(fp, es);
  fclose(fp);
  return r;
}

///////////////////////////////////////////////////////////////////////////////
// Utility
///////////////////////////////////////////////////////////////////////////////

void compute_connected_components(int n,
                                  const vector<pair<int, int> >& edges,
                                  vector<vector<int> >& cc_vertices,
                                  vector<vector<pair<int, int> > >& cc_edges) {
  vector<vector<int>> adj(n);
  for (auto e : edges) {
    adj[e.first ].emplace_back(e.second);
    adj[e.second].emplace_back(e.first );
  }

  cc_vertices.clear();
  cc_edges.clear();

  vector<int> id(n, -1);
  rep (s, n) {
    if (id[s] != -1) continue;

    cc_vertices.emplace_back();
    cc_edges.emplace_back();
    vector<int> &cvs = cc_vertices.back();
    vector<pair<int, int>> &ces = cc_edges.back();

    queue<int> que;
    que.push(s);
    id[s] = 0;
    cvs.emplace_back(s);

    while (!que.empty()) {
      int v = que.front();
      que.pop();
      for (int u : adj[v]) {
        if (id[u] != -1) {
          if (id[u] < id[v]) ces.emplace_back(id[u], id[v]);
        } else {
          que.push(u);
          id[u] = cvs.size();
          cvs.emplace_back(u);
        }
      }
    }
    assert(ces.size() <= edges.size());

    if (cvs.size() == 1) {
      cc_vertices.pop_back();
      cc_edges.pop_back();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Test
///////////////////////////////////////////////////////////////////////////////

namespace {
double solve_vc_naive_dfs(const vector<pair<int, int> > &edges,
                   const vector<double> &weight,
                   vector<int> &vc,
                   vector<bool> &use) {
  rep (i, edges.size()) {
    int v = edges[i].first, w = edges[i].second;
    if (!use[v] && !use[w]) {
      vector<int> tmp_vc_v, tmp_vc_w;

      use[v] = true;
      double tmp_v = weight[v] + solve_vc_naive_dfs(edges, weight, tmp_vc_v, use);
      use[v] = false;

      use[w] = true;
      double tmp_w = weight[w] + solve_vc_naive_dfs(edges, weight, tmp_vc_w, use);
      use[w] = false;

      if (tmp_v < tmp_w) {
        vc = tmp_vc_v;
        vc.emplace_back(v);
      } else {
        vc = tmp_vc_w;
        vc.emplace_back(w);
      }
      return min(tmp_v, tmp_w);
    }
  }

  return 0;
}
}  // namespace

double solve_vc_naive(const vector<pair<int, int> > &edges,
                   const vector<double> &weight,
                   vector<int> &vc) {
  vector<bool> use(weight.size(), false);
  return solve_vc_naive_dfs(edges, weight, vc, use);
}

double solve_vc_naive(const vector<pair<int, int> > &edges,
                   const vector<double> &weight) {
  vector<int> vc;
  return solve_vc_naive(edges, weight, vc);
}

double solve_vc_naive(vc_solver::instance &i, vector<int> &vc) {
  rep (v, i.n()) {
    if (i.value(v) == 1) {
      int r = i.revision();
      vector<int> vc0, vc2;
      i.fix(v, 0);
      double w0 = solve_vc_naive(i, vc0);
      i.revert(r);
      i.fix(v, 2);
      double w2 = solve_vc_naive(i, vc2);
      i.revert(r);
      vc = w0 < w2 ? vc0 : vc2;
      return min(w0, w2);
    }
  }
  return i.solution(vc);
}

double solve_vc_naive(vc_solver::instance &i) {
  vector<int> vc;
  return solve_vc_naive(i, vc);
}

int generate_random_graph(int max_v, int max_e,
                           vector<pair<int, int>> &edges) {
  int num_v = 1 + rand() % max_v;
  int num_e = min(1 + rand() % max_e, num_v * (num_v - 1) / 2);

  set<pair<int, int>> e;
  while ((int)e.size() < num_e) {
    int u = rand() % num_v, v = rand() % num_v;
    if (u == v) continue;
    if (u > v) swap(u, v);
    e.insert({u, v});
  }
  edges.assign(e.begin(), e.end());

  return num_v;
}

void generate_random_weight_integer(int num_v, int max_w, vector<double> &weight) {
  weight.resize(num_v);
  rep (i, num_v) weight[i] = 1 + rand() % max_w;
}

void generate_random_weight_non_integer(int num_v, vector<double> &weight) {
  weight.resize(num_v);
  rep (i, num_v) weight[i] = (1.0 + rand()) / (double)RAND_MAX;
}

bool is_vc(int num_vs,
           const vector<pair<int, int> > &edges,
           const vector<int> &vc) {
  vector<bool> b(num_vs, false);
  for (int v : vc) b[v] = true;
  for (const auto &e : edges) {
    if (!b[e.first] && !b[e.second]) {
      return false;
    }
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Macro for statistics
///////////////////////////////////////////////////////////////////////////////

double get_current_time_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}
