/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: ParamsParser.h
 *
 * ProgramParams class definition.
 * Class is responsible for parsing and validation of passed program parameters.
 *
 */
#ifndef ParamsParser_h
#define ParamsParser_h

#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <limits.h>
#include "Error.h"
using namespace std;

class ProgramParams
{
private:
    //! Server domain name or ip
    string server;
    //! Path to file with credentials
    string authFile;
    //! Path to output folder
    string outDir;
    //! Path to certificate file
    string certFile;
    //! Path to certificate folder
    string certAddr;
    //! Login name
    string login;
    //! Password
    string password;
    //! Server port number
    int port;
    
    //THIS METHOD IS SELF-PLAGIARISM from my IKP project_1 school year: 2016/17 Autor: xjudap00
    /**
     * @brief Check if path is valid dir.
     *
     * @param path Path to dir.
     *
     * @return 0 if true
     */
    int isDir(const char *path);
    
    //THIS METHOD IS SELF-PLAGIARISM from my IKP project_1 school year: 2016/17 Autor: xjudap00
    /**
     * @brief Check if path is valid file.
     *
     * @param path Path to file.
     *
     * @return 0 if true
     */
    int isFile(const char *path);
    
    //THIS METHOD IS SELF-PLAGIARISM from my IKP project_1 school year: 2016/17 Autor: xjudap00
    /**
     * @brief Convert relative path to absolute path.
     *
     * @param pathToFile Relative path to file.
     *
     * @return Converted absolut path in string.
     */
    string convertRelativePath (string pathToFile);
    
    //THIS METHOD IS SELF-PLAGIARISM from my IKP project_1 school year: 2016/17 Autor: xjudap00
    /**
     * @brief Check if file or dir exist.
     *
     * @param path Path to file.
     
     * @return true if exists.
     */
    bool fileOrDirExist (const char* path);
public:
    //THIS METHOD IS SELF-PLAGIARISM from my IKP project_1 school year: 2016/17 Autor: xjudap00
    /**
     * @brief Check if file or dir exists.
     *
     * @param path Path to dir/file.
     *
     * @return true when it exists.
     */
    bool testDirFile(string *path);
    
    //! Custom port set
    bool portSet;
    //! Help set
    bool hSet;
    //! Server set
    bool sSet;
    //! TSL set
    bool TSet;
    //! STARTTLS set
    bool SSet;
    //! Certificate file set
    bool cSet;
    //! Certificate folder set
    bool CSet;
    //! Delete messages set
    bool dSet;
    //! Download new set
    bool nSet;
    //! Auth file set
    bool aSet;
    //! OutDir set
    bool oSet;
    //! Error indicate bad parameter
    bool error;
    /**
     * @brief Constructor sets default variable values
     *
     */
    ProgramParams();
    
    /**
     * @brief Parse program parameters
     *
     * @param argc Number of parameters
     * @param argv Array with parameters
     *
     * @return 0 if success or Error_code
     */
    int ParseParams(int argc, char** argv);
    
    /**
     * @brief Print help
     *
     */
    void PrintHelp();
    
    /**
     * @brief Validate if files and folders in parameters exists
     *
     * @return 0 if success or Error_code
     */
    int ValidParams();
    
    /**
     * @brief Load and validate username and password from file
     *
     * @return true when file is ok
     */
    bool LoadLoginPass();
    
    /**
     * @brief Getter for server name
     *
     * @return servername
     */
    string getServer();
    
    /**
     * @brief Getter for server port
     *
     * @return port
     */
    int getPort();
    
    /**
     * @brief Getter for login
     *
     * @return login
     */
    string getLogin();
    
    /**
     * @brief Getter for password
     *
     * @return password
     */
    string getPass();
    
    /**
     * @brief Getter for path to OutDir
     *
     * @return OutDir path
     */
    string getOutDir();
    
    /**
     * @brief Getter for path to certificate file
     *
     * @return certfile path
     */
    string getCertFile();
    
    /**
     * @brief Getter for path to certificate folder
     *
     * @return ceraddr path
     */
    string getCertDir();
    
    //TODO delete
    //only for test
    //void PrintParamsSetting();
};

#endif /* ParamsParser_hpp */
