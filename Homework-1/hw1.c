#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define tcp_path "/proc/net/tcp"
#define tcp6_path "/proc/net/tcp6"


typedef struct connection
{
    char proto[6];
    char localAddress[50];
    char foreignAddress[50];
    char PIDProgram[100];
    struct connection* next;   
} connection;


// concatenate 2 strings
char* concatStrs(const char *s1, const char *s2)
{
    char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if (result == NULL)
    {
        fprintf(stderr, "Fatal: failed to allocate bytes.\n");
        abort();
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


// convert String to Port
char* strToPort(char* str)
{
    if (strlen(str) > 5)
    {
        perror("Port number is invalid!!!");
        exit(EXIT_FAILURE);
    }
    char* port = (char*) malloc(sizeof(char) * 6);
    unsigned int num = (unsigned int)strtol(str, NULL, 16);
    if(num == 0)
	    return "*";
    sprintf(port, "%d", num);
    return port;
}


// convert String to IP address
char* strToIp(char* str)
{
    struct in_addr inaddr;
    struct in6_addr in6addr;
    char* buf = (char*) malloc(sizeof(char) * INET_ADDRSTRLEN);
    char* buf6 = (char*) malloc(sizeof(char) * INET6_ADDRSTRLEN);

    // convert the hex-string to uint32
	inaddr.s_addr = (unsigned int)strtol(str, NULL, 16);
	if (inet_ntop(AF_INET, &inaddr.s_addr, buf, INET_ADDRSTRLEN) != NULL)
    	return buf;
    else 
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }
}


// parse a string to sub strings
char** parseStrToSubStrs(char* str, char* delim)
{
    char** result;
    int count = 0;
    char* tmp = strdup(str);

	char *ptr = strtok(tmp, delim);

    // count how many elements will be extracted
	while(ptr != NULL)
	{
        count ++;
		ptr = strtok(NULL, delim);
	}

    result = malloc(sizeof(char*) * count);

    // copy sub-string to the array
    if (result)
    {
        char *ptr = strtok(str, delim);
        int index = 0;
        while(ptr != NULL)
        {
            *(result + index++) = strdup(ptr);
            ptr = strtok(NULL, delim);
        }
    }
    return result;
}


// format IP address and port from hex-string to dec-string
char* formatIpAndPort(char* str)
{
    char* colon = ":";
    // split ip and port for local_address by ":"
    char** ipAndPort = parseStrToSubStrs(str, colon);
    char* ip = strToIp(ipAndPort[0]);
    char* port = strToPort(ipAndPort[1]);
    char* ipColon = concatStrs(ip, colon);
    char* ipColonPort = concatStrs(ipColon, port);
    if (strlen(ipColonPort) < 16) // 16 space ~ 2 tab
        ipColonPort = concatStrs(ipColonPort, "\t");
    return ipColonPort;
}


// print information
void printConnections(connection* connections)
{
    connection* tmp = connections;
    while (tmp != NULL)
    {
        printf("%s\t%s\t%s\t%s\n", tmp->proto, tmp->localAddress, tmp->foreignAddress, tmp->PIDProgram);
        tmp = tmp->next;
    }
}


// create a connection node
connection* createNode(char* proto, char* localAddress, char* foreignAddress, char* PIDProgram)
{
    // create a link
    connection *node = (connection*) malloc(sizeof(connection));
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


// dump a file (tcp/tcp6/upd/upd6)
connection* dumpFileToConnections(const char *fileName, connection* connections)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    char* space = " ";
    char* colon = ":";

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
        char* localAddress = formatIpAndPort(subString[1]);
        char* foreignAddress = formatIpAndPort(subString[2]);    
        // create a connection node
        connection* node = createNode("tcp", localAddress, foreignAddress, "pid");
        connections = insertEnd(connections, node);
    }

    fclose(file);
    if (line)
        free(line);
    
    return connections;
}


// handle TCP connections
connection* TCP_Handler()
{
    connection* TCPConnections = NULL;
    // read tcp connections
    TCPConnections = dumpFileToConnections("/proc/net/tcp", TCPConnections);
    return TCPConnections;
};


int main(int argc, char **argv[])
{
    connection* connections = initiateHead();    
    connections = insertEnd(connections, TCP_Handler());
    printConnections(connections);
    return 0;
}