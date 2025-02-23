//Author  : Group#2
//Date    : 03/12/2022
//Version : 2.0
//Filename: main.cpp

/**
 * This is CPSC 5042 Group 2 Server application. The server application holds
 * the logic for a Calculator. Clients can connect to this server to perform
 * mathematical calculations such as calculate expressions (+, -, /, *, *),
 * calculate statistics for a set of numbers, or convert between decimal,
 * hexadecimal and binary numbers.
 *
 * This file contains the 'main' function for the Server application. Program
 * execution begins and ends here.
*/

#include <cstdio>
#include <iostream>
#include "RPCServer.h"


/**
 * The main() function starts the server applications, waits for client
 * connection, then process RPCs from the client.
 *
 * This is an endless server, thus will not exit unless it hits an error,
 * or terminated from the OS
 *
 * @param argc An int containing the number input arguments
 * @param argv A char array containing the command line arguments.
 *              Expecting IP Address, port number (e.g. 127.0.0.1 8080)
 * @return An int value with the exit status
 */
int main(int argc, char const* argv[])
{
    cout << "************************************************" << endl;
    cout << "*   Welcome to the Group 2 Server Application  *" << endl;
    cout << "************************************************" << endl;

    //check if user entered correct # of Command Line args for IP and Port
    if (argc < 3)
    {
        //If insufficient number of args, print error and exit program.
        cout << "\nERROR: Missing IP Address or Port number.\n";
        cout << "Exiting Server Application...\n";
        return -1;
    }

    //Extract serverIP and port numbers from command line arguments
    const char* serverIP = argv[1];
    int port = atoi(argv[2]);

    //Declare a bool flag to track the status of the system
    bool statusOk = true;

    //Declare  and init server object variable to handle the server operations
    RPCServer* serverObj = new RPCServer(serverIP, port);

    //Get server Status
    statusOk = serverObj->StartServer();

    //Print Server Status
    if (statusOk)
    {
        cout << "\nServer is now running\n" << endl;
    }
    else
    {
        cout << "\nERROR: Server failed to start\n" << endl;
    }

    //While loop listening to clients' requests for connection
    while (statusOk)
    {
        //printf("\nWaiting...\n");
        statusOk = serverObj->ListenForClient();

    }

    //delete the serverObj and free up its memory
    delete serverObj;
    serverObj = nullptr;

    //Return success to OS
    return 0;
}