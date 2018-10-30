#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "rmodel.h"
#include "defs.h"
#include "param.h"

//namespace smashpp {
double BoundDouble      (double, double, double);
FILE*  Fopen            (const char*, const char*);
void   UnPackByte       (u8*, u8);
u8     PackByte         (u8*);
u64    SumWriteBits     (u8**, int, u64, FILE*, FILE*);
void   ShiftBuffer      (u8*, int, u8);
double Power            (double, double);
u8     DNASymToNum      (u8);
u8     NumToDNASym      (u8);
u8     GetCompNum       (u8);
u8     GetCompSym       (u8);
u64    NDNASyminFile    (FILE*);
u64    NDNASymInFastq   (FILE*);
u64    NDNASymInFasta   (FILE*);
u64    NBytesInFile     (FILE*);
u64    FopenBytesInFile (const char*);
u8*    ReverseStr       (u8*, u32);
char*  CloneString      (char*   );
char*  RepString        (const char*, const char*, const char *);
u32    ArgsNum          (u32 , char*[], u32, char*, u32, u32);
double ArgsDouble       (double, char*[], u32, char *);
u8     ArgsState        (u8, char*[], u32, char *);
char*  ArgsString       (char*, char *[], u32, char *);
char*  ArgsFilesMap     (char*[], u32, char*);
char*  ArgsFilesReduce  (char*[], u32, char*);
char*  ArgsFilesImg     (char*[], u32, char*);
u8     CmpCheckSum      (u32, u32);
void   FAccessWPerm     (char*);
void   CalcProgress     (u64, u64);
void   PrintArgs        (Parameters*);
char*  concatenate      (char*, char*);
i32    StrToArgv        (char*, char***);
//}

#endif