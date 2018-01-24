/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: Database.h
 *
 * Class responsible saving UID to database file. For downloading only new messages.
 *
 */
#ifndef Database_h
#define Database_h

#include "ParamsParser.h"
#include <vector>
#include <sstream>
#include <fstream>

class UIDdatabase
{
private:
    //! Prepared parameters
    ProgramParams params;
    //! Path to database folder
    string databaseFolder;
    //! Name of databe file
    string filename;
    //! UID from file
    string fileContent;
    //! Path to database file
    string fileLink;
    //! UIDs
    vector<string> uids;
    
public:
    /**
     * @brief Constructor, set default database folder
     *
     * @param params Prepared program parameters
     *
     */
    UIDdatabase(ProgramParams params);
    
    /**
     * @brief Create database folder if does not exists
     *
     * @return true if exists or created
     */
    bool CreateDir();
    
    /**
     * @brief Add UID to uids vector
     *
     */
    void AddUID(string uid);
    
    /**
     * @brief Parse UID from file to uids vector
     *
     */
    void ParseUID();
    
    /**
     * @brief Save UIDs to database file
     *
     * @return true if file saved
     */
    bool SaveUID();
    
    /**
     * @brief Check if UID is already in database
     *
     * @param uid UID to compare with database
     *
     * @return true if is in database
     */
    bool FindUID(string uid);
    
    /**
     * @brief Load database file
     *
     * @param username Username(login) to server
     * @param ip IP address of server (IPv4 or IPv6)
     *
     * @return true file loaded
     */
    bool LoadFile(string username, string ip);
    
    /**
     * @brief Tests if uids vector is empty
     *
     * @return true when uids is emty
     */
    bool IsUidsEmpty();
};

#endif /* Database_hpp */
