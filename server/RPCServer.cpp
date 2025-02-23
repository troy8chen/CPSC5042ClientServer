//Author  : Group#2
//Date    : 03/12/2022
//Version : 2.0
//Filename: RPCServer.cpp

#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <vector>
#include "RPCServer.h"
#include "ClientHandler.h"
#include <iostream>
#include "pthread.h"

using namespace std;

/**
 * GLOBAL VARIABLES
 */

//Mutex for global context
pthread_mutex_t g_contextLock;

//Mutex to protect console from printing race conditions
pthread_mutex_t g_screenLock;

struct GlobalContext g_globalContext;

const static string g_credentials = "credentials.csv";

/**
 * Constructor
 */
RPCServer::RPCServer(const char *serverIP, int port)
{
    //Initialize member elements
    m_serverIP = (char *) serverIP;
    m_port = port;
    int mutex_init_code = pthread_mutex_init(&g_contextLock, NULL);
    if (mutex_init_code != 0)
    {
        throw new runtime_error(MUTEX_INIT_FAIL);
    }
    mutex_init_code = pthread_mutex_init(&g_screenLock, NULL);
    if (mutex_init_code != 0)
    {
        throw new runtime_error(MUTEX_INIT_FAIL);
    }
}

/**
 * Destructor
 */
RPCServer::~RPCServer()
{
   pthread_mutex_destroy(&g_contextLock);
}

/**
 * StartServer will create a server on a Port that was passed in, and create a socket
 */
bool RPCServer::StartServer()
{
    int opt = 1;
    const int BACKLOG = 1000;

    // Creating socket file descriptor
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port passed in to the constructor
    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    // Forcefully attaching socket to the port passed in to the constructor
    if (bind(m_server_fd, (struct sockaddr*)&m_address, sizeof(m_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(m_server_fd, BACKLOG) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    return true;
}

/**
 * A function to execute a thread
 */
void* startThread(void* input) {
    //sleep(1);

    //Get socket number from input
    auto socket = *(int *) input;

    //Create a new client handler object to handle incoming RPCs
    ClientHandler *cHandler = new ClientHandler(socket, g_credentials);


    //Process incoming RPCs
    try
    {
        cHandler->ProcessRPC(&g_contextLock, &g_screenLock,
                             &g_globalContext);
    }

    //if exception is thrown, print the faulting socket
    catch(exception &e)
    {
        pthread_mutex_lock(&g_screenLock);
        printf("Exception on Socket %d", socket);
        pthread_mutex_unlock(&g_screenLock);
        close(socket);
    }

    //Memory cleanup
    delete cHandler;
    cHandler = nullptr;

    return nullptr;
}

/**
 * Will accept a new connection by listening on its address
 *
 */
bool RPCServer::ListenForClient()
{
    int addrlen = sizeof(m_address);

    //Listen to client and accept connection. If accept call fails print error
    if ((m_socket = accept(m_server_fd, (struct sockaddr*)&m_address,
        (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        //create thread object
        pthread_t thread_id;
        pthread_create(&thread_id, nullptr, startThread, (void*)&m_socket);
        pthread_detach(thread_id);
    }

    return true;
}
