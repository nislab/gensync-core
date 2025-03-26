/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/*
 * File:   DataTestRunner.cpp
 * Author: kaets
 *
 * Created on May 15, 2018, 10:33:52 AM
 */

// CppUnit site http://sourceforge.net/projects/cppunit/files

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestFailure.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <GenSync/Auxiliary/Auxiliary.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/CPISync.h>
#include <GenSync/Syncs/IBLTSync.h>
#include <GenSync/Syncs/IBLTSync_HalfRound.h>
#include <GenSync/Syncs/ProbCPISync.h>
#include <GenSync/Syncs/CPISync_HalfRound.h>
#include <GenSync/Syncs/InterCPISync.h>
#include <GenSync/Communicants/CommString.h>
#include <GenSync/Communicants/CommSocket.h>
#include <GenSync/Auxiliary/Auxiliary.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/FullSync.h>
#include <GenSync/Auxiliary/ForkHandle.h>
#include <type_traits>
#include <getopt.h>
#include <chrono>
#include "TestAuxiliary.h"

using namespace std;

// global variables and structs
static int help_flag = 0; /** Assume no help requested. */

// ... Communication mode
static const int CLIENT = 0; /** I am a client connecting to a server. */
static const int SERVER = 1; /** I am a server listening for a client. */
static int comm_flag = CLIENT;

// ... Synchronization mode
static int sync_flag = static_cast<int>(GenSync::SyncProtocol::InteractiveCPISync); /** Defaults to InterCPISync. */
static int nohash_flag = 0; /** Defaults to 0 = allow hashing. */
static int int_flag = 0;    /** Defaults to 0 = do *not* parse string data as big Integers. */
static int numElems = 1;    /** Defaults to 1 = at most 1 element is expected in the data structure. */



// Test runner functions
class ProgressListener : public CPPUNIT_NS::TestListener {
  public:

    ProgressListener()
        : m_lastTestFailed(false) {
    }

    ~ProgressListener() override = default;

    void startTest(CPPUNIT_NS::Test *test) override {
        CPPUNIT_NS::stdCOut() << test->getName();
        CPPUNIT_NS::stdCOut() << "\n";
        CPPUNIT_NS::stdCOut().flush();

        m_lastTestFailed = false;
    }

    void addFailure(const CPPUNIT_NS::TestFailure &failure) override {
        CPPUNIT_NS::stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
        m_lastTestFailed = true;
    }

    void endTest(CPPUNIT_NS::Test *test) override {
        if (!m_lastTestFailed)
            CPPUNIT_NS::stdCOut() << " : OK";
        CPPUNIT_NS::stdCOut() << "\n";
    }

    /// Prevents the use of the copy constructor.
    ProgressListener(const ProgressListener &copy) = delete;

    /// Prevents the use of the copy operator.
    void operator=(const ProgressListener &copy) = delete;

  private:
    bool m_lastTestFailed;
};


// Command-line options
static struct option long_options[] = {
    // Comm mode
    {"client", no_argument, &comm_flag,   CLIENT},
    {"server", no_argument, &comm_flag,   SERVER},

    // Sync mode
    {"cpi", no_argument, &sync_flag,      static_cast<int>(GenSync::SyncProtocol::CPISync)},
    {"oneway", no_argument, &sync_flag,   static_cast<int>(GenSync::SyncProtocol::OneWayCPISync)},
    {"prob", no_argument, &sync_flag,     static_cast<int>(GenSync::SyncProtocol::ProbCPISync)},
    {"inter", no_argument, &sync_flag,    static_cast<int>(GenSync::SyncProtocol::InteractiveCPISync)},
    {"iblt", no_argument, &sync_flag,     static_cast<int>(GenSync::SyncProtocol::IBLTSync)},
    {"nohash", no_argument, &nohash_flag, 1}, // if set, then no hashing is done to reduce the element size to a fixed bitlength
    {"integer", no_argument, &int_flag,   1},   // if set, then all set element strings are parsed as arbitrarily-sized Integers

    {"file", required_argument, nullptr,        'f'}, // a file from which to get elements to be synced
    {"reconciled", required_argument, nullptr, 'r'},  // expected reconciled result
    {"string", required_argument, nullptr,      's'}, // a string from which to get a sync hash to which to synchronize
    {"host", required_argument, nullptr,        'h'}, // a host to which to synchronize
    {"port", required_argument, nullptr, 'o'}, // a port to which to synchronize or on which to list for other synchronizing clients

    // sync parameters
    {"perr", required_argument, nullptr, 'e'}, // maximum probability of error allowed, for sync algorithms that take this as a parameter
    {"mbar", required_argument, nullptr, 'm'}, // maximum number of differences in an atomic sync operation
    {"bits", required_argument, nullptr, 'b'}, // number of bits used to represent each set element internally
    {"partition", required_argument, nullptr, 'p'}, // the number of partitions in interactive GenSync
    {"numElems", required_argument, nullptr, 'n'},  // maximum number of elements expected in the sync object (esp. for IBLT)

    // other
    {"help", no_argument, &help_flag, 1} // get help
};

/**
 * Display usage
 * @param str The command used to run this program
 */
string display_usage(char *progName) {
    stringstream strstr;
    strstr << "Usage:  " << progName
           << "   [(-f | --file) <data filename>]" << endl
           << "   [(-r | --reconciled) <expected reconciled set file name>" << endl
           << "   [(-s | --string) <string>]" << endl
           << "   [(-h | --host) <hostname>]" << endl
           << "   [(-o | --port] <port number>]" << endl
           << "   [(-e | --perr) <max error prob> " << endl
           << "   [(-m | --mbar) <int>" << endl
           << "   [(-b | --bits) <int>" << endl
           << "   [(-p | --partition) <int>" << endl
           << "   [(-n | --numElems) <int>" << endl
           << "   [--cpi | --oneway | --prob | --inter | --iblt]" << endl
           << "   [--nohash]" << endl
           << "   [--integer]" << endl
           << "   [--client | --server]" << endl
           << "   [--help]" << endl
           << endl;

    return strstr.str();
}

// Set up this executable as a server for Gensync tests
void SetupServer(int argc, char **argv) {
    // ... declarations
    string fileName; // the file to tie to the synchronization
    string reconciledName; // the file from which expected reconciled data will be taken
    int choice;
    struct timespec timerStart, timerEnd;
    multiset<string> reconciled;

    // ... default values
    list<shared_ptr<DataObject>> data;
    string cStr;           // initially unused
    string host;           // for Communication Sockets
    int port = 10000;      // default port for Communication Sockets
    double perr = 0.00005; // max error probability
    long mbar = 5;
    long bits = 32;
    long pFactor = 2;
    // the maximum number of elements expected in the data structure
    bool dataInFile = false;
    forkHandleReport clientReport, serverReport;
    int option_index = 0;
    int method_num = 0;

    // 0. Parse the command-line options
    do {
        choice = getopt_long(argc, argv, "f:r:s:h:o:p:e:m:b:n:?", long_options,
                             &option_index);
        switch (choice) {
        case -1:
        case 0:
            // long option without a short version
            // ... so far, nothing to do here
            break;
        case 'f': { // read data from a file
            dataInFile = true;
            fileName = optarg;
            string str;
            break;
        }
        case 'r': { // read expected data from file
            reconciledName = optarg;
            std::ifstream file("reconciledName");
            if (!file) {
                std::cerr << "Unable to open file 'reconciledName'.\n";
                exit(-1);
            }
            string line;
            while (getline(file, line)) {
                reconciled.insert(line);
            }
            file.close();
            break;
        }
        case 'h':
            host = optarg;
            break;

        case 'o':
            port = atoi(optarg);
            break;

        case 'p':
            pFactor = atoi(optarg);
            break;

        case 'e':
            perr = atof(optarg);
            break;

        case 'm':
            mbar = atol(optarg);
            break;

        case 'n':
        case 'b':
            bits = atol(optarg);
            break;

        case '?':
            Logger::error_and_quit(display_usage(argv[0]));
            break;
        }
    } while (choice != -1);

    // 1. Command-line post-processing
    if (help_flag == 1) // user requested help?
        Logger::error_and_quit(display_usage(argv[0]));

    // ... if some arguments were not processed ... call an error and halt
    if (optind < argc) {
        string args = "Unprocessed arguments: ";
        while (optind < argc)
            args += argv[optind++] + string(" ");
        Logger::error_and_quit(args + "\n" + display_usage(argv[0]));
    }
    // ... check for command-line conflicts
    if (!cStr.empty() && (!host.empty()))
        Logger::error_and_quit(
            "Cannot specify both sync hash string AND host. " +
            display_usage(argv[0]));

    // 2. Set up the sync
    // .... sync protocol
    GenSync::SyncProtocol proto;
    switch (static_cast<GenSync::SyncProtocol>(sync_flag)) {
    case GenSync::SyncProtocol::CPISync:
        if (nohash_flag)
            proto = GenSync::SyncProtocol::CPISync_OneLessRound;
        else
            proto = GenSync::SyncProtocol::CPISync;
        break;
    case GenSync::SyncProtocol::OneWayCPISync:
        proto = GenSync::SyncProtocol::CPISync_HalfRound;
        break;
    case GenSync::SyncProtocol::ProbCPISync:
        proto = GenSync::SyncProtocol::ProbCPISync;
        break;
    case GenSync::SyncProtocol::InteractiveCPISync:
        proto = GenSync::SyncProtocol::InteractiveCPISync;
        break;
    case GenSync::SyncProtocol::IBLTSync:
        proto = GenSync::SyncProtocol::OneWayIBLTSync;
        break;
    default:
        Logger::error_and_quit("Sync protocol not recognized: " +
                               toStr(sync_flag));
    }

    // ... communicants
    GenSync::SyncComm comm;
    if (!cStr.empty())                    // are we syncing with a string
        comm = GenSync::SyncComm::string; // cStr contains the string
    else                                  // we are syncing with a socket
        comm = GenSync::SyncComm::socket;

    int negLogPerr = static_cast<int>(
        -log(perr) / log(2)); // the negative log of perr ... this is how the constructors expect their error value

    // 3. Deploy the sync
    GenSync GenSyncServer = GenSync::Builder()
                                .setProtocol(proto)
                                .setComm(comm)
                                .setIoStr(cStr)
                                .setPort(port)
                                .setErr(negLogPerr)
                                .setMbar(mbar)
                                .setBits(bits)
                                .setNumPartitions(pFactor)
                                .setDataFile(fileName)
                                .build();

    // start up server
    high_resolution_clock::time_point start = high_resolution_clock::now();

    bool syncSuccess;
    Logger::gLog(Logger::METHOD, "\n[SERVER]");
    syncSuccess = GenSyncServer.serverSyncBegin(0);

    // 4. Results
    serverReport.totalTime =
        duration_cast<microseconds>(high_resolution_clock::now() - start)
            .count() *
        1e-6;
    serverReport.CPUtime = GenSyncServer.getCommTime(
        method_num); /// assuming method_num'th communicator corresponds to method_num'th syncagent
    serverReport.bytes = GenSyncServer.getXmitBytes(method_num) +
                         GenSyncServer.getRecvBytes(method_num);
    Logger::gLog(Logger::COMM, "exit a child process, server, status: " +
                                   toStr(serverReport.success) +
                                   ", pid: " + toStr(getpid()));

    multiset<string> resultantServer;
    for (const auto &elem : GenSyncServer.dumpElements()) {
        resultantServer.insert(elem);
    }
    bool serverSuccess = checkServerSucceeded(resultantServer, reconciled, false, sync_flag==static_cast<int>(GenSync::SyncProtocol::OneWayCPISync) , serverReport);
    Logger::gLog(Logger::COMM,
                 "server, status: " + toStr(serverReport.success) +
                     ", check success: " + toStr(serverSuccess) +
                     ", pid: " + toStr(getpid()));

    exit(serverSuccess);
}

/**
 * Without parameters, this code simply goes through various tests.
 * With parameters, this code starts up a server based on the GenSync provided in the parameters.
 */
int main(int argc, char **argv) {
    if (argc>1) {
        // in this mode, the test runner is running a server based on some gensync variant
        SetupServer(argc, argv);
        exit(0);
    }

    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that collects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    ProgressListener progress;
    controller.addListener(&progress);

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
    outputter.write();

    return result.wasSuccessful() ? 0 : 1;
}