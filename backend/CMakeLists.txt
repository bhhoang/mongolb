cmake_minimum_required(VERSION 3.15)
project(mongo_app)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")  # Use -pthread for threading support

# Define the absolute path to the MongoDB C++ Driver and Boost based on the project source directory
set(MONGO_CXX_DRIVER "${CMAKE_SOURCE_DIR}/../lib/mongo-cxx-driver/build/install")
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../lib/boost")
set(NLOHMANN_JSON "${CMAKE_SOURCE_DIR}/../lib/json")
# Convert relative paths to absolute paths
get_filename_component(MONGO_CXX_DRIVER_ABSOLUTE ${MONGO_CXX_DRIVER} ABSOLUTE)
get_filename_component(BOOST_ROOT_ABSOLUTE ${BOOST_ROOT} ABSOLUTE)
get_filename_component(NLOHMANN_JSON_ABSOLUTE ${NLOHMANN_JSON} ABSOLUTE)
# Include directories for MongoDB C++ Driver
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/include/mongocxx/v_noabi)
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/include/bsoncxx/v_noabi)
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/include/libmongoc-1.0)
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/include/libbson-1.0)
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/lib)
include_directories(${MONGO_CXX_DRIVER_ABSOLUTE}/include)
# Include Boost directories
include_directories(${BOOST_ROOT_ABSOLUTE})
include_directories(${BOOST_ROOT_ABSOLUTE}/stage/lib)
# Include nlohmann json directories
include_directories(${NLOHMANN_JSON_ABSOLUTE})
# Path to the MongoDB C++ and BSON libraries
set(LIBMONGOCXX_LIBRARIES "${MONGO_CXX_DRIVER_ABSOLUTE}/lib/libmongocxx.so")
set(LIBBSONCXX_LIBRARIES "${MONGO_CXX_DRIVER_ABSOLUTE}/lib/libbsoncxx.so")

# Executable
add_executable(mongo_app search.cpp)

# Link libraries
target_link_libraries(mongo_app ${MONGO_CXX_DRIVER_ABSOLUTE}/lib/libmongoc-1.0.so)
target_link_libraries(mongo_app ${MONGO_CXX_DRIVER_ABSOLUTE}/lib/libbson-1.0.so)
target_link_libraries(mongo_app ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})

