/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: Pop3.h
 *
 * Definition of class that is responsible for connection and downloading e-mails.
 *
 */
#ifndef Pop3_h
#define Pop3_h

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <regex>
#include "ParamsParser.h"
#include "Database.h"

class Pop3
{
private:
    //! Prepared program parameters
    ProgramParams params;
    //! BIO for secure connection
    BIO * bio;
    //! Shoud be try IPv6 connection
    bool tryIpv6;
    //! SSL started in STLS command
    bool sslStarted;
    //! Certificate structure
    SSL_CTX * ctx;
    //! SSL structure
    SSL * ssl;
    //! Server certificate
    X509 * serverCert;
    //! Server URL with port
    string url;
    //! Min messages size to download
    vector<int> sizesToDownload;
    //! Response from server
    string message;
    //! Connected IP address (save to UID database)
    string ip;
    //! Message count on server
    int serverMessageC;
    //! Total size of all messages on server
    int totalSize;
    //! IPv6 file descriptor
    int ipv6Socket;
    //! File number
    int fileNumber;
    //! Indicate if server supports UIDL command
    bool uidl_alloved;
    
    /**
     * @brief Message-Id parser
     *
     * @return return message-id or ""
     */
    string getMessageID();
    
    /**
     * @brief Try log-in to server
     *
     * @return true if login succeded
     */
    bool Login();
    
    /**
     * @brief Try log-in to server
     *
     * @return true if logout succeded
     */
    bool Logout();
    
    /**
     * @brief Parse UID from server response
     *
     * @return string with UID
     */
    string ParseUID();
    
    /**
     * @brief Check if IPv6 connection should be tried
     *
     * @return true if IPv6 address passed
     */
    bool testIpv6();
    
    /**
     * @brief Connect to server
     *
     * @return true if connection ok
     */
    bool ConnectToServer();
    
    /**
     * @brief Create encrypted connection
     *
     * @return true if ssl handshake ok
     */
    bool CreateSecConn();
    
    /**
     * @brief Download one message
     *
     * @param id Message id on server
     * @param save Save message to file when true.
     *
     * @return true if message saved
     */
    bool DownloadOne(int id, bool save);
    
    /**
     * @brief Parse size and id from LIST command
     *
     */
    void ListProcess();
    
    /**
     * @brief Create and connect IPv6 socket
     *
     * @return true if connection ok
     */
    bool CreateIPV6Conn();
    
    /**
     * @brief Delete all messages on server
     *
     * @return true if delete succeded
     */
    bool DeleteAll();
    
    /**
     * @brief Create uncrypted connection and then switch to STLS command
     *
     * @return true if connection and ssl handshake succeded
     */
    bool CreateSTLSConn();
    
    /**
     * @brief Delete one message
     *
     * @param id Message id on server
     *
     * @return true if delete succeded
     */
    bool DeleteOne(int id);
    
    /**
     * @brief Cut first line and ".\r\n" from message
     *
     * @return count of striped bytes
     */
    int StripMessage();
    
    /**
     * @brief Create unsecure connection
     *
     * @return true if connection succeded
     */
    bool CreateUnsecConn();
    
    /**
     * @brief Read single line response from server
     *
     * @return true if message +OK
     */
    bool ReadMessage();
    
    /**
     * @brief Read multiline message
     *
     * @param downloadedSize Downloaded bytes from server
     *
     * @return true if message +OK
     */
    bool ReadMultiMessage(int &downloadedSize);
    
    /**
     * @brief Check if command was succesfull
     *
     * @return true if message have +OK
     */
    bool ResponseOK();
    
    /**
     * @brief Send message to server
     *
     * @param str String command
     *
     * @return true if send ok
     */
    bool SendMessage(string str);
    
    /**
     * @brief Load certificates for TSL/SSL
     *
     * @return true if load ok
     */
    bool LoadCertificate();
    
    /**
     * @brief Download all messages
     *
     * @return true if download succeded
     */
    bool DownloadAll();
    
    /**
     * @brief Parse message response
     *
     * @param messageID ID from server
     * @param size Size of message
     * @param mess Message to parse
     *
     */
    void MessageParser(int &messageID, int &size, string mess);
public:
    /**
     * @brief SSL library init, default values
     *
     * @param params Prepared parameters
     *
     */
    Pop3(ProgramParams params);
    
    /**
     * @brief Destructor for cleanup
     *
     */
    ~Pop3();
    
    /**
     * @brief Download e-mails
     *
     * @return 0 or ERR_Code
     */
    int DownloadMessages();
    
    /**
     * @brief Save email to file
     *
     * @return true when save is succesfull
     */
    bool SaveMessToFile();
};

#endif /* Pop3_hpp */
