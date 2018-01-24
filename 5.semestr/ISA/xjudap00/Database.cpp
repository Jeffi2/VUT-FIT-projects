/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: Database.cpp
 *
 * Class responsible saving UID to database file. For downloading only new messages.
 *
 */
#include "Database.h"

UIDdatabase::UIDdatabase(ProgramParams params)
{
    this->params = params;
    this->databaseFolder = "./database";
}

bool UIDdatabase::CreateDir()
{
    if (!this->params.testDirFile(&databaseFolder))
    {
        if (mkdir(databaseFolder.c_str(), 0700) != 0)
        {
            cerr << "Error: Can not create a database folder." << endl;
            return false;
        }
    }
    return true;
}

bool UIDdatabase::IsUidsEmpty()
{
    return this->uids.empty();
}

bool UIDdatabase::SaveUID()
{
    ofstream outfile;
    outfile.open(this->fileLink);
    if (!outfile.is_open())
    {
        cerr << "Error: Can not create output file!" << endl;
        return false;
    }
    for (int i = 0; i < (int)this->uids.size(); i++)
    {
        outfile << this->uids.at(i);
        outfile << '\n';
    }
    outfile.close();
    return true;
}

void UIDdatabase::AddUID(string uid)
{
    if (!this->FindUID(uid))
    {
        this->uids.push_back(uid);
    }
}

bool UIDdatabase::FindUID(string uid)
{
    for (int i = 0; i < (int)this->uids.size(); i++) {
        if (uids.at(i) == uid)
        {
            return true;
        }
    }
    return false;
}

bool UIDdatabase::LoadFile(string username, string ip)
{
    this->filename = username + "_" + ip;
    this->fileLink = "./database/" + filename;
    if (this->params.testDirFile(&fileLink))
    {
        ifstream in(this->fileLink);
        stringstream strbuff;
        while (in >> strbuff.rdbuf());
        this->fileContent = strbuff.str();
        in.close();
        return true;;
    }
    return false;
}

void UIDdatabase::ParseUID()
{
    bool run = true;
    while (run)
    {
        size_t found = fileContent.find("\n");
        if (found == string::npos)
        {
            run = false;
        }
        else
        {
            string uid = fileContent.substr(0, found);
            fileContent = fileContent.substr(found+1);
            this->uids.push_back(uid);
        }
    }
}
