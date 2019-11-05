#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define PORT 8080

struct arg_struc {
    char* arg1;
    int arg2;
};

char* getrequest(char* buf);// parse the request and get the file name we want.
void deal_with_request(struct arg_struc* argument);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    pthread_t id;
//    char *file_name = NULL;
//    char file_content[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))>=0) {
        read( new_socket , buffer, 1024);
        struct arg_struc args;
        args.arg1 = buffer;
        args.arg2 = new_socket;
        pthread_create(&id, NULL, (void*) deal_with_request, &args);
        pthread_join(id, NULL);
    }
    return 0;
}

char* getrequest(char* buf) {
    char request[100];
    while(*buf != '/') {
        buf++;
    }
    int i = 0;
    buf++;
    while(*buf != ' ') {
        request[i] = *buf;
        i++;
        buf++;
    }
    request[i]='\0';
    for(int j = 0; j<100; j++) {
        buf[j] = request[j];
    }
    return buf;
}

void deal_with_request(struct arg_struc* argument) {
    char* buffer = argument->arg1;
    int new_socket = argument->arg2;
    char *file_name = NULL;
    char file_content[1024] = {0};
    file_name = getrequest(buffer);
    FILE* file = fopen(file_name,"rb");
    if(file != NULL) {
        fread(file_content, sizeof(char), 1024, file);
        send(new_socket , file_content , strlen(file_content) , 0 );
        printf("request file name: %s\n",file_name);
    } else {
        file = fopen("Not_found.html", "rb");
        fread(file_content, sizeof(char), 1024, file);
        send(new_socket , file_content, strlen(file_content) , 0 );
        printf("request file name: %s\n",file_name);
        printf("the file not found\n");
    }
}
