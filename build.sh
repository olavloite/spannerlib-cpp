cmake -S . -B cmake-build-release \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_MAKE_PROGRAM=/usr/bin/make \
    -DCMAKE_TOOLCHAIN_FILE=/home/loite/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build cmake-build-release --target main -j 6

cp /home/loite/spannerlib-cpp/cmake-build-release/libspannerlib.so /home/loite/RiderProjects/benchmark-spanner-lib/benchmark-spanner-lib/bin/Release/net9.0/runtimes/linux-x64/native/spannerlib.so
