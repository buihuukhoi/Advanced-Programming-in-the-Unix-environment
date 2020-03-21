#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#define tcp "/proc/net/tcp"
#define tcp6 "/proc/net/tcp6"


typedef struct connection
{
    char proto[5];
    char localAddress[50];
    char foreingAddress[50];
    char PIDProgram[100];
    struct connection* next;   
} connection;


// print information
void printConnections(connection* connections)
{
    connection* tmp = connections;
    while (tmp != NULL)
    {
        printf("%s\t%s\t%s\t%s\n", tmp->proto, tmp->localAddress, tmp->foreingAddress, tmp->PIDProgram);
        tmp = tmp->next;
    }
}


// create a connection node
connection* createNode(char* proto, char* localAddress, char* foreingAddress, char* PIDProgram)
{
    // create a link
    connection *node = (connection*) malloc(sizeof(connection));
    strcpy(node->proto, proto);
    strcpy(node->localAddress, localAddress);
    strcpy(node->foreingAddress, foreingAddress);
    strcpy(node->PIDProgram, PIDProgram);
    node->next = NULL;
 
    return node;
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


char** parseString2SubStrings(char* str)
{
    char** result;
    int count = 0;
    char* tmp = strdup(str);
    char delim[] = " ";

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


connection* dumpFile2Connections(const char *fileName, connection* connections)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    file = fopen(fileName, "r");
    if (file == NULL)
        exit(EXIT_FAILURE);

    //read line by line
    while ((read = getline(&line, &len, file)) != -1) {
        char** subString = parseString2SubStrings(line);
        // create a connection node
        connection* node = createNode("tcp", subString[1], subString[2], "pid");
        connections = insertEnd(connections, node);
    }

    fclose(file);
    if (line)
        free(line);
    
    return connections;
}


connection* TCP_Handler()
{
    // find length first
    connection* connections = NULL;
    // read tcp connections
    connections = dumpFile2Connections("/proc/net/tcp", connections);
    return connections;
};


int main(int argc, char **argv[])
{
    connection* connections = NULL;
    connections = insertEnd(connections, TCP_Handler());
    printConnections(connections);
    return 0;
}