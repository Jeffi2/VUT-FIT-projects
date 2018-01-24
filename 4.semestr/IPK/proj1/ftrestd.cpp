#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <cstdio>
#include <limits.h>
#include <cstring>
#include <errno.h>
#include <dirent.h>


extern int errno;
using namespace std;

/**
 * @file frest.cpp
 *
 * @brief Client for comunication with REST API server
 *
 * @autor xjudap00
 */


/**
 * @brief Sends message to client.
 *
 * @param message Message.
 * @param clientSocket Connected client.
 *
 * @return Created http REST header.
 */
int sendMessage(string message, int clientSocket) {
    int howMuchSend = send(clientSocket, message.data(), message.size(), 0);
    if (howMuchSend != message.size()) {
        return 1;
    }
    return 0;
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
 * @brief Convert relative path to absolute path.
 *
 * @param pathToFile Relative path to file.
 *
 * @return Created http REST header.
 */
string convertRelativePath (string pathToFile) {
    char absolutPath[PATH_MAX + 1];
    char *testPath = realpath(pathToFile.c_str(), absolutPath);
    if (!testPath) {
        return "BAD!";
    }
    return absolutPath;
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
 * @brief Create http response.
 *
 * @param returnCode Http status code.
 * @param httpReturnString Http status message.
 * @param contentType Type of body content.
 * @param body Sending body.
 * @param contentLength Size of body.
 *
 * @return Created REST response.
 */
string createResponse(int returnCode, string httpReturnString, string contentType, string body, size_t contentLength) {
    string response;
    int lengtByte = contentLength;
    if (strcmp(contentType.data(), "application/json") == 0) {
        lengtByte += 15;
    }
    
    stringstream headBuff;
    headBuff << "HTTP/1.1 " << returnCode << " " << httpReturnString << "\r\n"
    << "Date: " << getTimeStamp() << "\r\n"
    << "Content-Type: " << contentType << "\r\n"
    << "Content-Length: " << lengtByte << "\r\n" //TODO: switch to body length
    << "\r\n";
    
    response = headBuff.str();
    if (body != "") {
        if (strcmp(contentType.data(), "application/json") == 0) {
            response += "{\"message\": \"";
            response += body;
            response += "\"}";
        }
        else {
            response += body;
        }
    }
    
    return response;
}

/**
 * @brief HTTP handler.
 *
 * @param recivedMessage Message from client.
 * @param remotePath Path from client.
 * @param dirOrFile Dir of file request.
 * @param contentSize Body length.
 * @param user Username.
 * @param body Rquest body.
 * @param endOfHead Index where body ends.
 * @param type Http request type.
 *
 * @return 0 on success.
 */
int requestTypeHandler(string recivedMessage, string* remotePath, string* dirOrFile, int* contentSize, string* user, string* body, size_t* endOfHead, string* type) {
    
    //Find end of header
    *endOfHead = recivedMessage.find("\r\n\r\n")+4;
    size_t spacePosition = recivedMessage.find(" ");
    if (spacePosition == std::string::npos) {
        return 1;
    }
    //Parse http type
    *type = recivedMessage.substr(0, spacePosition);
    recivedMessage = recivedMessage.substr(spacePosition+1);
    //Parse file or dir
    size_t questionPosition = recivedMessage.find("?");
    if (questionPosition == std::string::npos) {
        return 1;
    }
    //Cut first /
    *remotePath = recivedMessage.substr(1, questionPosition-1); // cut /
    
    size_t secondSlashPossition = remotePath->find("/");
    if (secondSlashPossition == std::string::npos) {
       // perror("Bad URI protocol set! no user found\n");
        *user = *remotePath;
        remotePath->insert(0, "/");
        user->insert(0, "/");

    }
    else {
        //there is user
        *user = remotePath->substr(0, secondSlashPossition);
        //edit back correct path
        remotePath->insert(0, "/");
        user->insert(0, "/");
    }
    
    recivedMessage = recivedMessage.substr(questionPosition+1);
    
    size_t equalPosition = recivedMessage.find("=");
    if (equalPosition == std::string::npos) {
        return 1;;
    }
    size_t secondSpacePosition = recivedMessage.find(" ");
    if (secondSpacePosition == std::string::npos) {
        return 1;
    }
    *dirOrFile = recivedMessage.substr(equalPosition+1, secondSpacePosition-equalPosition-1);
    string contLenParse = recivedMessage.substr(recivedMessage.find("Content-Length: ")+16);
    size_t endLine = contLenParse.find("\r\n");
    string contLen = contLenParse.substr(0, endLine);
    
    size_t endHeader = recivedMessage.find("\r\n\r\n");
    
    *contentSize = std::stoi(contLen);
    *body = recivedMessage.substr(endHeader+4);
    if (recivedMessage.find("Content-Length: ") == std::string::npos)
        *contentSize = 0;
    
    return 0;
}

/**
 * @brief Check if file or dir exist.
 *
 * @param path Path to file.
 
 * @return true if exists.
 */
bool fileOrDirExist (const char* path) {
    if (std::ifstream(path))
    {
        return true;
    }
    return false;
}

/**
 * @brief Check if user exists on server.
 *
 *
 * @return true if exists
 */
bool checkUserExist(string rootDir, string user) {
    string path = rootDir;
    path += user;
    if (fileOrDirExist(path.data())) {
        if (isDir(path.data()) != 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Send respone to user.
 *
 * @param rootDir Server root dir.
 * @param user Username.
 * @param socket Connected clinet
 *
 * @return 0 on success.
 */
int userDoesNotExit (string rootDir, string user, int socket) {
    string firstResponse;
    if (!checkUserExist(rootDir, user)) {
        firstResponse = createResponse(404, "Not found", "application/json", "User Account Not Found.", strlen("User Account Not Found."));
        sendMessage(firstResponse, socket);
        close(socket);
        return 1;
    }
    return 0;
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void sendUnknownErr (int socket) {
    string firstResponse;
    firstResponse = createResponse(400, "Bad request", "application/json", "Unknown error.", strlen("Unknown error."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void alreadyExistErr (int socket) {
    string firstResponse;
    firstResponse = createResponse(400, "Bad request", "application/json", "Already exists.", strlen("Already exists."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void fileNotFoundErr (int socket) {
    string firstResponse;
    firstResponse = createResponse(404, "Not found", "application/json", "File not found.", strlen("File not found."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void notFileErr (int socket) {
    string firstResponse;
    firstResponse = createResponse(400, "Bad request", "application/json", "Not a file.", strlen("Not a file."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void notDirErr (int socket) {
    string firstResponse;
    firstResponse = createResponse(400, "Bad request", "application/json", "Not a directory.", strlen("Not a directory."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void dirNotFound (int socket) {
    string firstResponse;
    firstResponse = createResponse(404, "Not Found", "application/json", "Directory not found.", strlen("Directory not found."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Send respone to user.
 *
 * @param socket Connected clinet
 *
 */
void notEmptyDir (int socket) {
    string firstResponse;
    firstResponse = createResponse(400, "Bad request", "application/json", "Directory not empty.", strlen("Directory not empty."));
    sendMessage(firstResponse, socket);
    close(socket);
}

/**
 * @brief Proces upload file.
 *
 * @param rootDir Server root dir.
 * @param remotePath Path from user.
 * @param socket Connected clinet
 * @param uploadSize Sife of file.
 * @param user Username.
 * @param body File binary data.
 *
 * @return 0 on success.
 */
int processReciveFile(string rootDir, string remotePath, int socket, int uploadSize, string user, vector<unsigned char>& body) {
    string firstResponse;
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    
    string pathToCreate;
    pathToCreate += rootDir;
    pathToCreate += remotePath;
    
    //Bad file name
    if (strcmp(&pathToCreate.back(), "/") == 0) {
        firstResponse = createResponse(400, "Bad request", "application/json", "Unknown error.", strlen("Unknown error."));
        sendMessage(firstResponse, socket);
        close(socket);
        return 1;
    }
    //Parse root dir
    size_t lastSlashPossition = pathToCreate.find_last_of("/");
    if (lastSlashPossition == std::string::npos) {
        sendUnknownErr (socket);
        return 1;
    }
    //Can not write to dir
    string dirToWrite = pathToCreate.substr(0, lastSlashPossition);
    if (!fileOrDirExist(dirToWrite.c_str())) {
        sendUnknownErr (socket);
        return 1;
    }
    //Already exists
    if (fileOrDirExist(pathToCreate.c_str())) {
        alreadyExistErr (socket);
        return 1;
    }
    //Try to create file:
    ofstream file;
    file.open(pathToCreate.data(), std::ios::out | std::ios::binary);
    if (!file) {
        //Can not create file
        sendUnknownErr(socket);
        file.close();
        return 1;
    }
    //Write data to file
    file.write((const char*)&body[0], body.size());
    file.close();
    //Succes
    firstResponse = createResponse(200, "OK", "text/plain", "", 0);
    sendMessage(firstResponse, socket);
    close(socket);
    return 0;
}

/**
 * @brief Proces download file.
 *
 * @param rootDir Server root dir.
 * @param pathFromUser Path from user.
 * @param socket Connected clinet
 * @param user Username.
 *
 * @return 0 on success.
 */
int processSendFile(string rootDir, string pathFromUser, int socket, string user) {
    string firstResponse;
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    string pathToFile = rootDir;
    pathToFile += pathFromUser;
    if (!fileOrDirExist(pathToFile.data())) {
        //File not found
        fileNotFoundErr(socket);
        return 1;
    }
    if (!isFile(pathToFile.data())) {
        //Not a file
        notFileErr(socket);
        return 1;
    }
    
    //Open file for read
    ifstream openedFile;
    openedFile.open(pathToFile, std::ios::in | std::ios::binary | std::ios::ate);
    if (!openedFile.is_open()) {
        sendUnknownErr (socket);
        return 1;
    }
    //Size of file
    size_t downloadFileSize = openedFile.tellg();
    openedFile.seekg(0, std::ios::beg);
    //Send head
    firstResponse = createResponse(200, "OK", "application/octet-stream", "", downloadFileSize);
    sendMessage(firstResponse, socket);
    //Send data
    int loadFromFile = 1;
    char sendBuffer[1024];
    while (loadFromFile > 0) {
        openedFile.read(sendBuffer, 1024);
        loadFromFile = openedFile.gcount();
        if (send(socket, sendBuffer, loadFromFile, 0) < 0) {
            openedFile.close();
            close(socket);
            return 1;
        }
        //Clean
        memset(sendBuffer, 0, sizeof(sendBuffer));
    }
    //OK
    openedFile.close();
    close(socket);
    return 0;
}

//delete file
int processDeleteFile(string rootDir, string pathFromUser, int socket, string user) {
    string firstResponse;
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    string pathToFile = rootDir;
    pathToFile += pathFromUser;
    if (!fileOrDirExist(pathToFile.data())) {
        fileNotFoundErr(socket);
        return 1;
    }
    if (!isFile(pathToFile.data())) {
        notFileErr(socket);
        return 1;
    }
    //Delete file
    if (std::remove(pathToFile.data()) != 0) {
        sendUnknownErr (socket);
        return 1;
    }
    
    firstResponse = createResponse(200, "OK", "text/plain", "", 0);
    sendMessage(firstResponse, socket);
    close(socket);
    
    return 0;
}

/**
 * @brief Create dir on server.
 *
 * @param rootDir Server root dir.
 * @param pathFromUser Path from user.
 * @param socket Connected clinet
 * @param user Username.
 *
 * @return 0 on success.
 */
int processMakeDir(string rootDir, string pathFromUser, int socket, string user) {
    string firstResponse;
    string checkCreateUser = user;
    checkCreateUser += "/";
    //Check if try to create user
    if ((strcmp(pathFromUser.data(), user.data()) == 0) || (strcmp(pathFromUser.data(), checkCreateUser.data()) == 0)) {
        sendUnknownErr (socket);
        return 1;
    }
    //Already exist
    string pathToFile = rootDir;
    pathToFile += pathFromUser;
    if (fileOrDirExist(pathToFile.data())) {
        alreadyExistErr (socket);
        return 1;
    }
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    
    //Create dir
    const int dir_err = mkdir(pathToFile.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err) {
        //Can not create dir
        sendUnknownErr (socket);
        return 1;
    }
    
    firstResponse = createResponse(200, "OK", "text/plain", "", 0);
    sendMessage(firstResponse, socket);
    close(socket);
    return 0;
}

/**
 * @brief Remove dir on server.
 *
 * @param rootDir Server root dir.
 * @param pathFromUser Path from user.
 * @param socket Connected clinet
 * @param user Username.
 *
 * @return 0 on success.
 */
int processRemoveDir(string rootDir, string pathFromUser, int socket, string user) {
    string firstResponse;
    string checkDeleteUser = user;
    checkDeleteUser += "/";
    //Check if try to delete user
    if ((strcmp(pathFromUser.data(), user.data()) == 0) || (strcmp(pathFromUser.data(), checkDeleteUser.data()) == 0)) {
        sendUnknownErr (socket);
        return 1;
    }
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    string pathToFile = rootDir;
    pathToFile += pathFromUser;
    //Dir not found
    if (!fileOrDirExist(pathToFile.data())) {
        dirNotFound(socket);
        return 1;
    }
    //Not a dir
    if (!isDir(pathToFile.data())) {
        notDirErr(socket);
        return 1;
    }
    int dir_err = rmdir(pathToFile.data());
    if (-1 == dir_err) {
        switch (errno) {
            case EEXIST:
                notEmptyDir(socket);
                return 1;
            case ENOTEMPTY:
                notEmptyDir(socket);
                return 1;
            default:
                //Permission
                sendUnknownErr(socket);
                return 1;
        }
    }
    //OK
    firstResponse = createResponse(200, "OK", "text/plain", "", 0);
    sendMessage(firstResponse, socket);
    close(socket);
    
    return 0;
}

/**
 * @brief Send files and directory names in folder.
 *
 * @param rootDir Server root dir.
 * @param pathFromUser Path from user.
 * @param socket Connected clinet
 * @param user Username.
 *
 * @return 0 on success.
 */
int processListDir(string rootDir, string pathFromUser, int socket, string user) {
    string firstResponse;
    //If user dont exist send response
    if (userDoesNotExit (rootDir, user, socket) != 0) {
        return 1;
    }
    string pathToFile = rootDir;
    pathToFile += pathFromUser;
    //Not found
    if (!fileOrDirExist(pathToFile.data())) {
        dirNotFound(socket);
        return 1;
    }
    //Not a dir
    if (!isDir(pathToFile.data())) {
        notDirErr(socket);
        return 1;
    }
    
    string lstData;
    DIR* directory        { nullptr };
    struct dirent* ent    { nullptr };
    
    //Open and read directory.
    //Save file names into the queue.
    if ((directory = opendir(pathToFile.data())) != nullptr) {
        while ((ent = readdir(directory)) != nullptr) {
            if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)) {
                lstData += ent->d_name;
                lstData += '\n';
            }
        }
        closedir(directory);
    }
    else {
        sendUnknownErr (socket);
        return 1;
    }
    //Delete last \n
    if (lstData.size() > 1) {
        lstData = lstData.substr(0, lstData.size()-1);
    }
    
    //OK
    firstResponse = createResponse(200, "OK", "application/json", lstData.data(), lstData.size());
    sendMessage(firstResponse, socket);
    close(socket);
    
    return 0;
}

/**
 * @brief Check if server root dir is ok.
 *
 * @param rootPath Path to server root directory.
 *
 * @return true when it exists.
 */
bool testRootDir(string *rootPath) {
    //Check absolut path
    string absolutPathToRoot = convertRelativePath(*rootPath);
    if (absolutPathToRoot == "BAD!") {
        return false;
    }
    *rootPath = absolutPathToRoot;
    //Check if exists
    if (fileOrDirExist(absolutPathToRoot.c_str())) {
        //Check if is dir
        if (isDir(absolutPathToRoot.data()) != 0) {
            return true;
        }
    }
    return false;
}


/**
 * @brief Control flow, validate program params and sets socket comunication.
 *
 * @param argc Number of program params.
 * @param argv Array with program params.
 *
 */
int main(int argc, char* argv[]) {
    // --------------- START OF PARAM VALIDATION ------ //
    //Set default root directory
    string rootDir = "./";
    int portNumber = 6677;
    bool isPortSet = false;
    string possiblePort;
    if (argc > 5) {
        //More than 5 arguments
        cerr << "Error: Bad arguments." << endl;
        exit(1);
    }
    //Bad count of arguments
    if (argc == 2 || argc == 4) {
        cerr << "Error: Bad arguments." << endl;
        exit(1);
    }
    //Validation arguments
    if (argc ==  5) {
        //All 4 set
        if (strcmp(argv[1], "-r") == 0) {
            rootDir = argv[2];
            if (strcmp(argv[3], "-p") == 0) {
                possiblePort = argv[4];
                isPortSet = true;
            } else {
                cerr << "Error: Bad arguments." << endl;
                exit(1);
            }
        }
        else
            if(strcmp(argv[1], "-p") == 0) {
                possiblePort = argv[2];
                isPortSet = true;
                
                if (strcmp(argv[3], "-r") == 0) {
                    rootDir = argv[4];
                } else {
                    cerr << "Error: Bad arguments." << endl;
                    exit(1);
                }
            } else {
                cerr << "Error: Bad arguments." << endl;
                exit(1);
            }
    }
    else if (argc == 3){
        //Only 2 set
        if (strcmp(argv[1], "-p") == 0) {
            possiblePort = argv[2];
            isPortSet = true;
        } else if (strcmp(argv[1], "-r") == 0) {
            rootDir = argv[2];
        } else {
            cerr << "Error: Bad arguments." << endl;
            exit(1);
        }
    }
    //Port number set
    if (isPortSet == true) {
        portNumber = atoi(possiblePort.c_str());
    }
    if (portNumber <= 0 || portNumber > 65535) {
        cerr << "Error: Bad port numer." << endl;
        exit(1);
    }
    //Test if root dir is valid
    if (!testRootDir(&rootDir)) {
        cerr << "Error: Bad root dir." << endl;
        exit(3);
    }
   // cout << "Root dir: " <<rootDir << endl;
   // cout << "Port : " <<portNumber << endl;
    
    // --------------- END OF PARAM VALIDATION ------ //
    
    // ---------- START OF SOKET SETTING--------- //
    int welcomeSocket;
    struct sockaddr_in serverAdress;
    
    memset(&serverAdress, 0, sizeof(serverAdress));
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = INADDR_ANY;
    serverAdress.sin_port = htons(portNumber);
    
    //Create socket
    if ((welcomeSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //error create socket
        cerr << "Error: Can not create socket." << endl;
        exit(2);
    }
    
    //Bind socket
    if (::bind(welcomeSocket, (struct sockaddr *) &serverAdress, sizeof(serverAdress)) < 0) {
        close(welcomeSocket); //zavri socket
        cerr << "Error: Can not bind port." << endl;
        exit(2);
    }
    
    //Listen on socket
    if (listen(welcomeSocket, 1) < 0) {
        close(welcomeSocket);
        cerr << "Error: Can not listen on socket." << endl;
        exit(2);
    }
    //New socket for connect
    int connectSocket;
    struct sockaddr_in clientAdress;
    socklen_t clientAdressLength = sizeof(clientAdress);
    
    //Catch connection
    while (1) {
        //Accept connection
        connectSocket = accept(welcomeSocket, (struct sockaddr *) &clientAdress, &clientAdressLength);
        if (connectSocket > 0) {
            //Buffer for first data
            char recivedMessageBuff[1024];
            int reciveBytes;
            //Get header
            reciveBytes = recv(connectSocket, recivedMessageBuff, 1024, 0);
            //Variables for parse header
            string recivedMessage = recivedMessageBuff;
            string pathFromUser;
            string user;
            string dirOrFile;
            string httpType;
            string body;
            int contentSize = 0;
            size_t endOfHead;
            //Parse header
            if (requestTypeHandler(recivedMessage, &pathFromUser, &dirOrFile, &contentSize, &user, &body, &endOfHead, &httpType) != 0) {
                //This couse bad request response
                httpType = "BAD!";
            }
            //Recived bytes counter
            int recived = 0;
            //Vector for http body from head
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
                if ((bytes = recv(connectSocket, reciveBuffer, 1024, 0)) < 0) {
                    //error on recive
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
            //Internal error
            if (recived != contentSize) {
                cerr << "Error: Should recive more data." << endl;
                sendUnknownErr(connectSocket);
            }
            if (recived != fileData.size()) {
                cerr << "Error: Data are damaged." << endl;
                sendUnknownErr(connectSocket);
            }
            // Bad request
            if (dirOrFile != "file" && dirOrFile != "dir") {
                sendUnknownErr(connectSocket);
            }
            //Process accepted requests
            if (httpType == "PUT" && dirOrFile == "file") {
                //Process put (upload)
                processReciveFile(rootDir, pathFromUser, connectSocket, contentSize, user, fileData);
            }
            else if (httpType == "PUT" && dirOrFile == "dir") {
                //Process mkdir
                processMakeDir(rootDir, pathFromUser, connectSocket, user);
            }
            else if (httpType == "GET" && dirOrFile == "file") {
                //Process get (download)
                processSendFile(rootDir, pathFromUser, connectSocket, user);
            }
            else if (httpType == "GET" && dirOrFile == "dir") {
                //Proces lst (ls obsah)
                processListDir(rootDir, pathFromUser, connectSocket, user);
            }
            else if (httpType == "DELETE" && dirOrFile == "file") {
                //Process del file
                processDeleteFile(rootDir, pathFromUser, connectSocket, user);
            }
            else if (httpType == "DELETE" && dirOrFile == "dir") {
                //Process rmd (rmdir)
                processRemoveDir(rootDir, pathFromUser, connectSocket, user);
            }
            else {
                string response = createResponse(400, "Bad request", "application/json", "Bad request!", strlen("Bad request!"));
                sendMessage(response, connectSocket);
                close(connectSocket);
            }
            //Clear variables for another connection
            httpType.clear(); pathFromUser.clear(); user.clear(); dirOrFile.clear();
            memset(recivedMessageBuff, 0, sizeof(recivedMessageBuff));
        }
        else {
            cerr << "Error: Can not accept." << endl;
        }
    }
}
