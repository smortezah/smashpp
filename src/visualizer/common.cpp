#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "defs.h"
#include "mem.h"
#include "common.h"
#include "msg.h"
#include <fstream>
#include <iostream>
#include "../fn.hpp"
using namespace std;
using namespace smashpp;

double BoundDouble (double low, double val, double high) {
  if (val<low)   return low;
  if (val>high)  return high;
  return val;
}

void UnPackByte (u8* bin, u8 sym) {
  *bin++ = (sym & 0x80) >> 7;
  *bin++ = (sym & 0x40) >> 6;
  *bin++ = (sym & 0x20) >> 5;
  *bin++ = (sym & 0x10) >> 4;
  *bin++ = (sym & 0x08) >> 3;
  *bin++ = (sym & 0x04) >> 2;
  *bin++ = (sym & 0x02) >> 1;
  *bin++ = (sym & 0x01);
}

u8 PackByte (u8* bin) {
  u8 byte = 0;
/*
  byte = (((*bin++)<<7) | ((*bin++)<<6) | ((*bin++)<<5) |
          ((*bin++)<<4) | ((*bin++)<<3) | ((*bin++)<<2) |
          ((*bin++)<<1) |  (*bin)); // 48 = '0' in decimal
*/
  return byte;
}

// Pow function from http://martin.ankerl.com/2007/10/04/
// optimized-pow-approximation-for-java-and-c-c/
double Power (double a, double b) {
  int tmp  = (*(1 + (int*)&a));
  int tmp2 = int(b*(tmp-1072632447) + 1072632447);
  double p = 0.0;
  *(1 + (int*)&p) = tmp2;
  return p;
}

void ShiftBuffer (u8* buf, int size, u8 val) {
  memmove(buf, buf+1, size*sizeof(u8));
  buf[size-1] = val;
}

// REPLACE STRING
char* ReplaceSubStr (char* str, char* a, char* b) {
  char* buf = (char*) Calloc(MAX_STR, sizeof(char));
  char* p;
  if (strlen(str)>MAX_STR)    error("string too long!");
  if (!(p = strstr(str, a)))  return str;
  strncpy(buf, str, p-str);
  buf[p-str] = '\0';
  sprintf(buf+(p-str), "%s%s", b, p+strlen(a));
  return buf;
}

u64 NBytesInFile (FILE* file) {
  u64 size=0;
  fseeko(file, 0, SEEK_END);
  size = ftello(file);
  rewind(file);
  return size;
}

u64 NDNASyminFile (FILE* file) {
  u8 buffer[BUFFER_SIZE];
  u32 k, idx;
  u64 nSymbols = 0;

  while (k = fread(buffer, 1, BUFFER_SIZE, file))
    for (idx=0; idx != k; ++idx)
      switch (buffer[idx]) {
      case 'A':  ++nSymbols;  break;
      case 'T':  ++nSymbols;  break;
      case 'C':  ++nSymbols;  break;
      case 'G':  ++nSymbols;  break;
      default:;
      }

  rewind(file);
  return nSymbols;
}

u64 NDNASymInFasta (FILE* file) {
  u8 buffer[BUFFER_SIZE], sym=0, header=1;
  u32 k, idx;
  u64 nSymbols=0;

  while ((k = fread(buffer, 1, BUFFER_SIZE, file)))
    for (idx=0; idx != k; ++idx) {
      sym = buffer[idx];
      if (sym=='>')               { header=1;  continue; }
      if (sym=='\n' && header==1) { header=0;  continue; }
      if (sym=='\n')                           continue;
      if (sym=='N')                            continue;
      if (header==1)                           continue;
      // FINAL FILTERING DNA CONTENT
      if (sym!='A' && sym!='C' && sym!='G' && sym!='T')
        continue;
      ++nSymbols;
    }

  rewind(file);
  return nSymbols;
}

u64 NDNASymInFastq (FILE* file) {
  u8 buffer[BUFFER_SIZE], sym=0, line=0, dna=0;
  u32 k, idx;
  u64 nSymbols=0;

  while (k = fread(buffer, 1, BUFFER_SIZE, file))
    for (idx=0; idx != k; ++idx) {
      sym = buffer[idx];

      switch (line) {
      case 0:  if (sym=='\n') { line=1;  dna=1; }  break;
      case 1:  if (sym=='\n') { line=2;  dna=0; }  break;
      case 2:  if (sym=='\n') { line=3;  dna=0; }  break;
      case 3:  if (sym=='\n') { line=0;  dna=0; }  break;
      }
      if (dna==0 || sym=='\n')  continue;
      if (dna==1 && sym=='N')   continue;

      // FINAL FILTERING DNA CONTENT
      if (sym!='A' && sym!='C' && sym!='G' && sym!='T')
        continue;
      ++nSymbols;
    }

  rewind(file);
  return nSymbols;
}

u64 FopenBytesInFile (const char* fn) {
  u64 size=0;
  FILE* file = Fopen(fn, "r");
  size = NBytesInFile(file);
  fclose(file);
  return size;
}

u8 DNASymToNum (u8 sym) {
  switch (sym) {
  case 'A':  return 0;
  case 'T':  return 3;
  case 'C':  return 1;
  case 'G':  return 2;
  case 'N':  return 2;
  default :  return 4;
  }
}

u8 NumToDNASym (u8 sym) {
  switch (sym) {
  case 0:  return 'A';
  case 3:  return 'T';
  case 1:  return 'C';
  case 2:  return 'G';
  case 4:  return 'N';
  default: error("unknown numerical symbols.");
  }
}

u8 GetCompSym (u8 sym) {
  switch (sym) {
  case 'A':  return 'T';
  case 'C':  return 'G';
  case 'G':  return 'C';
  case 'T':  return 'A';
  default:   return sym;
  }
}

u8 GetCompNum (u8 sym) {
  switch (sym) {
  case 0:  return 3;
  case 1:  return 2;
  case 2:  return 1;
  case 3:  return 0;
  default: cerr<<"Symbol 'N' found in inverted computation\n";
           return sym;
  }
}

FILE* Fopen (const char* path, const char* mode) {
  FILE* file = fopen(path, mode);
  if (file == NULL) {
    fprintf(stderr, "Error opening: %s (mode %s). Does the file exist?\n",
            path, mode);
    exit(1);
  }
  return file;
}

u8* ReverseStr (u8* str, u32 end) {
  for (u32 start=0; start < end;) {
    str[start] ^= str[end];
    str[end]   ^= str[start];
    str[start] ^= str[end];
    ++start;
    --end;
  }
  return str;
}

void SortString (char str[]) {
  i32 length=strlen(str);

  for (i32 i=0; i != length-1; ++i)
    for (i32 j=i+1; j != length; ++j)
      if (str[i] > str[j]) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
      }
}

char* CloneString (char* str) {
  char* clone;
  if (str == NULL)
    return NULL;
  strcpy((clone = (char*) Malloc((strlen(str)+1) * sizeof(char))), str);
  return clone;
}

char* concatenate (char* a, char* b) {
  char* base;
  base = (char*) Malloc(strlen(a)+strlen(b)+1);
  strcpy(base, a);
  strcat(base, b);
  return base;
}

u32 ArgsNum (u32 d, char* a[], u32 n, char* s, u32 l, u32 u) {
  u32 x;
  for (; --n;)
    if (!strcmp(s, a[n])) {
      if ((x=atol(a[n+1])) < l || x > u)
        error("invalid number! Interval: ["+to_string(l)+";"+to_string(u)+"].");
      return x;
    }
  return d;
}

double ArgsDouble (double def, char* arg[], u32 n, char* str) {
  for (; --n;)
    if (!strcmp(str, arg[n]))
      return atof(arg[n+1]);
  return def;
}

u8 ArgsState (u8 def, char* arg[], u32 n, char* str) {
  for (; --n;)
    if (!strcmp(str, arg[n]))
      return def==0 ? 1 : 0;
  return def;
}

char* ArgsString (char* def, char* arg[], u32 n, char* str) {
  for (; --n;)
    if (!strcmp(str, arg[n]))
      return arg[n+1];
  return def;
}

char* ArgsFilesMap (char* arg[], u32 argc, char* str) {
  for (i32 n=argc; --n;)
    if (!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return concatenate((char*) "match", (char*) ".map");
}

char* ArgsFilesReduce (char* arg[], u32 argc, char* str) {
  for (i32 n=argc; --n;)
    if (!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return concatenate((char*) "match", (char*) ".red");
}

char* ArgsFilesImg (char* arg[], u32 argc, char* str) {
  for (i32 n=argc; --n;)
    if (!strcmp(str, arg[n]))
      return CloneString(arg[n+1]);

  return concatenate((char*) "map", (char*) ".svg");
}

void FAccessWPerm (char* fn) {
  if (access(fn, F_OK) != -1) {
    fprintf(stderr, "Error: file %s already exists!\n", fn);
    if (access(fn, W_OK) != -1)
      cerr << "Note: file " << fn << " has write permission.\nTip: to force "
              "writing rerun with \"-f\" option.\nWarning: on forcing, the "
              "old (" << fn << ") file will be deleted permanently.\n";
    exit(1);
  }
}

void TestReadFile (char* fn) {
  FILE* f = NULL;
  f = Fopen(fn, "r");
  fclose(f);
}

// GET STRING TO ARGV
int32_t StrToArgv (char* s, char*** v) {
  i32 n=0, c=2;
  char* d = strdup(s);   // STRDUP <=> CLONE STR
  while (d[n])
    if (d[n++] == ' ')
      ++c;
  *v = (char**) Calloc(c, sizeof(char*));
  n = 0;
  (*v)[0] = d;
  c = 1;
  do
    if (d[n] == ' ') {
      d[n] = '\0';
      (*v)[c++] = d+n+1;
    }
  while (d[++n]);
  return c;
}

void CalcProgress (u64 size, u64 i) {
  if (i%(size/100)==0 && size>1000)
    fprintf(stderr, "      Progress:%3d %%\r", (u8) (i / (size / 100)));
}

u8 CmpCheckSum (u32 cs, u32 checksum) {
  if (checksum != cs) {
    error("invalid reference file!\n"
          "Compression reference checksum ................. "+to_string(cs)+"\n"
          "Decompression reference checksum ............... "+
          to_string(checksum)+"\n"
          "Tip: rerun with correct reference file\n");
    return 1;
  }
  return 0;
}

void PrintArgs (Parameters* P) {
  cerr <<
    "==[ CONFIGURATION ]=================\n"
    "Verbose mode ....................... " <<(P->verbose==0  ?"no":"yes")<<"\n"
    "Force mode ......................... " <<(P->force==0    ?"no":"yes")<<"\n"
    "Using inversions ................... " <<(P->inversion==0?"no":"yes")<<"\n"
    "K-mer size ......................... " << P->kmer                    <<"\n"
    "Minimum block size ................. " << P->minimum                 <<"\n"
    "Repeats number ..................... " << P->repeats                 <<"\n"
    "Number of threads .................. " << P->nThreads                <<"\n"
    "Output positions filename .......... " << P->positions               <<"\n"
    "Contigs:"                                                            <<"\n"
    "  [+] Name ......................... " << P->Tar.name                <<"\n"
    "  [+] Length ....................... " << P->Tar.length              <<"\n"
    "Reference:"                                                          <<"\n"
    "  [+] Name ......................... " << P->Ref.name                <<"\n"
    "  [+] Length ....................... " << P->Ref.length              <<"\n"
                                                                        << endl;
}