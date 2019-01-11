#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

// a,A->84(T)  c,C->71(G)  g,G->67(C)  t,T->65(A)  n,N->78(N)
static constexpr char REV[123] {
 0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,   // #20
 0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,
 0, 0, 0,  0,  0,  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,
 0, 0, 0,  0,  0,'T', 0,'G', 0, 0,  0,'C', 0, 0, 0, 0,  0,  0,'N',  0,
 0, 0, 0,  0,'A',  0, 0,  0, 0, 0,  0,  0, 0, 0, 0, 0,  0,'T',  0,'G',
 0, 0, 0,'C',  0,  0, 0,  0, 0, 0,'N',  0, 0, 0, 0, 0,'A',  0,  0,  0,
 0, 0, 0
};

size_t file_size (const string& fileName) {
  ifstream ifs(fileName, ifstream::ate | ifstream::binary);
  return ifs.tellg();
}

int main (int argc, char* argv[]) {
  const string inFileName = argv[1];
  const string outFileName = argv[2];
  size_t size = file_size(inFileName);

  ifstream inFile(inFileName);
  vector<char> buffer(size, 0);
  inFile.read(buffer.data(), size);
  inFile.close();

  reverse(begin(buffer), end(buffer)-1);

  for (auto& c : buffer)
    c = REV[static_cast<unsigned char>(c)];

  ofstream outFile(outFileName);
  outFile.write(buffer.data(), size);
  outFile.close();

  return 0;
}
