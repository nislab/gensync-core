/* This code is part of the GenSync project developed at Boston University. Please see the README for use and references. */

#include <sys/socket.h>
#include <sstream>
#include <thread>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Communicants/CommSocketUDP.h>

CommSocketUDP::CommSocketUDP() = default;

CommSocketUDP::CommSocketUDP(int port, string host) : Communicant() {
    remoteHost = std::move(host);
    remotePort = port;

    Logger::gLog(Logger::METHOD, string("Setting up host ") + toStr(remoteHost) + " on port " + toStr(remotePort));
}

CommSocketUDP::~CommSocketUDP() {
    CommSocketUDP::commClose();  // make sure that the socket has been closed
}

void CommSocketUDP::commListen() {
    state = Listening;

    my_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_fd == -1) {
        Logger::error_and_quit("UDP socket creation failed");
    }

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(remotePort);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(my_fd, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        Logger::error_and_quit("Could not bind UDP socket");
    }

    resetCommCounters();
    Logger::gLog(Logger::METHOD, "Listening for UDP on port " + toStr(remotePort));
}

void CommSocketUDP::commConnect() {
    state = Connecting;

    my_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_fd == -1) {
        Logger::error_and_quit("UDP socket creation failed");
    }

    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(remotePort);

    if (!remoteHost.empty()) {
        struct hostent *he = gethostbyname(remoteHost.c_str());
        if (!he || !he->h_addr_list) {
            Logger::error_and_quit("Failed to resolve hostname: " + remoteHost);
        }
        memcpy(&remoteAddr.sin_addr, he->h_addr_list[0], he->h_length);
    } else {
        remoteAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // default to localhost
    }

    resetCommCounters();
    Logger::gLog(Logger::METHOD, "UDP target set to " + toStr(remoteHost) + ":" + toStr(remotePort));
}

void CommSocketUDP::commClose() {
    if (my_fd == -1) {
        Logger::gLog(Logger::METHOD, "Attempted to close an unused socket");
    } else {
        close(my_fd);
        my_fd = -1;
        Logger::gLog(Logger::COMM_DETAILS, "UDP SOCKET CLOSED");
    }
}

void CommSocketUDP::commSend(const char* toSend, size_t len) {
    if (my_fd == -1)
        Logger::error_and_quit("Socket not open");

    Logger::gLog(Logger::COMM_DETAILS, "<UDP SEND> " + toStr(len) + " bytes (base64): " +
        base64_encode(toSend, len));

    ssize_t sent = sendto(my_fd, toSend, len, 0, 
                          (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
    if (sent != (ssize_t)len) {
        Logger::error_and_quit("UDP sendto failed");
    }

    addXmitBytes(sent);
}

std::string CommSocketUDP::commRecv(unsigned long numBytes) {
    if (my_fd == -1)
        Logger::error_and_quit("Socket not open");

    char* buffer = new char[numBytes];
    struct sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    ssize_t recvLen = recvfrom(my_fd, buffer, numBytes, 0,
                                (struct sockaddr*)&senderAddr, &senderLen);

    if (recvLen < 0) {
        delete[] buffer;
        Logger::error_and_quit("UDP recvfrom failed");
    }

    if (state == Listening) {
        this->remoteAddr = senderAddr;

        // Optional: Log the client's address for debugging
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, client_ip, INET_ADDRSTRLEN);
        Logger::gLog(Logger::METHOD, "Received from and set reply target to " + string(client_ip) + ":" + to_string(ntohs(senderAddr.sin_port)));
    }

    addRecvBytes(recvLen);
    Logger::gLog(Logger::COMM_DETAILS, "<UDP RECV> " + toStr(recvLen) + " bytes (base64): " +
        base64_encode(buffer, recvLen));

    std::string result(buffer, recvLen);
    delete[] buffer;
    return result;
}

