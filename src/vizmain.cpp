#include "vizparam.hpp"
#include "vizpaint.hpp"
#include "fn.hpp"
using namespace std;
using namespace smashpp;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - M A I N - - - - - - - - - - - - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main (int argc, char* argv[]) {
  const auto t0 {now()};

  VizParam par;
  par.parse(argc, argv);

  cerr << endl;
  auto paint = make_shared<VizPaint>();
  paint->print_plot(par);
  cerr << endl;

  const auto t1 {now()};
  cerr << "==[ STATISTICS ]====================\n"
       << "Total time: " << hms(t1-t0) << endl;

  return EXIT_SUCCESS;
}