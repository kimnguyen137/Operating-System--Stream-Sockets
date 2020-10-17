#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
using namespace std;

// Struct to save 3 different information 
struct weather
{
    string city, temp, sky;
};

void error(char *msg)
{
    perror(msg);
    exit(1);
}
int main ()
{
    int sockfd, newsockfd, portno, clilen, n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    // Read text file by line and put into vector using stream
    ifstream inFile ("weather20.txt");
    if (!inFile)
    {
        cout << "ERROR, cannot open text file!" << endl;
        return -1;
    }    
    // Use vector of struct to read different part from line, separate by ","
    vector<weather> vecOfStr;
    string line;
    string x,y,z;
    while (getline(inFile, x, ',') && getline(inFile, y, ',') && getline(inFile, z))
    {
        weather xxx;
        xxx.city=x; xxx.temp=y; xxx.sky=z;
        vecOfStr.push_back(xxx);
    }
    inFile.close(); 
    // Enter port number for server, check validation
    cout << "Enter server port number: " ;
    cin >> portno; 
    if (portno < 1024) // port number must greater than 1024
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create stream socket
    if (sockfd < 0) 
        error((char *) "ERROR opening socket");
    // Bind stream socket to specified port number
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error((char *) "ERROR on binding");
    listen(sockfd,5); // Specify maximum 5 queued connection requests
    clilen = sizeof(cli_addr);

    // Return information that client request from specific city, print out in server and send back to client
    while (1)
    {
        // Wait for connection requests from client
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0) 
            error((char *)"ERROR on accept");
        // Read socket from client message
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) 
            error((char *)"ERROR reading from socket");
            printf("Weather report for %s\n",buffer);
        // Reply information from server to client, by string
        string reply;
        int check = 0; // Check client request city information validation
        for (int i=0; i < vecOfStr.size(); i++)
        {        
            if (buffer ==  vecOfStr[i].city)
            {
                check ++;
                reply = "Tomorrow's maximum temperature is " + vecOfStr[i].temp + "\n" +
                        "Tomorrow's sky condition is " + vecOfStr[i].sky + "\n";
                cout << reply;        
            }           
        }
        if (check == 0) // If city not valid, return no data
        {
            cout << "No data" << endl;
            reply = "No data\n" ;
        }
        // Write to socket  
        n = write(newsockfd,reply.c_str(),256);
        if (n < 0) 
            error((char *)"ERROR writing to socket");
        close(newsockfd);
        }
    return 0; 
}
