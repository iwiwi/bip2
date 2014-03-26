#include "common.h"
#include "vc_solver/solver.h"

DEFINE_string(problem, "", "");

int main(int argc, char **argv) {
  JLOG_INIT(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);

  JLOG_PUT("setting.problem", FLAGS_problem);
  vector<pair<int, int>> es;
  int num_vs = load_graph(FLAGS_problem.c_str(), es);
  JLOG_PUT("setting.num_vs", num_vs);
  JLOG_PUT("setting.num_es", es.size());

  vector<int> vc;
  double vc_weight;
  JLOG_PUT_BENCHMARK("time.solve_total") {
    vc_solver::solver s;
    vc_weight = s.solve(es, vector<double>(num_vs, 1), vc);
  }
  JLOG_PUT("solution", vc_weight);
  printf("%.f\n", vc_weight);

  { // Check
    vector<bool> b(num_vs);
    for (int v : vc) b[v] = true;
    for (auto e : es) CHECK(b[e.first] || b[e.second]);
  }
  exit(EXIT_SUCCESS);
}
