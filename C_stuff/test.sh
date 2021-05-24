echo "building"
cmake -S . -B build
cmake --build build

echo ""
echo "testing
"
cd build && ctest --output-on-failure && cd ..
