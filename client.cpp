#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main ()
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    string hostname;
    
    // Get host name into server, as a const char
    cout << "Enter host name: " ;
    cin >> hostname;
    const char *hname = hostname.c_str();
    server = gethostbyname(hname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    // Get port number 
    cout << "Enter port number: ";
    cin >> portno;
    //portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error((char *)"ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error((char *)"ERROR connecting");
    
    // Get city name request from client, use ignore to ignore the Enter from cin then write to socket
    bzero(buffer,256);
    printf("Enter city name: ");
    cin.ignore();
    cin.getline(buffer, sizeof(buffer));
    //fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error((char *)"ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error((char *)"ERROR reading from socket");
    printf("%s",buffer);
    return 0;

}
