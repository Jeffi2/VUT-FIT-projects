/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: Pop3.cpp
 *
 * Class responsible for connection and downloading e-mails.
 *
 */
#include "Pop3.h"

Pop3::Pop3(ProgramParams params)
{
    this->params = params;
    this->url += params.getServer();
    this->url += ':';
    this->url += to_string(params.getPort());
    this->sslStarted = false;
    this->ipv6Socket = 0;
    this->uidl_alloved = false;
    this->tryIpv6 = false;
    this->ctx = NULL;
    this->bio = NULL;
    this->serverCert = NULL;
    this->fileNumber = 1;
    
    // Initializing OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
}

bool Pop3::ConnectToServer()
{
    if (this->params.TSet)
    {
        //Create TLS connection
        return this->CreateSecConn();
    }
    
    if (this->params.SSet)
    {
        //Create STARTTLS connection
        return this->CreateSTLSConn();
    }
    
    if (this->tryIpv6)
    {
        //IPV6 unsececured connection
        return this->CreateIPV6Conn();
    }
    //IPV4 unsecured connection
    return this->CreateUnsecConn();
}

bool Pop3::CreateSTLSConn()
{
    //Load certificate for SSL handshake
    if (!this->LoadCertificate())
    {
        return false;
    }
    
    //IPV4 x IPV6
    if (!tryIpv6)
    {
        this->CreateUnsecConn();
    }
    else
    {
        this->CreateIPV6Conn();
    }
   
    //Get welcome message
    if (!ReadMessage())
    {
        return false;
    }
    //Send STLS command
    if (!SendMessage("STLS\r\n"))
    {
        return false;
    }
    //Server ready for handshake
    if (!ReadMessage())
    {
        return false;
    }
    //Set up SSL properties
    this->ssl = SSL_new(ctx);
    SSL_set_mode(this->ssl, SSL_MODE_AUTO_RETRY);
    
    this->sslStarted = true;
    SSL_set_bio(this->ssl, bio, bio);
    //SSL connect and handshake
    if (SSL_connect(ssl) != 1)
    {
        cerr << "Error: SSL connect failed." << endl;
        return false;
    }
    if (!SSL_do_handshake(ssl))
    {
        cerr << "Error: SSL handshake fail." << endl;
    }
    //Check certificate
    if (SSL_get_verify_result(ssl) != X509_V_OK)
    {
        cerr << "Error: Invalide certificate." << endl;
        return false;
    }
    //Check if server really send a certificate
    if ((this->serverCert = SSL_get_peer_certificate(ssl)) == NULL)
    {
        cerr << "Error: Server does not presented certificate." << endl;
    }
    //Connection ok
    return true;
}


bool Pop3::CreateSecConn()
{
    //Load certificate
    if (!this->LoadCertificate())
    {
        return false;
    }
    
    if (!this->tryIpv6)
    {
        //IPV4
        this->bio = BIO_new_ssl_connect(this->ctx);
        BIO_get_ssl(this->bio, &ssl);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
        BIO_set_conn_hostname(this->bio, this->url.c_str());
        if (BIO_do_connect(this->bio) <= 0)
        {
            cerr << "Error: Can not connect to server." << endl;
            return false;
        }
    }
    else
    {
        //IPV6
        this->CreateIPV6Conn();
        this->ssl = SSL_new(ctx);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
        BIO_set_ssl(this->bio, ssl, BIO_CLOSE);
        SSL_set_fd(ssl, ipv6Socket);
        if (SSL_connect(ssl) != 1)
        {
            cerr << "Error: SSL connect failed." << endl;
            return false;
        }
        if (SSL_do_handshake(ssl) != 1)
        {
            cerr << "Error: SSL handshake failed." << endl;
            return false;
        }
    }
    //Check certificate
    if (SSL_get_verify_result(ssl) != X509_V_OK)
    {
        cerr << "Error: Invalide certificate." << endl;
        return false;
    }
    //Check if server really send a certificate
    if ((this->serverCert = SSL_get_peer_certificate(ssl)) == NULL)
    {
        cerr << "Error: Server does not presented certificate." << endl;
    }
    //Connect ok
    return true;
}

bool Pop3::CreateIPV6Conn()
{
    //IPV6 socket
    if ((this->ipv6Socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Error: Can not create socket." << endl;
        return false;
    }
    struct sockaddr_in6 sa;
    memset(&sa,0,sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(this->params.getPort());
    inet_pton(AF_INET6, this->params.getServer().c_str(), (void *)&sa.sin6_addr.s6_addr);
    
    //Try to connect to server
    if (connect(ipv6Socket, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    {
        cerr << "Error: Can not connect to server." << endl;
        return false;
    }
    
    if (this->params.TSet)
    {
        //SSL BIO
        this->bio = BIO_new(BIO_f_ssl());
    }
    else
    {
        //UNCRYPTED BIO
        this->bio = BIO_new(BIO_s_socket());
    }
    BIO_set_fd(this->bio, ipv6Socket, BIO_CLOSE);
    return true;
}

bool Pop3::CreateUnsecConn()
{
    //IPV4 standard connection
    this->bio = BIO_new_connect(const_cast<char*>(this->url.c_str()));
    if (this->bio == NULL)
    {
        cerr << "Error: Problem with creting socket." << endl;
        return false;
    }
    
    if (BIO_do_connect(this->bio) <= 0)
    {
        cerr << "Error: Can not connect to server." << endl;
        return false;
    }
    
    return true;
}

bool Pop3::ReadMessage()
{
    //Read single line message
    this->message.clear();
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    bool tryAgain = true;
    int readCounter = 0;
    //If BIO read shoud be retry
    while (tryAgain)
    {
        if (this->params.SSet && this->sslStarted)
        {
            //Only when STARTTLS started
            readCounter = SSL_read(this->ssl, buffer, 1022);
        }
        else
        {
            readCounter = BIO_read(this->bio, buffer, 1022);
        }
        if(readCounter == 0)
        {
            cerr << "Error: Connection lost." << endl;
            return false;
        }
        else if(readCounter < 0)
        {
            if (!this->params.SSet) {
                if(! BIO_should_retry(this->bio))
                {
                    cerr << "Error: Connection lost." << endl;
                    return false;
                }
            }
            else
                //When STARTTLS read fail
                return false;
        }
        else
        {
            //BIO read ok
            tryAgain = false;
        }
    }
    for (int i = 0; i < readCounter; i++)
    {
        this->message.push_back(buffer[i]);
    }
    //Check if server send +OK message
    return this->ResponseOK();
}

bool Pop3::ReadMultiMessage(int &downloadedSize)
{
    downloadedSize = 0;
    this->message.clear();
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    bool tryAgain = true;
    bool messageComplete = false;
    int readCounter = 0;
    bool triedAgain = false;
    
    //Read until \n\r.\n\r
    while (tryAgain || !messageComplete)
    {
        triedAgain = false;
        if (this->params.SSet && this->sslStarted)
        {
            readCounter = SSL_read(this->ssl, buffer, 1022);
        }
        else
        {
            readCounter = BIO_read(this->bio, buffer, 1022);
        }
        if(readCounter == 0)
        {
            cerr << "Error: Connection lost." << endl;
            return false;
        }
        else if(readCounter < 0)
        {
            if (!this->params.SSet)
            {
                if(! BIO_should_retry(this->bio))
                {
                    cerr << "Error: Connection lost." << endl;
                    return false;
                }
                triedAgain = true;
            }
            else
                //STARTTLS read fail
                return false;
        }
        else
        {
            //BIO read ok
            tryAgain = false;
        }
        for (int i = 0; i < readCounter; i++)
        {
            this->message.push_back(buffer[i]);
        }
        string tmp(message.begin(), message.end());

        //Look if message is comlete
        size_t found = tmp.find("\r\n.\r\n");
        if (found != string::npos)
        {
            messageComplete = true;
        }
        else
            memset(buffer, 0, sizeof(buffer));
        if (!triedAgain)
        {
            //Count downloaded size
            downloadedSize += readCounter;
        }
        readCounter = 0;
    }
    //Check if server response contains +OK
    return this->ResponseOK();
}


bool Pop3::SendMessage(string str)
{
    char buffer[1024];
    for (int i = 0; i < (int) str.length(); i++) {
        buffer[i] = str.at(i);
    }
    bool tryAgain = true;
    int sendCounter = 0;
    
    while (tryAgain) {
        if (this->params.SSet && this->sslStarted)
        {
            //Send encrypted message for STARTTLS
            sendCounter = SSL_write(this->ssl, buffer, (int)str.size());
        }
        else
        {
            sendCounter = BIO_write(this->bio, buffer, (int)str.size());
        }
        if(sendCounter <= 0)
        {
            if (!BIO_should_retry(this->bio)) {
                cerr << "Error: Can not send request to server." << endl;
                return false;
            }
        }
        else
        {
            //BIO send ok
            tryAgain = false;
        }
    }
    return true;
}

bool Pop3::Logout()
{
    //Send QUIT command
    if (!this->SendMessage("QUIT\r\n"))
    {
        return false;
    }
    if (!this->ReadMessage())
    {
        return false;
    }
    return true;
}

bool Pop3::ResponseOK()
{
    //+0K
    if (this->message.at(0) == '+')
    {
        return true;
    }
    return false;
}

bool Pop3::Login()
{
    if (this->SendMessage("USER "+this->params.getLogin()+"\r\n"))
    {
        if (this->ReadMessage())
        {
            if (this->SendMessage("PASS "+this->params.getPass()+"\r\n"))
            {
                if (!this->ReadMessage())
                {
                    this->Logout();
                    cerr << "Error: Bad password." << endl;
                    return false;
                }
                return true;
            }
        }
        else {
            this->Logout();
            cerr << "Error: Bad login." << endl;
            return false;
        }
    }
    return false;
}

bool Pop3::testIpv6()
{
    //IPV6 address shoud contain ':' character
    size_t found = this->params.getServer().find(":");
    if (found == string::npos)
    {
        return false;
    }
    return true;
}

bool Pop3::LoadCertificate()
{
    this->ctx = SSL_CTX_new(TLSv1_2_client_method());
    if (this->ctx == NULL)
    {
        cerr << "Error: Can not create SSL struct." << endl;
        return false;
    }
    
    if (this->params.cSet && !this->params.CSet)
    {
        //Load cert file
        if (!SSL_CTX_load_verify_locations(this->ctx, this->params.getCertFile().c_str(), NULL))
        {
            cerr << "Error: Can not load certificate file." << endl;
            return false;
        }
    }
    
    if (this->params.CSet && !this->params.cSet)
    {
        //Load cert dir
        if (!SSL_CTX_load_verify_locations(this->ctx, NULL, this->params.getCertDir().c_str()))
        {
            cerr << "Error: Can not load certificate folder." << endl;
            return false;
        }
    }
    
    if (this->params.CSet && this->params.cSet)
    {
        //Load both
        if (!SSL_CTX_load_verify_locations(this->ctx, this->params.getCertFile().c_str(), this->params.getCertDir().c_str()))
        {
            cerr << "Error: Can not load certificate file or folder." << endl;
            return false;
        }
    }
    
    if (!this->params.CSet && !this->params.cSet)
    {
        //Load default
        if (!SSL_CTX_set_default_verify_paths(this->ctx))
        {
            cerr << "Error: Can not load default certificate folder." << endl;
            return false;
        }
    }
    return true;
}

int Pop3::DownloadMessages()
{
    //Test if server name/IPv4 or IPv6
    this->tryIpv6 = testIpv6();
    //Connect to POP3 server
    if (!this->ConnectToServer())
    {
        return ERR_CONNECTION;
    }
    else
    {
        //Connection OK
        //If not STLS read welcome message
        if (!this->params.SSet) {
            if (!this->ReadMessage())
            {
                cerr << "Error: Server not ready." << endl;
                return ERR_SERVER_FAIL;
            }
        }
    }
    
    if (tryIpv6)
    {
        //IPv6 from cmd
        this->ip = this->params.getServer();
    }
    else
    {
        //Get IP from connected BIO
        char * test = BIO_get_conn_ip(this->bio);
        if (!((int)test[0] == 0))
        {
            unsigned char ip1 = test[0];
            unsigned char ip2 = test[1];
            unsigned char ip3 = test[2];
            unsigned char ip4 = test[3];
            this->ip = to_string((int)ip1) + "." + to_string((int)ip2) + "." + to_string((int)ip3) + "." + to_string((int)ip4);
        }
    }
   
    //Try to login
    if (!this->Login())
    {
        return ERR_LOGIN;
    }
    
    //Send stat
    if (!this->SendMessage("STAT\r\n"))
        return ERR_SERVER_FAIL;
    if (!this->ReadMessage())
        return ERR_SERVER_FAIL;
    
    //Get total message count on server
    this->MessageParser(this->serverMessageC, this->totalSize, this->message.substr(4));
    if (this->serverMessageC == 0)
    {
        cout << "Email account is empty. (0 messages)" << endl;
        return OK;
    }
    
    int multiMessSize = 0;
    //Send list and parse sizes
    if (!this->SendMessage("LIST\r\n"))
        return ERR_SERVER_FAIL;
    if (!this->ReadMultiMessage(multiMessSize))
        return ERR_SERVER_FAIL;
    this->ListProcess();
    
    //Test if UIDL is alloved
    if (!this->SendMessage("UIDL 1\r\n"))
        return ERR_SERVER_FAIL;
    this->uidl_alloved = ReadMessage();
    
    //Download messages and check if their size are correct
    if (this->params.nSet)
    {
        //Only new messages
        int counter = 0;
        UIDdatabase database = UIDdatabase(params);
        //Database dir
        database.CreateDir();
        //Load file from database
        if (database.LoadFile(this->params.getLogin(), this->ip))
        {
        //Read UIDs from file
        database.ParseUID();
        for (int i = 1; i <= serverMessageC; i++)
        {
            string uid = "";
            if (this->uidl_alloved)
            {
                //Get message UID
                if (!this->SendMessage("UIDL "+to_string(i)+"\r\n"))
                    return false;
                if (!this->ReadMessage())
                {
                    return false;
                }
                uid = this->ParseUID();
            }
            //Download new message
            if (!this->DownloadOne(i, false))
                return ERR_SERVER_FAIL;
            if (!this->uidl_alloved)
            {
                uid = getMessageID();
            }
            if (!uid.empty())
            {
                //Check if UID is in database
                if (!database.FindUID(uid))
                {
                    //Add UID to databe
                    database.AddUID(uid);
                    //Save to file
                    if (!this->SaveMessToFile())
                        return ERR_SAVE_FILE;
                    counter++;
                    //Delete
                    if (this->params.dSet)
                    {
                        //If -d set delete message
                        if (!this->DeleteOne(i))
                            return ERR_SERVER_FAIL;
                    }
                }
            }
        }
        //Save changes in database to file
        database.SaveUID();
        }
        else
        {
            if (!this->DownloadAll())
                return ERR_SERVER_FAIL;
            counter = this->serverMessageC;
        }
        if (counter == 1)
        {
            cout << "Downloaded: " << counter << " new message." << endl;
        }
        else
        {
            cout << "Downloaded: " << counter << " new messages." << endl;
        }
    }
    else
    {
        if (this->DownloadAll())
        {
            if (this->serverMessageC == 1)
            {
                cout << "Downloaded: " << this->serverMessageC << " message." << endl;
            }
            else
            {
                cout << "Downloaded: " << this->serverMessageC << " messages." << endl;
            }
        }
        else
        {
            return ERR_SERVER_FAIL;
        }
        if (this->params.dSet)
        {
            if (!this->DeleteAll())
            {
                return ERR_SERVER_FAIL;
            }
        }
    }
    //Logout from server
    if (!this->Logout())
    {
        cerr << "Error: Uncorrect logout." << endl;
        return ERR_SERVER_FAIL;
    }
    return 0;
}

bool Pop3::DeleteOne(int id)
{
    //Delete one message
    if (!this->SendMessage("DELE "+to_string(id)+"\r\n"))
    {
        return false;
    }
    if (!this->ReadMessage())
    {
        cerr << "Error: Server can not delete message." << endl;
        return false;
    }
    return true;
}

bool Pop3::DeleteAll()
{
    for (int i = 1; i <= this->serverMessageC; i++)
    {
        if (!this->DeleteOne(i))
        {
            return false;
        }
    }
    return true;
}

bool Pop3::SaveMessToFile()
{
    int counter = this->fileNumber;
    string path = this->params.getOutDir()+"/"+this->params.getLogin()+"_"+this->ip+"_"+to_string(counter);
    while (this->params.testDirFile(&path))
    {
        counter++;
        path = this->params.getOutDir()+"/"+this->params.getLogin()+"_"+this->ip+"_"+to_string(counter);
    }
    //Save to file
    ofstream outfile;
    outfile.open(path);
    if (!outfile.is_open())
    {
        cerr << "Error: Can not create output file!" << endl;
        return false;
    }
    outfile << this->message;
    outfile.close();
    counter++;
    this->fileNumber = counter;
    return true;
}

bool Pop3::DownloadOne(int id, bool save)
{
    int size = 0;
    if (!this->SendMessage("RETR "+to_string(id)+"\r\n"))
        return false;
    if (!this->ReadMultiMessage(size))
        return false;
    size -= this->StripMessage();
    if (size < this->sizesToDownload[id-1])
    {
        cerr << "Error: Downloaded message size is smaller that should be! (Maybe not complete message)" << endl;
    }
    if (save)
    {
        return SaveMessToFile();
    }
    return true;
}

string Pop3::ParseUID()
{
    //UIDL number parser
    string uid = this->message;
    uid = uid.substr(4);
    size_t found = uid.find(" ");
    if (found != string::npos)
    {
        uid = uid.substr(found+1);
        uid = uid.substr(0, uid.size()-2);
    }
    return uid;
}

string Pop3::getMessageID()
{
    string messId = "";
    regex rgx("Message-Id: <.*>", regex_constants::icase);
    smatch match;
    
    if (regex_search(message, match, rgx))
    {
        messId = match[0];
    }
    
    return messId;
}

bool Pop3::DownloadAll()
{
    UIDdatabase database = UIDdatabase(params);
    database.CreateDir();
    database.LoadFile(this->params.getLogin(), this->ip);
    for (int i = 1; i <= serverMessageC; i++)
    {
        string uid = to_string(i);
        if (this->uidl_alloved)
        {
            if (!this->SendMessage("UIDL "+to_string(i)+"\r\n"))
                return false;
            if (!this->ReadMessage())
            {
                return false;
            }
            uid = this->ParseUID();
            database.AddUID(uid);
        }
        if (!this->DownloadOne(i, true))
        {
            return false;
        }
        if (!this->uidl_alloved)
        {
            string messageID = getMessageID();
            if (!messageID.empty())
            {
                database.AddUID(messageID);
            }
        }
    }
    if (!database.IsUidsEmpty())
    {
        database.SaveUID();
    }
    return true;
}

void Pop3::ListProcess()
{
    //Parse message sizes from LIST command to vector
    this->StripMessage();
    bool stopWhile = true;
    while (stopWhile)
    {
        int size = 0;
        int id = 0;
        size_t found = this->message.find("\r\n");
        if (found != string::npos) {
            string tmp = message.substr(0, found);
            message = message.substr(found+2);
            this->MessageParser(id, size, tmp);
            this->sizesToDownload.push_back(size);
        }
        else
            stopWhile = false;
    }
}

void Pop3::MessageParser(int &messageID, int &size, string mess)
{
    //Parse ID and size for messages
    if (!mess.empty())
    {
        size_t found = mess.find(" ");
        if (found != string::npos)
        {
            try {
                messageID = stoi(mess.substr(0, found));
                size = stoi(mess.substr(found+1));
            }
            catch (...)
            {
                cerr << "Error: Exception server fail." << endl;
                return;
            }
        }
    }
}

int Pop3::StripMessage()
{
    //Get message from response
    int stripedBytes = 0;
    this->message = this->message.substr(0, this->message.size()-3);
    stripedBytes += 3;
    size_t found = this->message.find("\r\n");
    if (found != string::npos) {
        this->message = this->message.substr(found+2);
        stripedBytes += (int) found;
        stripedBytes += 2;
    }
    regex findDots("\r\n\\.\\.");
    int dots = (int)std::distance(sregex_iterator(this->message.begin(), this->message.end(), findDots), sregex_iterator());
    if (dots > 0)
    {
        this->message = regex_replace(this->message, findDots, "\r\n.");
        stripedBytes += dots;
    }
    return stripedBytes;
}

Pop3::~Pop3()
{
    //Close socket and clen BIO
    if (this->ctx != NULL)
    {
        SSL_CTX_free(this->ctx);
    }
    if (this->sslStarted)
    {
        SSL_free(this->ssl);
    }
    else
    {
        if (this->bio != NULL)
        {
             BIO_free_all(this->bio);
        }
    }
    if (this->serverCert != NULL)
    {
        X509_free(this->serverCert);
    }
    // SOURCE:
    // https://stackoverflow.com/questions/29008145/valgrind-shows-memory-leak-in-ssl-after-closing-the-connection
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    ERR_remove_state(0);
    EVP_cleanup();
}
