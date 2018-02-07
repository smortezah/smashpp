//
// Created by morteza on 02-02-2018.
//

#define __STDC_FORMAT_MACROS
#if defined(_MSC_VER)
#    include <io.h>
#else
#    include <unistd.h>
#endif

#include <iostream>
#include <chrono>
#include <iomanip>      // setw, setprecision
//#include "def.hpp"
#include "par.hpp"
#include "fcm.hpp"
using std::cout;
using std::chrono::high_resolution_clock;
using std::setprecision;


int main (int argc, char* argv[])
{
    auto* p = new Parameters;
    p->parse(argc, argv);
    
    auto* m = new FCM(*p);    // Uses some parsed values

//    // Build models
//    auto t0 = high_resolution_clock::now(); // Start time
//    m->buildModel(*p);
//    auto t1 = high_resolution_clock::now(); // Finish time
//    dur_t e = t1-t0; // Elapsed
//    cout << "in " << std::fixed<< setprecision(3) << e.count() << " seconds.\n";
//
////    m->printTable(*p);
//
//    // Compress
//    t0 = high_resolution_clock::now();
//    m->compress(*p);
//    t1 = high_resolution_clock::now();
//    e = t1-t0;
//    cout << "in " << std::fixed<< setprecision(3) << e.count() << " seconds.\n";

    
    delete m, p;

    return 0;
}




//#include <iostream>
//#include <sparsehash/dense_hash_map>
//#include <backward/hash_fun.h>
//#include <cstring>
//
//using google::dense_hash_map;      // namespace where class lives by default
//using std::cout;
//using std::endl;
//using __gnu_cxx::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
//
//struct eqstr
//{
//    bool operator()(const char* s1, const char* s2) const
//    {
//        return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
//    }
//};
//
//int main()
//{
//    dense_hash_map<const char*, int, hash<const char*>, eqstr> months;
//
//    months.set_empty_key(NULL);
//    months["january"] = 31;
//    months["february"] = 28;
//    months["march"] = 31;
//    months["april"] = 30;
//    months["may"] = 31;
//    months["june"] = 30;
//    months["july"] = 31;
//    months["august"] = 31;
//    months["september"] = 30;
//    months["october"] = 31;
//    months["november"] = 30;
//    months["december"] = 31;
//
//    cout << "september -> " << months["september"] << endl;
//    cout << "april     -> " << months["april"] << endl;
//    cout << "june      -> " << months["june"] << endl;
//    cout << "november  -> " << months["november"] << endl;
//}