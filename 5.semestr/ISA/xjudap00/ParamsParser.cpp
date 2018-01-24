/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: ParamsParser.cpp
 *
 * ProgramParams class is responsible for parsing and validation of passed program parameters.
 *
 */
#include "ParamsParser.h"

ProgramParams::ProgramParams()
{
    this->port = 110;
    this->TSet = false;
    this->SSet = false;
    this->cSet = false;
    this->CSet = false;
    this->dSet = false;
    this->nSet = false;
    this->aSet = false;
    this->hSet = false;
    this->oSet = false;
    this->error = false;
    this->sSet = false;
    this->portSet = false;
}

void ProgramParams::PrintHelp()
{
    cout << "./popcl <server> -a <auth_file> -o <out_dir> [-p <port>] [-T|-S [-c <certfile>] [-C <certaddr]] [-d] [-n]" << endl;
    cout << "<server>       - compulsory parameter specifying server URL or IPv4/IPv6 address" << endl;
    cout << "-a <auth_file> - compulsory parameter specifying file with user name and password" << endl;
    cout << "-o <out_dir>   - compulsory parameter specifying output dir for downloaded messages" << endl;
    cout << "-p <port>      - optional parameter specifying custom port number (default is 995 for TSL and 110 for unsec and STARTTLS)" << endl;
    cout << "-T             - optional parameter specifying to start an encrypted communication" << endl;
    cout << "-S             - optional parameter specifying to srart an unencrypted communication and then switch to encrypted" << endl;
    cout << "-c             - optional parameter specifying custom certificate file (can be used only with -S or -T)" << endl;
    cout << "-C             - optional parameter specifying custom folder with cerificates (can be used only with -S or -T)" << endl;
    cout << "-d             - optional parameter specifying to delete messages on server after download" << endl;
    cout << "-n             - optional parameter specifying to download only new messages" << endl;
}

int ProgramParams::ParseParams(int argc, char** argv)
{
    // Only -h set
    if (argc == 1)
    {
        this->hSet = true;
        return OK;
    }
   
    //Test all args
    for (int i = 1; i < argc; i++) {
        if (this->error) {
            break;
        }
        
        //Port set
        if (strcmp(argv[i], "-p") == 0)
        {
            if (this->portSet)
            {
                this->error = true;
                break;
            }
            if (i+1 > argc)
            {
                this->error = true;
            }
            else
            {
                this->portSet = true;
                string tmp = argv[i+1];
                try {
                    this->port = std::stoi(tmp);
                    i++;
                } catch (...) {
                    this->error = true;
                    break;
                }
                if (this->port <= 0 || this->port > 65535)
                {
                    cerr << "Error: Bad port numer. (range 0 - 65535)" << endl;
                    return ERR_BAD_PARAMS;
                }
            }
        }
        //Auth file set
        else if (strcmp(argv[i], "-a") == 0)
        {
            if (this->aSet)
            {
                this->error = true;
                break;
            }
            if (i+1 > argc)
            {
                this->error = true;
            }
            else
            {
                this->aSet = true;
                this->authFile = argv[i+1];
                i++;
            }
        }
        //Outdir set
        else if (strcmp(argv[i], "-o") == 0)
        {
            if (this->oSet)
            {
                this->error = true;
                break;
            }
            if (i+1 > argc)
            {
                this->error = true;
            }
            else
            {
                this->oSet = true;
                this->outDir = argv[i+1];
                i++;
            }
        }
        //Delete messages set
        else if (strcmp(argv[i], "-d") == 0)
        {
            if (this->dSet)
            {
                this->error = true;
                break;
            }
            else
            {
                this->dSet = true;
            }
        }
        //Download only new set
        else if (strcmp(argv[i], "-n") == 0)
        {
            if (this->nSet)
            {
                this->error = true;
                break;
            }
            else
            {
                this->nSet = true;
            }
        }
        //STARTTLS set
        else if (strcmp(argv[i], "-S") == 0)
        {
            if (!portSet)
            {
                this->port = 110;
            }
            if (this->SSet)
            {
                this->error = true;
                break;
            }
            else
            {
            this->SSet = true;
            }
        }
        //Certification file set
        else if (strcmp(argv[i], "-c") == 0)
        {
            if (this->cSet)
            {
                this->error = true;
                break;
            }
            if (i+1 > argc)
            {
                this->error = true;
                }
                else
                {
                    this->cSet = true;
                    this->certFile = argv[i+1];
                    i++;
                }
        }
        //Certification folder set
        else if (strcmp(argv[i], "-C") == 0)
        {
            if (this->CSet)
            {
                this->error = true;
                break;
            }
            if (i+1 > argc)
            {
                this->error = true;
            }
            else
            {
                this->CSet = true;
                this->certAddr = argv[i+1];
                i++;
            }
        }
        //TLS communication set
        else if (strcmp(argv[i], "-T") == 0)
        {
            if (!portSet)
            {
                this->port = 995;
            }
            if (this->TSet)
            {
                this->error = true;
                break;
            }
            else
            {
                this->TSet = true;
            }
        }
        //Help
        else if (strcmp(argv[i], "-h") == 0)
        {
            this->hSet = true;
            return OK;
        }
        //Help
        else if (strcmp(argv[i], "--help") == 0)
        {
            this->hSet = true;
            return OK;
        }
        //Server or ip
        else if (this->sSet)
        {
            this->error = true;
            break;
        }
        else
        {
            this->sSet = true;
            this->server = argv[i];
        }
    } // end of for
    
    //Minimal configuration
    if (!sSet || !aSet || !oSet)
    {
        this->error = true;
    }
    //Certaddr or certfile only with -T or -S
    if ((cSet || CSet) && !(TSet || SSet))
    {
        this->error = true;
    }
    //-T and -S together
    if (TSet && SSet)
    {
        this->error = true;
    }

    //Error catch
    if (this->error)
    {
        cerr << "Error: Bad program parameters. Usage:" << endl;
        this->PrintHelp();
        return ERR_BAD_PARAMS;
    }
    return OK;
}

//TODO delete
/*
void ProgramParams::PrintParamsSetting()
{
    cout << "sSet: " << this->sSet << endl;
    cout << "Server nebo IP: " << this->server << endl;
    cout << "Port set: " << this->portSet << endl;
    cout << "Port: " << this->port << endl;
    cout << "aSet: " << this->aSet << endl;
    cout << "authfile: " << this->authFile << endl;
    cout << "oSet: " << this->oSet << endl;
    cout << "outdir: " << this->outDir << endl;
    cout << "TSet: " << this->TSet << endl;
    cout << "SSet: " << this->SSet << endl;
    cout << "cSet: " << this->cSet << endl;
    cout << "certfile: " << this->certFile << endl;
    cout << "certdir: " << this->certAddr << endl;
    cout << "CSet: " << this->CSet << endl;
    cout << "dSet: " << this->dSet << endl;
    cout << "nSet: " << this->nSet << endl;
    cout << "Error: " << this->error << endl;
}
 */

int ProgramParams::isDir(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int ProgramParams::isFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

string ProgramParams::convertRelativePath (string pathToFile)
{
    char absolutPath[PATH_MAX + 1];
    char *testPath = realpath(pathToFile.c_str(), absolutPath);
    if (!testPath)
    {
        //This is safe way
        return "BAD!";
    }
    return absolutPath;
}

bool ProgramParams::fileOrDirExist (const char* path)
{
    if (ifstream(path))
    {
        return true;
    }
    return false;
}

bool ProgramParams::testDirFile(string *path)
{
    //Check absolut path
    string absolutPathToRoot = this->convertRelativePath(*path);
    if (absolutPathToRoot == "BAD!")
    {
        return false;
    }
    *path = absolutPathToRoot;
    //Check if exists
    if (this->fileOrDirExist(absolutPathToRoot.c_str()))
    {
        return true;
    }
    return false;
}

int ProgramParams::ValidParams()
{
    //Auth file is real
    if (testDirFile(&authFile))
    {
        if (!isFile(this->authFile.data())) {
            this->error = true;
        }
    }
    else
    {
        this->error = true;
    }
    
    //Out dir exists
    if (testDirFile(&outDir))
    {
        if (!isDir(this->outDir.data())) {
            this->error = true;
        }
        this->outDir = convertRelativePath(outDir);
    }
    else
    {
        this->error = true;
    }
    
    if (cSet) {
        //Certification file exits
        if (testDirFile(&certFile))
        {
            if (!isFile(this->certFile.data())) {
                this->error = true;
            }
        }
        else
        {
            this->error = true;
        }
    }
    
    if (CSet) {
        //Certificate folder exist
        if (testDirFile(&certAddr))
        {
            if (!isDir(this->certAddr.data())) {
                this->error = true;
            }
        }
        else
        {
            this->error = true;
        }
    }

    if (this->error) {
        cout << "Error: File or Dir does not exists!" << endl;
        return ERR_FILE_DIR_NOT_EXISTS;
    }
    return OK;
}

bool ProgramParams::LoadLoginPass()
{
    ifstream file;
    string read;
    file.open(this->authFile);
    if (!file.is_open()) {
        cerr << "Error: Problem with authFile open!" << endl;
        return false;
    }
    
    getline(file, read);
    //Find username and test possition
    if (read.find("username = ") == string::npos || (read.find("username = ") != 0))
    {
        this->error = true;
    }
    else
    {
        this->login = read.substr(11);
        if (this->login == "")
        {
            cerr << "Error: empty login name!" << endl;
            this->error = true;
        }
    }
    
    getline(file, read);
    //Find password and test possition
    if (read.find("password = ") == string::npos || (read.find("password = ") != 0))
    {
        this->error = true;
    }
    else
    {
        this->password = read.substr(11);
    }
   
    //Catch error
    if (this->error)
    {
        cerr << "Error: Bad authFile structure!" << endl;
        file.close();
        return false;
    }
    file.close();
    return true;
}

string ProgramParams::getServer()
{
    return this->server;
}

int ProgramParams::getPort()
{
    return this->port;
}

string ProgramParams::getLogin()
{
    return this->login;
}

string ProgramParams::getPass()
{
    return this->password;
}

string ProgramParams::getOutDir()
{
    return this->outDir;
}

string ProgramParams::getCertDir()
{
    return this->certAddr;
}

string ProgramParams::getCertFile()
{
    return this->certFile;
}
