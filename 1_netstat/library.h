#ifndef LIBRARY_H_
#define LIBRARY_H_


#include <stdio.h>
#include <stdlib.h>
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


// define a truct to store a connection information
typedef struct connection
{
    char proto[6];
    char localAddress[50];
    char foreignAddress[50];
    char PIDProgram[1024];
    struct connection* next;   
} connection;


void handleFailingMalloc();

// concatenate 2 strings
char* concatStrs(const char *s1, const char *s2);

// convert String to Port
char* strToPort(char* str);

// convert String to IP address
char* strToIp(char* str, bool ipv4);

// get program name
char* getProgramName(char* str);

// parse a string to sub strings
char** parseStrToSubStrs(char* str, char* delim);

// format IP address and port from hex-string to dec-string
char* formatIpAndPort(char* str, bool ipv4);

// check whether a string contain only numbers
bool checkStrContainNums(char* str);

// get "PID/Program name and arguments" from an PID 
char* getPIDProcNameAndArg(char* PID);

// check whether an inode is matched with a PID
bool checkInodeMatchPID(char* inode, char* PID);

// get "PID/Program name and arguments" from an inode
char* inodeToPID(char* inode);

// check whether a string cointains a sub string
bool checkStrContainsSubStr(char *subStr, char *str);

// print connection information
void printConnections(connection* connections);

// filter connections
void filterConnections(connection* connections, char* str);

// create a connection node
connection* createNode(char* proto, char* localAddress, char* foreignAddress, char* PIDProgram);

// initiate a head note
connection* initiateHead();

// add second connections to tail of connections
connection* insertEnd(connection* connections1, connection* connections2);

// dump a file (tcp/tcp6/udp/udp6)
connection* dumpFileToConnections(char* connectionType, connection* connections);

// handle TCP connections
connection* connectionsHandler(bool tcpFlag);


#endif  // LIBRARY_H_