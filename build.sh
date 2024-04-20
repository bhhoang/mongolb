#!/usr/bin/env bash

#Check if lib/boost is present
if [ ! -d "lib/boost" ]; then
    echo "Not found lib/boost"
    exit 1
fi
echo "Building boost..."
pushd lib/boost
./bootstrap.sh && ./b2 stage link=static
popd

BOOST_ROOT=$(realpath lib/boost)
BOOST_LIBS=$(realpath lib/boost/stage/lib)

# Check if lib/mongo-cxx-driver is present``
if [ ! -d "lib/mongo-cxx-driver" ]; then
    echo "Not found lib/mongo-cxx-driver"
    exit 1
fi
echo "Building mongo-cxx-driver..."
pushd lib/mongo-cxx-driver/build
# Build mongo-cxx-driver
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DMONGOCXX_OVERIDE_DEFAULT_INSTALL_PREFIX=OFF \
         -DBoost_LIBRARY_DIR="$BOOST_LIBS" \
         -DBSONCXX_POLY_USE_BOOST=1 \
         -DBoost_INCLUDE_DIR="$BOOST_ROOT"
cmake --build . --target install
popd
