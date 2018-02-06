//
// Created by morteza on 2/4/18.
//

#include <fstream>
#include <cmath>
#include "fcm.hpp"
using std::ifstream;
using std::cout;

/*
 * Build FCM (finite-context model)
 */
void FCM::buildModel (const Parameters& p)
{
    u64 ctx;                  // Context (integer) to slide in the file
    u64 maxPV = POW5[p.k];    // Max Place Value
    u64 ctxIR;                // Inverted repeat context (integer)
    u8  curr;                 // Current symbol integer
    u64 ctxIRCurr;           // Concat of IR context and current symbol
    ifstream rf(p.ref);       // Ref file
    char c;                   // Chars read from ref file
    tbl = new u64[TAB_COL*maxPV];
//    double a=p.alpha, sa=ALPH_SZ*a;
//    tbl = new double[TAB_COL*maxPV];
//    for (u64 i=0; i!=TAB_COL*maxPV; ++i) {
//        if (i%TAB_COL==5)  tbl[i]=sa;
//        else tbl[i]=a;
//    }
    u64 rowIdx;
    
    
    ctx   = 0;
    ctxIR = maxPV-1;
    
    cerr << "Building models ...\n";
    
    // Fill tbl by no. occurrences of symbols A,C,N,G,T
//    for (u32 i=0; rf.get(c) && i!=BLK_SZ; ++i) {
    while (rf.get(c)) {
        if (c!='\n') {
            curr = NUM[c];

            // Inverted repeats
            if (p.ir) {
                ctxIRCurr = ctxIR + (IR_MAGIC-curr)*maxPV; //Concat: ctxIR, curr
                ctxIR     = ctxIRCurr/ALPH_SZ;    // Update ctxIR (integer)
                rowIdx    = ctxIR*TAB_COL;
                ++tbl[rowIdx+ctxIRCurr%ALPH_SZ];
                ++tbl[rowIdx+ALPH_SZ];            // 'sum' col
            }

            rowIdx = ctx*TAB_COL;
            ++tbl[rowIdx+curr];
            ++tbl[rowIdx+ALPH_SZ];

            // Update ctx.  (rowIdx - k) == (k * ALPH_SIZE)
            ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
//            ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
//            ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
        }
    }
    
    rf.close();
    
    cerr << "Models built.\n";
}

/*
 * Compress
 */
void FCM::compress (const Parameters& p)
{
    double a=p.alpha, sa=ALPH_SZ*a;
    const string tfName = p.tar;
    ifstream tf(tfName);
    char c;
    u8 curr;           // Current symbol in integer format
    u64 maxPV=POW5[p.k];
    u64 ctx=0;// Context(s) (integer) sliding through the dataset
    u64 rowIdx;                   // Index of a row in the table
//    u64 n;                 // No. symbols (n_s). in pr numerator
//    u64 sn;      // Sum of no. symbols (sum n_a). in pr denom.
//    double n;                 // No. symbols (n_s). in pr numerator
//    double sn;      // Sum of no. symbols (sum n_a). in pr denom.
//    double prR=0;         // Reverse of Probability of a symbol for each model
    double sEntr=0;   // Sum of entropies for different symbols
    u64 symsNo=0;  // Number of symbols
    double avEntr=0;   // Average entropy (H)
    
    cerr << "Compressing ...\n";
    
    while (tf.get(c)) {
        if (c!='\n') {
            ++symsNo;             // No. syms in target file, except \n
            
            curr   = NUM[c];    // Current symbol (integer)
            rowIdx = ctx*TAB_COL;
//            n      = tbl[rowIdx+curr];      // No.syms
//            sn     = tbl[rowIdx+ALPH_SZ];   // Sum of number of symbols
////            prR    = (double) (sn+sa)/(n+a);         // P(s|c^t)
//            prR    = (double) sn/n;         // P(s|c^t)
//            sEntr += log2(prR);             // sum( log_2 P(s|c^t) )
            sEntr += log2((tbl[rowIdx+ALPH_SZ]+sa)/(tbl[rowIdx+curr]+a));
//            sEntr += log2(tbl[rowIdx+ALPH_SZ]/tbl[rowIdx+curr]);             // sum( log_2 P(s|c^t) )
            
            
//            cerr<<"n="<<n<<'\t'<<"sn="<<sn<<'\t'<<"sn+sa/n+a="<<prR<<'\n';
            
            
            
            // Update ctx.  (rowIdx - ctx[i]) = (ctx[i] * ALPH_SIZE)
            ctx = (rowIdx-ctx)%maxPV + curr;             // Fastest
//            ctx = (rowIdx-ctx+curr)%maxPV;             // Faster
//            ctx = (ctx%POW5[p.k-1])*ALPH_SZ + curr;    // Fast
        }
    }
    
    tf.close();                      // Close target file
    
    avEntr = (double) sEntr/symsNo;
    
    cerr << "Compression finished.\n";
    cerr << "avEntr="<<avEntr;
}

/*
 * Number of symbols in a file
 */
inline u64 FCM::countSyms (string const &fileName)
{
    ifstream fileIn(fileName); // Open file
    
    // Error handling
    if (!fileIn)
    {
        cerr << "The file '" << fileName
             << "' cannot be opened, or it is empty.\n";
        fileIn.close();                 // Close file
        exit(1);                        // Exit this function
    }
    
    string fileLine;    // Each line of file
    u64 symsNo = 0;     // Number of symbols
    
    while (getline(fileIn, fileLine))
        for (char const& c : fileLine)
            ++symsNo;
    
    return symsNo;
}

/*
 * Print table
 */
void FCM::printTable (const Parameters& p)
{
    for (u8 i=0; i!=POW5[p.k]; ++i) {
        for (u8 j=0; j!=TAB_COL; ++j)
            cout << (u64) tbl[i*TAB_COL+j] << '\t';
        cout << '\n';
    }
}