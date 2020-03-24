#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <getopt.h>
#include <regex.h>


#define connectionsPath "/proc/net/"
#define tcp "tcp"
#define tcp6 "tcp6"
#define udp "udp"
#define udp6 "udp6"

char proc[] = "/proc";
char cmdline[] = "/cmdline";
char fd[] = "/fd";
char slash[] = "/";


// define a truct to store a connection information
typedef struct connection
{
    char proto[6];
    char localAddress[50];
    char foreignAddress[50];
    char PIDProgram[1024];
    struct connection* next;   
} connection;


void handleFailingMalloc()
{
    fprintf(stderr, "Fatal: failed to allocate bytes!\n");
    // abort();
    exit(EXIT_FAILURE);
}


// concatenate 2 strings
char* concatStrs(const char *s1, const char *s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if (result == NULL)
        handleFailingMalloc();
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


// convert String to Port
char* strToPort(char* str)
{
    if (strlen(str) > 5)
    {
        perror("Port number is invalid!");
        exit(EXIT_FAILURE);
    }
    char* port = (char*) malloc(sizeof(char) * 6);
    if (port == NULL)
        handleFailingMalloc();
    unsigned int num = (unsigned int)strtol(str, NULL, 16);
    if(num == 0)
	    return "*";
    sprintf(port, "%d", num);
    return port;
}


// convert String to IP address
char* strToIp(char* str, bool ipv4)
{
    if (ipv4)
    {
        struct in_addr inaddr;
        char* buf = (char*) malloc(sizeof(char) * INET_ADDRSTRLEN);
        if (buf == NULL)
            handleFailingMalloc();
        
        // convert the hex-string to uint32
        inaddr.s_addr = (uint32_t)strtol(str, NULL, 16);
        if (inet_ntop(AF_INET, &inaddr.s_addr, buf, INET_ADDRSTRLEN) != NULL)
            return buf;
        else 
        {
            perror("inet_ntop");
            exit(EXIT_FAILURE);
        }
    }

    else
    {
        struct in6_addr in6addr;
        char* buf6 = (char*) malloc(sizeof(char) * INET6_ADDRSTRLEN);
        if (buf6 == NULL)
            handleFailingMalloc();
        
        for (int i=0; i<4; i++)
        {
            char subStr[9];
            memcpy(subStr, &str[8*i], 8);
            subStr[9] = '\0';
            for(int j=0; j<4; j++)
            {
                char sub[3];
                memcpy(sub, &subStr[2*j], 2);
                sub[3] = '\0';
                // convert the hex-string to uint8
                in6addr.s6_addr[4*i + 3 - j] = (uint8_t)strtol(sub, NULL, 16);
            } 
        }
    	if (inet_ntop(AF_INET6, &in6addr.s6_addr, buf6, INET6_ADDRSTRLEN) != NULL)
            return buf6;
        else 
        {
            perror("inet_ntop");
            exit(EXIT_FAILURE);
        }
    }
}


// parse a string to sub strings
char** parseStrToSubStrs(char* str, char* delim)
{
    char** result;
    int count = 0;
    char* tmp = (char*) malloc(sizeof(char) * strlen(str) + 1);
    if (tmp == NULL)
        handleFailingMalloc();
    strcpy(tmp, str);

	char *ptr = strtok(tmp, delim);

    // count how many elements will be extracted
	while(ptr != NULL)
	{
        count ++;
		ptr = strtok(NULL, delim);
	}

    result = malloc(sizeof(char*) * count);
    if (result == NULL)
        handleFailingMalloc();
    // copy sub-string to the array
    else    
    {
        char *ptr = strtok(str, delim);
        int index = 0;
        while(ptr != NULL)
        {
            char* tmpResult = (char*) malloc(sizeof(char) * strlen(ptr) + 1);
            if (tmpResult == NULL)
                handleFailingMalloc();
            strcpy(tmpResult, ptr);
            *(result + index++) = tmpResult;
            ptr = strtok(NULL, delim);
        }
    }
    return result;
}


// format IP address and port from hex-string to dec-string
char* formatIpAndPort(char* str, bool ipv4)
{
    char* colon = ":";
    // split ip and port for local_address by ":"
    char** ipAndPort = parseStrToSubStrs(str, colon);
    char* ip = strToIp(ipAndPort[0], ipv4);
    char* port = strToPort(ipAndPort[1]);
    char* ipColon = concatStrs(ip, colon);
    char* ipColonPort = concatStrs(ipColon, port);
    if (strlen(ipColonPort) < 8)
        ipColonPort = concatStrs(ipColonPort, "\t\t");
    else if (strlen(ipColonPort) < 16) // 16 space ~ 2 tab
        ipColonPort = concatStrs(ipColonPort, "\t");
    return ipColonPort;
}


// check whether a string contain only numbers
bool checkStrContainNums(char* str)
{
    int n = strlen(str);
    for (int index=0; index<n; index++)
        if (str[index] < 48 || str[index] > 57)
            return false;
    return true;
}


// // get "PID/Program name and arguments" from an PID 
char* getPIDProcNameAndArg(char* PID)
{
    char* result = PID;

    char cmdlinePath[50];
    strcpy(cmdlinePath, proc);
    strcat(cmdlinePath, slash);
    strcat(cmdlinePath, PID);
    strcat(cmdlinePath, cmdline); // example: /proc/[PID]/cmdline

    // open and read lines in a file
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(cmdlinePath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1) {
        result = concatStrs(result, line);
    }

    if (line)
        free(line);
    fclose(fp);

    return result;
}


// check whether an inode is matched with a PID
bool checkInodeMatchPID(char* inode, char* PID)
{
    bool result = false;
    char fdPath[50];
    strcpy(fdPath, proc);
    strcat(fdPath, slash);
    strcat(fdPath, PID);
    strcat(fdPath, fd);

    // open a directory
    DIR* pDir;
    pDir = opendir(fdPath);
    if (pDir == NULL)
    {
            printf ("Cannot open directory %s, try checking permission OR this directory is blocked by other OR this process is killed!\n", fdPath);
            exit(EXIT_FAILURE);
    }

    // read each entry into the directory that opened above
    struct dirent* pDirEntry;
    while ((pDirEntry = readdir(pDir)) != NULL)
    {
        char* name = pDirEntry->d_name;
        if (checkStrContainNums(name))
        {
            char path[50];
            strcpy(path, fdPath);
            strcat(path, slash);
            strcat(path, name); // example: /proc/[PID]/fd/[numbers]

            //read a Synbolic Link
            char bufSymbolicLink[256] = "";
            readlink(path, bufSymbolicLink, sizeof(bufSymbolicLink));
            if (strstr(bufSymbolicLink, inode) != NULL)
            {
                result = true;
                break;
            }
        }
    }

    closedir(pDir);

    return result;
}


// get "PID/Program name and arguments" from an inode
char* inodeToPID(char* inode)
{
    char* result = "";

    // open a directory
    DIR* pDir; // pointer directory
    pDir = opendir(proc);
    if (pDir == NULL)
    {
            printf ("Cannot open directory %s!\n", proc);
            exit(EXIT_FAILURE);
    }

    // read each entry into the directory that opened above
    struct dirent* pDirEntry; // pointer for directory entry
    while ((pDirEntry = readdir(pDir)) != NULL)
    {
        char* name = pDirEntry->d_name; // PID
        if (checkStrContainNums(name) && checkInodeMatchPID(inode, name))
        {
            result = getPIDProcNameAndArg(name);
            break;
        }
    }

    closedir(pDir);

    return result;
}


// check regular expression
bool checkRegex(char *regex, char *str)
{
    bool result;

    regex_t pregex;
    int reti;
    char msgbuf[100];

    /* Compile regular expression */
    reti = regcomp(&pregex, regex, 0);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(EXIT_FAILURE);
    }

    /* Execute regular expression */
    reti = regexec(&pregex, str, 0, NULL, 0);
    if (!reti) {
        return true;
    }
    else if (reti == REG_NOMATCH) {
        return false;
    }
    else {
        regerror(reti, &pregex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        exit(EXIT_FAILURE);
    }

    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&pregex);

    return result;
}


// print connection information
void printConnections(connection* connections)
{
    connection* tmp = connections;
    while (tmp != NULL)
    {
        printf("%s\t%s\t%s\t%s\n", tmp->proto, tmp->localAddress, tmp->foreignAddress, tmp->PIDProgram);
        tmp = tmp->next;
    }
    printf("\n");
}


// filter connections
void filterConnections(connection* connections, char* str)
{
    connection* previousNode;
    connection* tmp = connections;
    if (tmp != NULL)
    {
        previousNode = tmp;
        tmp = tmp->next;
    }
    while (tmp != NULL)
    {
        if (!checkRegex(str, tmp->proto)
            && !checkRegex(str, tmp->localAddress)
            && !checkRegex(str, tmp->foreignAddress)
            && !checkRegex(str, tmp->PIDProgram))
        {
            previousNode->next = tmp->next;
        }
        else
            previousNode = tmp; // same with previous->next
        tmp = tmp->next;
    }
}


// create a connection node
connection* createNode(char* proto, char* localAddress, char* foreignAddress, char* PIDProgram)
{
    // create a link
    connection* node = (connection*) malloc(sizeof(connection));
    if (node == NULL)
        handleFailingMalloc();
    strcpy(node->proto, proto);
    strcpy(node->localAddress, localAddress);
    strcpy(node->foreignAddress, foreignAddress);
    strcpy(node->PIDProgram, PIDProgram);
    node->next = NULL;
 
    return node;
}


// initiate a head note
connection* initiateHead()
{
    char proto[] = "Proto";
    char localAddress[] = "Local Address\t";
    char foreignAddress[] = "Foreign Address\t";
    char PID[] = "PID/Program name and arguments";

    return createNode(proto, localAddress, foreignAddress, PID);
}


// add second connections to tail of connections
connection* insertEnd(connection* connections1, connection* connections2)
{
    if (connections1 == NULL)
    {
        return connections2;
    }
    else
    {
        // find tail
        connection* currentConnection = connections1;
        while (currentConnection->next != NULL)
        {
            currentConnection = currentConnection->next;
        }
        // add node to tail
        currentConnection->next = connections2;
    }
    return connections1;
}


// dump a file (tcp/tcp6/udp/udp6)
connection* dumpFileToConnections(char* connectionType, connection* connections)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    char* space = " ";
    bool ipv4 = true;

    if (strstr(connectionType, "6") != NULL)
        ipv4 = false;

    char* fileName = (char*) malloc(sizeof(char) * (strlen(connectionsPath) + strlen(connectionsPath) + 1));
    if (fileName == NULL)
        handleFailingMalloc();
    strcpy(fileName, connectionsPath);
    strcat(fileName, connectionType);

    file = fopen(fileName, "r");
    if (file == NULL)
        exit(EXIT_FAILURE);

    // read the first line
    read = getline(&line, &len, file);
    // read line by line
    while ((read = getline(&line, &len, file)) != -1) {
        // split line by space
        char** subString = parseStrToSubStrs(line, space);
        // format ip and port address
        char* localAddress = formatIpAndPort(subString[1], ipv4);
        char* foreignAddress = formatIpAndPort(subString[2], ipv4);    
        // create a connection node
        connection* node = createNode(connectionType, localAddress, foreignAddress, inodeToPID(subString[9]));
        connections = insertEnd(connections, node);
    }

    fclose(file);
    if (line)
        free(line);
    
    return connections;
}


// handle TCP connections
connection* connectionsHandler(bool tcpFlag)
{
    connection* TCPConnections = NULL;
    if (tcpFlag)
    {
        // read tcp connections
        TCPConnections = dumpFileToConnections(tcp, TCPConnections);
        TCPConnections = dumpFileToConnections(tcp6, TCPConnections);
    }
    else
    {
        // read udp connections
        TCPConnections = dumpFileToConnections(udp, TCPConnections);
        TCPConnections = dumpFileToConnections(udp6, TCPConnections);
    }
    return TCPConnections;
};


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
            printf("get error\n");
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