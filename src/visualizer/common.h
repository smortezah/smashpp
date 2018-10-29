#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "rmodel.h"
#include "defs.h"
#include "param.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

double      BoundDouble      (double, double, double);
FILE        *Fopen           (const char *, const char *);
void        UnPackByte       (uint8_t *, uint8_t);
uint8_t     PackByte         (uint8_t *);
uint64_t    SumWriteBits     (uint8_t **, int, uint64_t, FILE *, FILE *);
void        ShiftBuffer      (uint8_t *, int, uint8_t);
double      Power            (double, double);
uint8_t     DNASymToNum      (uint8_t  );
uint8_t     NumToDNASym      (uint8_t  );
uint8_t     GetCompNum       (uint8_t  );
uint8_t     GetCompSym       (uint8_t  );
uint64_t    NDNASyminFile    (FILE *);
uint64_t    NDNASymInFastq   (FILE *);
uint64_t    NDNASymInFasta   (FILE *);
uint64_t    NBytesInFile     (FILE *);
uint64_t    FopenBytesInFile (const char *);
uint8_t     *ReverseStr      (uint8_t *, uint32_t);
char        *CloneString     (char *   );
char        *RepString       (const char *, const char *, const char *);
uint32_t    ArgsNum          (uint32_t , char *[], uint32_t, char *, uint32_t,
                              uint32_t);
double      ArgsDouble       (double, char *[], uint32_t, char *);
uint8_t     ArgsState        (uint8_t  , char *[], uint32_t, char *);
char        *ArgsString      (char    *, char *[], uint32_t, char *);
char        *ArgsFilesMap    (char *[], uint32_t, char *);
char        *ArgsFilesReduce (char *[], uint32_t, char *);
char        *ArgsFilesImg    (char *[], uint32_t, char *);
void        TestReadFile     (char *);
uint8_t     CmpCheckSum      (uint32_t, uint32_t);
void        FAccessWPerm     (char    *);
void        CalcProgress     (uint64_t , uint64_t);
void        PrintArgs        (Parameters *);
char        *concatenate     (char *, char *);
int32_t     StrToArgv        (char *, char ***);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
