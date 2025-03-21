/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * File:   CommString.cpp
 * Created on November 5, 2011, 8:49 PM
 */

#include <chrono>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Communicants/CommString.h>

CommString::CommString(const string& initial, bool base64) {
    if (base64)
        stream = new stringstream(base64_decode(initial), stringstream::out | stringstream::in);
    else
        stream = new stringstream(initial, stringstream::out | stringstream::in);
}

CommString::~CommString() {
    delete stream;
}

string CommString::getString() {
    return stream->str();
}

void CommString::commConnect() {
} // nothing needed to connect to a string

void CommString::commListen() {
}// nothing needed to listen from a string

void CommString::commClose() {
	stream->flush();
}

void CommString::commSend(const char *toSend, size_t numBytes) {
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    // save the next bytes to the string stream
    stream->write(toSend, numBytes);
    addXmitBytes(numBytes); // update the byte transfer counter
}

string CommString::commRecv(unsigned long numBytes) {
    // returns the next few bytes from the string stream
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    auto *tmpBuf = new char[numBytes]; // buffer into which received bytes are placed
    stream->read(tmpBuf, numBytes);

    addRecvBytes(numBytes); // update the received byte counter

    return string(tmpBuf, numBytes);
}
