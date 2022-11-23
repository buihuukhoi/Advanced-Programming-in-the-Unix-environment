#include "library.h"


int main(int argc, char **argv)
{
    char* filterStr = "";
    connection* tcpConnections = NULL;
    connection* udpConnections = NULL;

    int c;
    const char* optStr = "tu"; 
    static struct option long_options[] = 
    {
        {"tcp", no_argument, NULL, 't'},
        {"udp", no_argument, NULL, 'u'}
    };
    int optionIndex = 0;

    while ((c = getopt_long(argc, argv, optStr, long_options, &optionIndex)) != -1)
    {
        switch (c)
        {
        // tcp
        case 't':
            tcpConnections = initiateHead();
            tcpConnections = insertEnd(tcpConnections, connectionsHandler(true));
            break;
        // udp
        case 'u':
            udpConnections = initiateHead();
            udpConnections = insertEnd(udpConnections, connectionsHandler(false));
            break;
        case '?':
            printf("get error!!!\n");
            exit(EXIT_FAILURE);
        default:
            printf("default\n");
            break;
        }
    }

    // handle any remaining command line arguments (not options).
    if (optind < argc)
    {
        while (optind < argc)
        {
            filterStr = concatStrs(filterStr, argv[optind++]);
            filterStr = concatStrs(filterStr, " ");
        }
        filterStr[strlen(filterStr)-1] = '\0';
    }

    // no argument is given
    if ((tcpConnections == NULL) && (udpConnections == NULL))
    {
        // tcp
        tcpConnections = initiateHead();
        tcpConnections = insertEnd(tcpConnections, connectionsHandler(true));
        
        // udp
        udpConnections = initiateHead();
        udpConnections = insertEnd(udpConnections, connectionsHandler(false));
    }

    if (tcpConnections != NULL)
    {
        filterConnections(tcpConnections, filterStr);
        printf("List of TCP connections:\n");
        printConnections(tcpConnections);
    }
    if (udpConnections != NULL)
    {
        filterConnections(udpConnections, filterStr);
        printf("List of UDP connections:\n");
        printConnections(udpConnections);
    }
    
    return 0;
}