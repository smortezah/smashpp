// Smash++
// Morteza Hosseini    mhosayny@gmail.com

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

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

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: smashpp-inv-rep <INPUT_FILE> <OUTPUT_FILE>\n";
    return EXIT_FAILURE;
  }

  const std::string inFileName{argv[1]};
  const std::string outFileName{argv[2]};

  std::ifstream inFile(inFileName, std::ios::binary);
  if (!inFile) {
    std::cerr << "Error: cannot open input file \"" << inFileName << "\".\n";
    return EXIT_FAILURE;
  }

  inFile.seekg(0, std::ios::end);
  const auto endPos = inFile.tellg();
  if (endPos < 0) {
    std::cerr << "Error: cannot determine input file size for \"" << inFileName
              << "\".\n";
    return EXIT_FAILURE;
  }

  if (endPos > std::numeric_limits<std::streamsize>::max()) {
    std::cerr << "Error: input file \"" << inFileName
              << "\" is too large to process.\n";
    return EXIT_FAILURE;
  }

  const auto size = static_cast<size_t>(endPos);
  inFile.seekg(0, std::ios::beg);

  std::vector<char> buffer(size, 0);
  if (size != 0 &&
      !inFile.read(buffer.data(), static_cast<std::streamsize>(size))) {
    std::cerr << "Error: failed to read input file \"" << inFileName
              << "\".\n";
    return EXIT_FAILURE;
  }
  inFile.close();

  auto payloadSize = buffer.size();
  while (payloadSize != 0 &&
         (buffer[payloadSize - 1] == '\n' || buffer[payloadSize - 1] == '\r'))
    --payloadSize;

  std::reverse(std::begin(buffer), std::begin(buffer) + payloadSize);

  for (auto it = std::begin(buffer);
       it != std::begin(buffer) + payloadSize; ++it)
    *it = REV[static_cast<unsigned char>(*it)];

  std::ofstream outFile(outFileName, std::ios::binary);
  if (!outFile) {
    std::cerr << "Error: cannot open output file \"" << outFileName << "\".\n";
    return EXIT_FAILURE;
  }
  if (size != 0)
    outFile.write(buffer.data(), static_cast<std::streamsize>(size));
  if (!outFile) {
    std::cerr << "Error: failed to write output file \"" << outFileName
              << "\".\n";
    return EXIT_FAILURE;
  }
  outFile.close();

  return 0;
}
