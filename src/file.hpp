#ifndef PROJECT_FILE_HPP
#define PROJECT_FILE_HPP

#include <iterator>
#include "def.hpp"
#include "file.hpp"
#include "exception.hpp"

namespace smashpp {
struct SubSeq {
  string inName, outName;
  u64    begPos;
  streamsize size;
};

inline static void ignore_this_line (ifstream& fs) {
  fs.ignore(numeric_limits<std::streamsize>::max(), '\n');
}

inline static void check_file (const string& name) {  // Must be inline
  ifstream f(name);
  if (!f) {
    f.close();
    error("the file \"" + name + "\" cannot be opened or is empty.");
  }
  else {
    bool foundChar{false};
    for (char c; f.get(c) && !foundChar;)
      if (c != ' ' && c != '\n' && c != '\t')
        foundChar = true;
    if (!foundChar)
      error("the file \"" + name + "\" is empty.");
    f.close();
  }
}

inline static bool file_is_empty (const string& name) {
  ifstream f(name);
  bool foundChar {false};

  for (char c; f.get(c) && !foundChar;)
    if (c!=' ' && c!='\n' && c!='\t')
      foundChar = true;
  f.close();

  if (!foundChar)
    return true;

  return false;
}

inline static string file_name (const string& str) {
  const auto found = str.find_last_of("/\\");
  return str.substr(found+1);
}

inline static u64 file_size (const string& name) {
  check_file(name);
  ifstream f(name, ifstream::ate | ifstream::binary);
  return static_cast<u64>(f.tellg());
}

inline static u64 file_lines (const string& name) {
  ifstream f(name);
  f.unsetf(ios_base::skipws);  // New lines will be skipped unless we stop it
  return u64(count(istream_iterator<char>(f), istream_iterator<char>(), '\n'));
}

// Must be inline
inline static void extract_subseq (const unique_ptr<SubSeq>& subseq) {
  ifstream fIn(subseq->inName);
  ofstream fOut(subseq->outName);

  fIn.seekg(subseq->begPos);
  vector<char> buffer(static_cast<u64>(subseq->size), 0);
  fIn.read(buffer.data(), subseq->size);
  fOut.write(buffer.data(), subseq->size);

  fIn.close();  fOut.close();
}

inline static FileType file_type (const string& name) {
  check_file(name);
  ifstream f(name);
  char c;
  while (f.peek()=='\n' || f.peek()==' ')  f.get(c); //Skip leading blank spaces

  // Fastq
  while (f.peek()=='@')  ignore_this_line(f);
  for (u8 nTabs=0; f.get(c) && c!='\n';)  if (c=='\t') ++nTabs;
  if (f.peek()=='+') { f.close();  return FileType::FASTQ; }

  // Fasta or bare Seq
  f.clear();  f.seekg(0, std::ios::beg);
  while (f.peek()!='>' && f.peek()!=EOF)  ignore_this_line(f);
  if (f.peek()=='>') { f.close();  return FileType::FASTA; }
  else               { f.close();  return FileType::SEQ;   }
}

inline static void to_seq (const string& inName, const string& outName, 
const FileType& type) {
  ifstream fIn(inName);
  ofstream fOut(outName);

  if (type == FileType::FASTA) {
    bool isHeader = false;  // MUST be positioned before the following loop
    for (vector<char> buffer(FILE_BUF,0); fIn.peek()!=EOF;) {
      fIn.read(buffer.data(), FILE_BUF);
      string out;
      for (auto it=buffer.begin(); it!=buffer.begin()+fIn.gcount(); ++it) {
        const auto c = *it;
        if      (c=='>')        {               isHeader=true;   continue; }
        else if (c=='\n')       { if (isHeader) isHeader=false;  continue; }
        else if (isHeader)      {                                continue; }
        else if (c>64 && c<123) { out += c;                                }
      }
      fOut.write(out.data(), out.size());
    }
  }
  else if (type == FileType::FASTQ) {
    u8 line  = 0;        // MUST be positioned before the following loop
    bool isDNA = false;  // MUST be positioned before the following loop
    for (vector<char> buffer(FILE_BUF,0); fIn.peek()!=EOF;) {
      fIn.read(buffer.data(), FILE_BUF);
      string out;
      for (auto it=buffer.begin(); it!=buffer.begin()+fIn.gcount(); ++it) {
        const auto c = *it;
        switch (line) {
        case 0:  if (c=='\n') { line=1;  isDNA=true;  }  break;
        case 1:  if (c=='\n') { line=2;  isDNA=false; }  break;
        case 2:  if (c=='\n') { line=3;  isDNA=false; }  break;
        case 3:  if (c=='\n') { line=0;  isDNA=false; }  break;
        default:                                         break;
        }
        if (!isDNA || c=='\n')  continue;
        if (c>64 && c<123)      out += c;
      }
      fOut.write(out.data(), out.size());
    }
  }

  fIn.close();  fOut.close();
}
}

#endif //PROJECT_FILE_HPP