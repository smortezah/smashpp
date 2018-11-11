#include "vizparam.hpp"
#include "vizpaint.hpp"
#include "fn.hpp"
using namespace std;
using namespace smashpp;


int main (int argc, char* argv[]) {
  try {
    const auto t0 {now()};

    VizParam par;
    par.parse(argc, argv);

    cerr << endl;
    auto paint = make_unique<VizPaint>();
    paint->print_plot(par);
    cerr << endl;

    const auto t1 {now()};
    cerr << "==[ STATISTICS ]====================\n"
         << "Total time: " << hms(t1-t0) << endl;
  }
  catch (std::exception& e) { cout << e.what(); }
  catch (...) { return EXIT_FAILURE; }

  return 0;
}