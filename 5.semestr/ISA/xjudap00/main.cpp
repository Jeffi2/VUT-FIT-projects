/**
 * Project: ISA - Klient POP3 s podporou TLS
 * @author: Petr Jůda  (xjudap00)
 * @file: main.cpp
 *
 * Control main program flow.
 *
 */
#include <iostream>
#include "ParamsParser.h"
#include "Pop3.h"
#include "Database.h"
#include "Error.h"

int main(int argc, char **argv)
{
    int returnCode = 0;
    //Parse program parameters
    ProgramParams parameters = ProgramParams();
    if ((returnCode = parameters.ParseParams(argc, argv)) != 0)
        return returnCode;
    
    //Test if help should be printed
    if (parameters.hSet)
    {
        parameters.PrintHelp();
        return OK;
    }
    
    //Validate parameters (files and dir exists)
    if ((returnCode = parameters.ValidParams()) != 0)
        return returnCode;
    
    //Load username and password from AuthFile (check also if file format is correct)
    if (!parameters.LoadLoginPass())
    {
        return ERR_BAD_AUTH_FILE;
    }
    
    //Create connection and download e-mails
    Pop3 pop3 = Pop3(parameters);
    if ((returnCode = pop3.DownloadMessages()) != 0)
    {
        return returnCode;
    }
    return OK;
}
