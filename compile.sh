mkdir -p build
cd build
cmake ../src
make -j4
mv smashpp ..
cd ..