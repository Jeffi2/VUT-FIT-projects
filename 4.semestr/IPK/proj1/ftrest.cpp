#include <iostream>
#include <string>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <cstring>
#include <limits.h>

#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

/**
 * @file frest.cpp
 *
 * @brief Client for comunication with REST API server
 *
 * @autor xjudap00
 */

/**
 * @brief Response message parser
 *
 * @param message Recived message from server.
 * @param returnCode Code from server.
 * @param downloadSize Size of content in http header.
 * @param endOfHead Index where http header ends.
 *
 * @return response body
 */
string parseReturnMessage(string message, int *returnCode, int* downloadSize, int* endOfHead) {
    string body;
    string textReturnCode;
    string contentLength;
    string download;
    
    *endOfHead = message.find("\r\n\r\n")+4;
    //Remove http1.1
    message = message.substr(9);
    textReturnCode = message.substr(0, 3);
    *returnCode = stoi(textReturnCode);
    
    //Get content-type
    string contentTypeTmp = message.substr(message.find("Content-Type: ")+14);
    size_t lineEndType = contentTypeTmp.find("\r\n");
    string contentType = contentTypeTmp.substr(0, lineEndType);
    
    //Get content-length
    string messageLenTmp = message.substr(message.find("Content-Length: ")+16);
    size_t lineEnd = messageLenTmp.find("\r\n");
    if (lineEnd == string::npos) {
        cerr << "Error: Bad response head from server." << endl;
        exit(10);
    }
    contentLength = messageLenTmp.substr(0, lineEnd);
    *downloadSize = stoi(contentLength);
    //Find end head position
    size_t endHeader = message.find("\r\n\r\n");
    if (endHeader == string::npos) {
        cerr << "Error: Bad response head from server." << endl;
        exit(10);
    }
    //Rest of message
    body = message.substr(endHeader+4, endHeader + (stoi(contentLength)));

    if (strcmp(contentType.data(), "application/json") == 0) {
        body = body.substr(13);
        body = body.substr(0, body.size()-2);
    }
    
    return body;
}

/**
 * @brief Generate actual timestamp for http header.
 *
 *
 * @return actual timestamp
 */
string getTimeStamp () {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    strftime (buffer,80,"%a,%e %b %Y %X %Z",timeinfo);
    return buffer;
}

/**
 * @brief Create http request
 *
 * @param httpType Type of http request.
 * @param remotePath Path to folder or file from user.
 * @param fileOrDir Puts "file" or "dir" to request.
 * @param hostname Name or ip of the server.
 * @param contentLength Size of request body.
 * @param accept Accepted response type.
 *
 * @return Created http REST header.
 */
string createRestHeader(string httpType, string remotePath, string fileOrDir, string hostname, string contentType, size_t contentLength, string accept) {
    string header;
    stringstream headBuff;
    headBuff << httpType << " " << remotePath << "?type=" << fileOrDir << " HTTP/1.1\r\n"
    << "Host: " << hostname << "\r\n"
    << "Date: " << getTimeStamp() << "\r\n"
    << "Accept: " << accept << "\r\n" //TODO: switch to json
    << "Accept-Encoding: " << "identity" << "\r\n"
    << "Content-Type: " << contentType << "\r\n"
    << "Content-Length: " << contentLength << "\r\n" //TODO: switch to body length
    << "\r\n";
    
    header = headBuff.str();
    
    return header;
}

/**
 * @brief Check if path is valid dir.
 *
 * @param path Path to dir/file.
 *
 * @return 0 if true
 */
int isDir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

/**
 * @brief Check if path is valid file.
 *
 * @param path Path to dir/file.
 *
 * @return 0 if true
 */
int isFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/**
 * @brief Check if file or dir really exists.
 *
 * @param path Path to dir/file.
 *
 * @return true if exists.
 */
bool fileOrDirExist (const char* path) {
    if (ifstream(path))
    {
        return true;
    }
    return false;
}

/**
 * @brief Check local file for upload.
 *
 * @param pathToFile Path to local file.
 *
 */
void checkLocalFile (string pathToFile) {
    char absolutPath[PATH_MAX + 1];
    char *testPath = realpath(pathToFile.c_str(), absolutPath);
    if (!testPath) {
        cerr << "Error: Bad path to local file." << endl;
        exit(9);
    }
    
    if (fileOrDirExist(absolutPath) == false) {
        cerr << "Error: Bad path to local file." << endl;
        exit(9);
    }
    
    if ((isFile(absolutPath)) == 0) {
        cerr << "Error: Bad local path. Put file." << endl;
        exit(9);
    }
    
}

/**
 * @brief Recive response from server processor.
 *
 * @param clientSocket Connected socket.
 * @param downloadSize Size of response body.
 * @param responseCode Return code from server.
 * @param recivedBytes Specify how much data was recived.
 * @param endOfHead Index where http header ends.
 *
 * @return Response body.
 */
string reciveMessage(int clientSocket, int* downloadSize, int* responseCode, int *recivedBytes, int* endOfHead) {
    char firstResponse[1024];
    memset(firstResponse, 0, sizeof(firstResponse));
    *recivedBytes = recv(clientSocket, firstResponse, 1024, 0);
    int codeFromServer;
    int size;
    int endOfHeadDebug;
    //Parse header
    string body = parseReturnMessage(firstResponse, &codeFromServer, &size, &endOfHeadDebug);
    *endOfHead = endOfHeadDebug;
    *responseCode = codeFromServer;
    *downloadSize = size;
    return body;
}

/**
 * @brief Upload file process.
 *
 * @param returnCode Response code.
 * @param remotePath Path from user.
 * @param hostname Name or IP of the server.
 * @param clientSocket Connected socket.
 * @param localPath Path to local file.
 *
 * @return 0 on success.
 */
int processPut (int* returnCode, string remotePath, string hostname, int clientSocket, string localPath) {
    checkLocalFile(localPath);
    ifstream openedFile;
    //Open file for read
    openedFile.open(localPath, ios::in | ios::binary | ios::ate);
    if (!openedFile.is_open()) {
        close(clientSocket);
        cerr << "Error: Can not open file." << endl;
        exit(3);
    }
    //Get file size
    size_t uploadFileSize = openedFile.tellg();
    openedFile.seekg(0, ios::beg);
    
    //Send request head
    string informHeader = createRestHeader("PUT", remotePath, "file", hostname, "text/plain", uploadFileSize, "application/json");//change text type
    int howMuchSend = send(clientSocket, informHeader.data(), informHeader.size(), 0);
    
    //Send data
    int loadFromFile = 1;
    char sendBuffer[1024];
    while (loadFromFile > 0) {
        openedFile.read(sendBuffer, 1024);
        loadFromFile = openedFile.gcount();
        if (send(clientSocket, sendBuffer, loadFromFile, 0) < 0) {
            openedFile.close();
            close(clientSocket);
            cerr << "Error during sending to server." << endl;
            exit(3);
        }
        memset(sendBuffer, 0, sizeof(sendBuffer));
    }
    openedFile.close();
    
    //Parse response
    int downloadSize;
    int responseCode;
    int endOfHead;
    int recivedBytes;
    //Recive message back
    string body = reciveMessage(clientSocket, &downloadSize, &responseCode, &recivedBytes, &endOfHead);
    //Response message
    if (responseCode > 200) {
        cerr << body << endl;
        close(clientSocket);
        exit(3);
    }
    
    //cout << body << endl;
    return 0;
}

/**
 * @brief Download file process.
 *
 * @param returnCode Response code.
 * @param remotePath Path from user.
 * @param hostname Name or IP of the server.
 * @param clientSocket Connected socket.
 * @param localPath Path to local file.
 * @param localPathSet False if default path is used.
 *
 * @return 0 on success.
 */
int processGet(int* returnCode, string remotePath, string hostname, int clientSocket, string localPath, bool localPathSet) {
    string filename;
    //Check if i want a file
    if (strcmp(&remotePath.back(), "/") != 0) {
        size_t slashPossition = remotePath.find_last_of("/");
        filename = remotePath.substr(slashPossition+1);
    }
    
    
    //Check if local path is dir
    if (localPathSet) {
        if (strcmp(&localPath.back(), "/") == 0) {
            // dir here:
            if (!fileOrDirExist(localPath.data())){
                close(clientSocket);
                cerr << "Error: Bad local path." << endl;
                exit(4);
            }
            localPath += filename;
        }
        else {
            if (isDir(localPath.data())) {
                close(clientSocket);
                cerr << "Error: Can not save file here. Same name as dir." << endl;
                exit(4);
            }
        }
    }
    
    //Send request
    string informHeader = createRestHeader("GET", remotePath, "file", hostname, "text/plain", 0, "application/octet-stream");//change text type
    int howMuchSend = send(clientSocket, informHeader.data(), informHeader.size(), 0);
    
    //Recive header and data
    int reciveBytes;
    char recivedMessageBuff[1024];
    memset(recivedMessageBuff, 0, sizeof(recivedMessageBuff));
    reciveBytes = recv(clientSocket, recivedMessageBuff, 1024, 0);
    
    string recivedMessage = recivedMessageBuff;

    int codeFromServer;
    int contentSize = 0;
    int endOfHead;
    //Parse header
    string body = parseReturnMessage(recivedMessage, &codeFromServer, &contentSize, &endOfHead);
    
    //If header containe some file data catch them
    int recived = 0;
    vector<unsigned char> httpBody;
    if (reciveBytes > endOfHead) {
        if (contentSize > 0 ) {
            if (contentSize >= 1024-endOfHead) {
                for (int i = (int)endOfHead; i < 1024; i++) {
                    httpBody.push_back(recivedMessageBuff[i]);
                    recived++;
                }
            }
            else {
                for (int i = (int)endOfHead; i <endOfHead+contentSize ; i++) {
                    httpBody.push_back(recivedMessageBuff[i]);
                    recived++;
                }
            }
        }
    }
    
    //Recive all data
    int bytes = 1;
    char reciveBuffer[1024];
    while (recived < contentSize) {
        if ((bytes = recv(clientSocket, reciveBuffer, 1024, 0)) < 0) {
            close(clientSocket);
            cerr << "Error: Error on reciving data." << endl;
            exit(4);
        }
        if (bytes > 0) {
            httpBody.insert(httpBody.end(), reciveBuffer, reciveBuffer+1024);
            memset(reciveBuffer, 0, sizeof(reciveBuffer));
        }
        recived += bytes;
    }
    //Create valid data vector
    vector<unsigned char>::const_iterator first = httpBody.begin();
    vector<unsigned char>::const_iterator last = httpBody.begin() + contentSize;
    vector<unsigned char> fileData(first, last);
    
    
    if (recived != fileData.size()) {
        close(clientSocket);
        cerr << "Error: File download failed." << endl;
        exit(4);
    }
    //Check response
    if (codeFromServer > 200) {
        cerr << body << endl;
        close(clientSocket);
        exit(4);
    }

    //Ok create file
    ofstream file;
    file.open(localPath, ios::out | ios::binary);
    if (!file) {
        close(clientSocket);
        cerr << "Error: No perrmission." << endl;
        exit(4);
    }
    //Write data
    file.write((const char*)&fileData[0], fileData.size());
    file.close();
    return 0;
}

/**
 * @brief Other http comunication.
 *
 * @param operation Specify which http request type should be send.
 * @param remotePath Path from user.
 * @param hostname Name or IP of the server.
 * @param clientSocket Connected socket.
 *
 * @return 0 on success.
 */
int processUniverzal(int operation, string remotePath, string hostname, int clientSocket) {
    string informHeader;
    switch (operation) {
        case 5:
            informHeader = createRestHeader("DELETE", remotePath, "file", hostname, "text/plain", 0, "application/json");
            break;
        case 6:
            informHeader = createRestHeader("GET", remotePath, "dir", hostname, "text/plain", 0, "application/json");
            break;
        case 7:
            informHeader = createRestHeader("PUT", remotePath, "dir", hostname, "text/plain", 0, "application/json");
            break;
        case 8:
            informHeader = createRestHeader("DELETE", remotePath, "dir", hostname, "text/plain", 0, "application/json");
            break;
            
        default:
            break;
    }
   
    //send header
    int howMuchSend = send(clientSocket, informHeader.data(), informHeader.size(), 0);
    
    int sizeDownload;
    int responseCode;
    int endOfHead;
    int recivedBytes;
    string body = reciveMessage(clientSocket, &sizeDownload, &responseCode, &recivedBytes, &endOfHead);
    if (responseCode > 200) {
        cerr << body << endl;
        close(clientSocket);
        exit(operation);
    }
    
    if (operation == 6) {
        //Lst output
        if (!body.empty()) {
            cout << body << endl;
        }
    }
    
    return 0;
}

/**
 * @brief Control flow, validate program params and sets socket comunication.
 *
 * @param argc Number of program params.
 * @param argv Array with program params.
 *
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    // ------------- PARAMS VALIDATION START ------//
    if (argc > 4 || argc < 3) {
        exit(1);
    }
    string localPath;
    string command = argv[1];
    string remotePath = argv[2];
    
    //if patch contein %20 change it to space
    while (remotePath.find("%20") != string::npos) {
        remotePath = remotePath.replace(remotePath.find("%20"), 3, " ");
    }
    
    localPath = "./";
    bool localPathSet = false;
    if (argc == 4) {
        localPath = argv[3];
        localPathSet = true;
    }
    
    if (strcmp(argv[1], "put") == 0 && argc != 4) {
        cerr << "Error: Bad arguments." << endl;
        exit(1);
    }
    //Flafs for each request
    bool put = false;
    bool get = false;
    bool del = false;
    bool lst = false;
    bool mkd = false;
    bool rmd = false;
    
    if (strcmp(argv[1], "put") == 0) {
        put = true;
    }
    else if (strcmp(argv[1], "get") == 0) {
        get = true;
    }
    else if (strcmp(argv[1], "del") == 0) {
        del = true;
    }
    else if (strcmp(argv[1], "lst") == 0) {
        lst = true;
    }
    else if (strcmp(argv[1], "mkd") == 0) {
        mkd = true;
    }
    else if (strcmp(argv[1], "rmd") == 0) {
        rmd = true;
    }
    else {
        cerr << "Error: Bad arguments. Use: put, get, del, lst, mkd or rmd." << endl;
        exit(1);
    }
    //------------- PARAMS VALIDATION END --------------//
    
    //Parse remote path for hostname an operations
    string protocolTest = remotePath.substr(0, 7);
    if (protocolTest != "http://") {
        perror("Bad URI protocol set! Use http://\n");
        exit(1);
    }
    //Path without protocol
    string remotePathNoProtocol = remotePath.substr(7);
    
    int port = 6677;
    string hostName;
    //Try to find :
    size_t doubleDotPossiton = remotePathNoProtocol.find(":");
    if (doubleDotPossiton != string::npos) {
        //Port is set
        //There is hostname
        hostName = remotePathNoProtocol.substr(0, doubleDotPossiton);
        remotePathNoProtocol = remotePathNoProtocol.substr(doubleDotPossiton+1);
        
        //Get port
        size_t slashPossition = remotePathNoProtocol.find("/");
        if (slashPossition == string::npos) {
            cerr << "Error: Bad arguments. Bad URI protocol set! no \"/\" found." << endl;
            exit(1);
        }
        //There is port
        string possiblePort = remotePathNoProtocol.substr(0, slashPossition);
        port = atoi(possiblePort.c_str());
        
        //There is rest of remote path
        remotePathNoProtocol = remotePathNoProtocol.substr(slashPossition);
        
    }
    else {
        //No port set, use default
        size_t slashPossition = remotePathNoProtocol.find("/");
        if (slashPossition == string::npos) {
            cerr << "Error: Bad arguments. Bad URI protocol set! no \"/\" found." << endl;
            exit(1);
        }
        hostName = remotePathNoProtocol.substr(0, slashPossition);
        remotePathNoProtocol = remotePathNoProtocol.substr(slashPossition);
    }
    
    //Check port range
    if (port <= 0 || port > 65535) {
        cerr << "Error: Bad port number given." << endl;
        exit(1);
    }
    
    //----------- SOCKET START-----------//
    struct hostent *server;
    //Find server address trought DNS
    if ((server = gethostbyname(hostName.c_str())) == NULL) {
        cerr << "Error: Hostname not found." << endl;
        exit(2);
    }
    
    //Find ip adress
    struct sockaddr_in serverAdress;
    memcpy(&serverAdress.sin_addr, server->h_addr_list[0], server->h_length); //clean
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(port);
    
    //Create a socket
    int clientSocket;
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Error: Can not create socket." << endl;
        exit(2);
    }
    
    //Connect to server
    if (connect(clientSocket, (const struct sockaddr*) &serverAdress, sizeof(serverAdress)) != 0) {
        cerr << "Error: Can not connect to server." << endl;
        exit(2);
    }
    
    int returnCode;
    //Connected to the server :)
    if (put == true) {
        //Process put (upload) file
        processPut(&returnCode, remotePathNoProtocol, hostName, clientSocket, localPath);
    }
    else if (get == true) {
        //process get
        processGet(&returnCode, remotePathNoProtocol, hostName, clientSocket, localPath, localPathSet);
    }
    else if (del == true) {
        //process del 5
        processUniverzal(5, remotePathNoProtocol, hostName, clientSocket);
    }
    else if (lst == true) {
        //process lst 6
        processUniverzal(6, remotePathNoProtocol, hostName, clientSocket);
    }
    else if (mkd == true) {
        //process mkd 7
        processUniverzal(7, remotePathNoProtocol, hostName, clientSocket);
    }
    else if (rmd == true) {
        //process rmd 8
        processUniverzal(8, remotePathNoProtocol, hostName, clientSocket);
        
    }
    //Close connection
    close(clientSocket);
    return 0;
}
