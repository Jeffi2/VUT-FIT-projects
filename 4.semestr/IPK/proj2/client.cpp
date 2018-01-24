/** 
 *  @file client.cpp
 *
 *  @brief Chat client. (IPK proj2)
*
 *  @author Petr Juda (xjudap00)
 *
*/

#include <string>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <vector>
#include <csignal>
#include <errno.h>
#include <sstream>
using namespace std;

/**
 * @brief Global variables only for this file. (Static in memory)
 */
namespace {
    int clientSocket;
    string username;
    bool run = true;
    string lastMessage;
}

/**
 * @brief Handle exit. (kill -2 or ctrl+c)
 *
 * @param signum Signal number.
 *
 */
void signalHandler( int signum ) {
    run = false;
    size_t howMuchSend = send(clientSocket, lastMessage.data(), lastMessage.size(), 0);
    if (howMuchSend < lastMessage.size()) {
        cerr << "Problem with sending message!" << endl;
        terminate();
    }
}

/**
 * @brief Non-block console input.
 *
 * @return true when input ready.
 */
bool inputAvailable()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}

/**
 * @brief Thread for reading from stdin and sending to server.
 *
 */
void sendMessage() {
    unsigned int microseconds = 300000;
    string inputLine;
    signal(SIGINT, signalHandler);
    while (run) {
        string message = "";
        while (!inputAvailable() && run) {
            signal(SIGINT, signalHandler);
            continue;
        }
        if (run) {
            getline(std::cin, inputLine);
            if (inputLine.empty()) {
                continue;
            }
            message = username + ": " + inputLine + "\r\n";
            size_t howMuchSend = send(clientSocket, message.data(), message.size(), 0);
            if (howMuchSend < inputLine.size()) {
                cerr << "Problem with sending message!" << endl;
                terminate();
            }
        }
    }
}

/**
 * @brief Thread for recive messages and print to stdout.
 *
 */
void reciveMessage() {
    signal(SIGINT, signalHandler);
    int bytes = 0;
    char reciveBuffer[1024];
    while (run) {
        signal(SIGINT, signalHandler);
        if ((bytes = recv(clientSocket, reciveBuffer, 2048, 0)) > 0) {
            for (int i = 0; i < bytes; i++) {
                cout << reciveBuffer[i];
            }
            memset(reciveBuffer, 0, sizeof(reciveBuffer));
            bytes = 0;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            signal(SIGINT, signalHandler);
            if (!run)
                break;
            continue;
        }
        else {
            cerr << "Error: Can not recive data." << endl;
            terminate();
        }
    }
}

/**
 * @brief Control program flow, validate params and create threads.
 *
 * @param argc Number of command line params.
 * @param argv Array with command line params.
 *
 * @return 0 on succes
 */
int main(int argc, const char * argv[]) {
    // Arguments
    if (argc != 5) {
        cerr << "Error: Bad arguments." << endl;
        exit(1);
    }
    string ipaddr;
    if (strcmp(argv[1], "-i") == 0) {
        ipaddr = argv[2];
        if (strcmp(argv[3], "-u") == 0) {
            username = argv[4];
        } else {
            cerr << "Error: Bad arguments." << endl;
            exit(1);
        }
    }
    else
        if(strcmp(argv[1], "-u") == 0) {
            username = argv[2];
            if (strcmp(argv[3], "-i") == 0) {
                ipaddr = argv[4];
            } else {
                cerr << "Error: Bad arguments." << endl;
                exit(1);
            }
        } else {
            cerr << "Error: Bad arguments." << endl;
            exit(1);
        }
   
    // Connection
    int port = 21011;
    
    struct hostent *server;
    // Find server address trought DNS
    if ((server = gethostbyname(ipaddr.data())) == NULL) {
        cerr << "Error: Hostname not found." << endl;
        exit(2);
    }
    
    struct sockaddr_in serverAdress;
    memcpy(&serverAdress.sin_addr, server->h_addr_list[0], server->h_length); //clean
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port);
    
    // Create a socket
    
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error: Can not create socket." << endl;
        exit(2);
    }
    
    // Connect to server
    if (connect(clientSocket, (const struct sockaddr*) &serverAdress, sizeof(serverAdress)) != 0) {
        cerr << "Error: Can not connect to server." << endl;
        exit(2);
    }
    int rc = 0;
    int flags = fcntl(clientSocket, F_GETFL, 0);
    rc = fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);
    if (rc < 0) {
        cerr << "Error: Can not create non-block socket." << endl;
        exit(3);
    }
    
    string firstMessage = username + " logged in\r\n";
    lastMessage = username + " logged out\r\n";
    
    size_t howMuchSend = send(clientSocket, firstMessage.data(), firstMessage.size(), 0);
    if (howMuchSend < firstMessage.size()) {
        cerr << "Problem with sending message!" << endl;
        exit(4);
    }
    // Create thread for reading from stdout:
    thread t1(sendMessage);
    thread t2(reciveMessage);
    signal(SIGINT, signalHandler);
    
    // Wait for threads end
    t1.join();
    t2.join();
    close(clientSocket);
    return 0;
}


