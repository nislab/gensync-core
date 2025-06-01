cmake_minimum_required(VERSION 3.6)
project(gensync)

cmake_policy(SET CMP0077 NEW)

set(CMAKE_CXX_STANDARD 11)

# if compiled with RECORD, GenSync server and client produce extensive log files
# that can be used to reproduce exact sync behavior afterwards
set(RECORD_DIR ".gensync" CACHE STRING "location of record directory")

option(BUILD_TESTS "build tests" ON)
set(DEFAULT_LOG_LEVEL "TEST" CACHE STRING "default log level")

if(BUILD_TESTS)
    include(CTest)
endif()

include(GNUInstallDirs)

# Set project directory strucuture
set(SRC_DIR src)
set(INCLUDE_DIR include)
set(AUX_DIR ${SRC_DIR}/Aux)
set(DATA_DIR ${SRC_DIR}/Data)
set(COMM_DIR ${SRC_DIR}/Communicants)
set(SYNC_DIR ${SRC_DIR}/Syncs)
set(BENCH_DIR ${SRC_DIR}/Benchmarks)
set(TOOLS_DIR ${SRC_DIR}/Tools)

set(INCLUDE include/GenSync)
set(AUX_DIR_INC ${INCLUDE}/Aux)
set(DATA_DIR_INC ${INCLUDE}/Data)
set(COMM_DIR_INC ${INCLUDE}/Communicants)
set(SYNC_DIR_INC ${INCLUDE}/Syncs)
set(SYNC_BENCH_INC ${INCLUDE}/Benchmarks)

# Test directory structure
set(TEST_DIR tests)
set(UNIT_TEST_DIR ${TEST_DIR}/unit)
set(SYSLONG_TEST_DIR ${TEST_DIR}/sys/long)
set(BENCHMARK_TEST_DIR ${TEST_DIR}/sys/benchmark)

# Set location of the test runner
set(TEST_RUNNER ${TEST_DIR}/testRunner.cpp)

# Set file config
set(SOURCE_FILES

        ${AUX_DIR}/Logger.cpp
        ${AUX_DIR}/UID.cpp
        ${AUX_DIR}/SyncMethod.cpp
        ${AUX_DIR}/Sketches.cpp

        ${DATA_DIR}/DataObject.cpp

        ${COMM_DIR}/CommSocket.cpp
        ${COMM_DIR}/CommString.cpp
        ${COMM_DIR}/Communicant.cpp
        ${COMM_DIR}/CommDummy.cpp

        ${SYNC_DIR}/CPISync.cpp
        ${SYNC_DIR}/GenSync.cpp
        ${SYNC_DIR}/InterCPISync.cpp
        ${SYNC_DIR}/probCPISync.cpp
        ${SYNC_DIR}/HashSync.cpp
        ${SYNC_DIR}/GenIBLT.cpp
        ${SYNC_DIR}/IBLT.cpp
        ${SYNC_DIR}/IBLTMultiset.cpp
        ${SYNC_DIR}/IBLTSync.cpp
        ${SYNC_DIR}/IBLTSync_Multiset.cpp
        ${SYNC_DIR}/IBLTSync_Adaptive.cpp
        ${SYNC_DIR}/IBLTSetOfSets.cpp
        ${SYNC_DIR}/Compact2DBitArray.cpp
        ${SYNC_DIR}/Cuckoo.cpp
        ${SYNC_DIR}/CuckooSync.cpp
        ${SYNC_DIR}/FullSync.cpp
	    ${SYNC_DIR}/BloomFilterSync.cpp
	    ${SYNC_DIR}/BloomFilter.cpp
        ${SYNC_DIR}/MET_IBLTSync.cpp
        ${SYNC_DIR}/MET_IBLT.cpp

        ${BENCH_DIR}/BenchParams.cpp
        ${BENCH_DIR}/FromFileGen.cpp
    )

set(HEADERS

        ${AUX_DIR_INC}/Auxiliary.h
        ${AUX_DIR_INC}/ConstantsAndTypes.h
        ${AUX_DIR_INC}/Exceptions.h
        ${AUX_DIR_INC}/ForkHandle.h
        ${AUX_DIR_INC}/Logger.h
        ${AUX_DIR_INC}/SyncMethod.h
        ${AUX_DIR_INC}/UID.h
        ${AUX_DIR_INC}/Sketches.h

        ${DATA_DIR_INC}/DataFileC.h
        ${DATA_DIR_INC}/DataMemC.h
        ${DATA_DIR_INC}/DataObjC.h
        ${DATA_DIR_INC}/DataObject.h
        ${DATA_DIR_INC}/DataPriorityObject.h

        ${COMM_DIR_INC}/CommSocket.h
        ${COMM_DIR_INC}/CommString.h
        ${COMM_DIR_INC}/Communicant.h
        ${COMM_DIR_INC}/CommDummy.h

        ${SYNC_DIR_INC}/CPISync.h
        ${SYNC_DIR_INC}/CPISync_ExistingConnection.h
        ${SYNC_DIR_INC}/CPISync_HalfRound.h
        ${SYNC_DIR_INC}/CPISync_HalfRound_Hashed.h
        ${SYNC_DIR_INC}/CPISync_OneLessRound.h
        ${SYNC_DIR_INC}/FullSync.h
        ${SYNC_DIR_INC}/GenSync.h
        ${SYNC_DIR_INC}/HashSync.h
        ${SYNC_DIR_INC}/GenIBLT.h
        ${SYNC_DIR_INC}/IBLT.h
        ${SYNC_DIR_INC}/IBLTMultiset.h
        ${SYNC_DIR_INC}/IBLTSync.h
        ${SYNC_DIR_INC}/IBLTSetOfSets.h
        ${SYNC_DIR_INC}/IBLTSync_HalfRound.h
        ${SYNC_DIR_INC}/IBLTSync_Multiset.h
        ${SYNC_DIR_INC}/IBLTSync_Adaptive.h
        ${SYNC_DIR_INC}/Compact2DBitArray.h
        ${SYNC_DIR_INC}/Cuckoo.h
        ${SYNC_DIR_INC}/CuckooSync.h
        ${SYNC_DIR_INC}/InterCPISync.h
        ${SYNC_DIR_INC}/PrioCPISync.h
        ${SYNC_DIR_INC}/ProbCPISync.h
	    ${SYNC_DIR_INC}/BloomFilterSync.h
	    ${SYNC_DIR_INC}/BloomFilter.h
        ${SYNC_DIR_INC}/MET_IBLTSync.h
        ${SYNC_DIR_INC}/MET_IBLT.h

        ${SYNC_BENCH_INC}/BenchObserv.h
        ${SYNC_BENCH_INC}/BenchParams.h
        ${SYNC_BENCH_INC}/DataObjectGenerator.h
        ${SYNC_BENCH_INC}/RandGen.h
        ${SYNC_BENCH_INC}/FromFileGen.h
    )

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(NOT DEFINED includedir)
        set(includedir "/usr/local")
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    if(NOT DEFINED includedir)
        set(includedir "/opt/local")
    endif()
endif()

# Users may need to update these paths
include_directories(${includedir}/include)
link_directories(${includedir}/lib)
link_directories(${includedir}/lib64)


include_directories(${CMAKE_INSTALL_INCLUDEDIR})
link_directories(${CMAKE_INSTALL_LIBDIR})

# Add gensync library
add_library(gensync STATIC ${SOURCE_FILES} ${HEADERS})
target_include_directories(gensync PUBLIC ${INCLUDE_DIR})
target_compile_definitions(gensync PUBLIC RECORD="${RECORD_DIR}")
target_compile_definitions(gensync PUBLIC DEFAULT_LOGLEVEL=${DEFAULT_LOG_LEVEL})

# include Apache Data Sketches as a header-only library
target_link_libraries(gensync PUBLIC ntl pthread gmp)
target_include_directories(gensync PUBLIC
                    incubator-datasketches-cpp/common/include
                    incubator-datasketches-cpp/hll/include
                    incubator-datasketches-cpp/fi/include
    )

install(CODE "FILE(REMOVE_RECURSE ${CMAKE_INSTALL_INCLUDEDIR}/gensync)") #Remove the existing GenSync library data and reinstall
install(CODE "FILE(REMOVE_RECURSE ${CMAKE_INSTALL_LIBDIR}/libgensync.a)") #Remove the existing GenSync library data and reinstall
install(TARGETS gensync LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GenSync) #create a new folder for GenSync inside of the system include directory
install(DIRECTORY ${AUX_DIR_INC} ${DATA_DIR_INC} ${COMM_DIR_INC} ${SYNC_DIR_INC} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GenSync COMPONENT devel) #copy the contents of the include folder into the system include directory

# Add the TryMe executable
add_executable(TryMe ${SRC_DIR}/TryMe.cpp)
target_link_libraries(TryMe gensync)

add_executable(TryMe2 ${SRC_DIR}/TryMe2.cpp)
target_link_libraries(TryMe2 gensync)

# for adaptive-iblt testing
add_executable(test_adaptive ${SRC_DIR}/test_adaptive.cpp)
target_link_libraries(test_adaptive gensync)

# Benchmarks
add_executable(Benchmarks ${BENCH_DIR}/Runner.cpp)
target_link_libraries(Benchmarks gensync)

# Tools
add_executable(EncodeJoin ${TOOLS_DIR}/EncodeJoin.cpp)
target_link_libraries(EncodeJoin gensync)

# Define a macro for adding executables testing multiple files
# @param dir The relative path to the folder containing test files to add
# @param name The executable name
macro(add_group_test dir name)
    file(GLOB testPaths ${dir}/*Test.cpp ${dir}/*Tests.cpp)

    # add one executable with all tests
    add_executable(${name} ${TEST_RUNNER} ${testPaths})
    target_link_libraries(${name} gensync cppunit)
    target_include_directories(${name} PRIVATE tests)

    # add an executable for each test file and register it
    foreach(test ${testPaths})
        get_filename_component(testName ${test} NAME_WE)
        add_executable(${testName} ${TEST_RUNNER} ${test})
        target_link_libraries(${testName} gensync cppunit)
        target_include_directories(${testName} PRIVATE tests)
        add_test(${testName} ${testName})
    endforeach(test)
endmacro()

if(BUILD_TESTS)
    # Add test groups (note: executable will throw errors if there are no tests in the respective folder)
    add_group_test(${UNIT_TEST_DIR} UnitTest)
    # #add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
    add_group_test(${SYSLONG_TEST_DIR} SystemLongTest)
    # #add_group_test(${SYSSHORT_TEST_DIR} SystemShortTests)
    add_group_test(${BENCHMARK_TEST_DIR} Benchmark)
endif()

#Packaging
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A library for remote file synchronization research")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README.md)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_NAME "GenSync")
set(CPACK_PACKAGE_VERSION_MAJOR "2")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "3")
set(CPACK_PACKAGE_CONTACT "Ari Trachtenberg")
set(CPAKC_PACKAGE_VENDOR "NISLAB")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST /usr/local /usr/local/lib64 /usr/local/lib /usr/local/include)
include(CPack)