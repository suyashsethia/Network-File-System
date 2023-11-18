#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "list.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"


void list()
{
    char path[MAX_PATH_LENGTH + 1];
    printf("Enter Path :");
    if (fgets(path, sizeof(path), stdin) == NULL) {
        return;
    }
    size_t path_size = strlen(path);
    path[path_size - 1] = '\0'; // set newline to null

    struct sockaddr_in nm_address = {
        .sin_family=AF_INET,
        .sin_port=htons(NM_CLIENT_HANDLER_SERVER_PORT),
        .sin_addr={
            .s_addr=inet_addr(NM_CLIENT_HANDLER_SERVER_IP),
        }
    };
    int connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        log_errno_error("Couldn't create socket: %s\n");
        return;
    }

    if (connect(connection_socket, (struct sockaddr *) &nm_address, sizeof(nm_address)) == -1) {
        log_errno_error("Couldn't connect to ss: %s\n");
        return;
    }
    // SENDING LIST REQUEST WITH THE PATH 
    if (send_list_request(connection_socket, path) == -1) {
        log_errno_error("Couldn't send list request: %s\n");
        return;
    }
    // RECEIVING RESPONSE WITH HAS AN ADDRESS  
    char response;
    char address[100];
    if (receive_list_response(connection_socket, &response, &address) == -1) {
        log_errno_error("Couldn't receive response: %s\n");
        return;
    }
    log_response(response, &nm_address);
    close(connection_socket);

    // MAKE A NEW CONNECTION TO SS
    int ss_connection;

    // SENDING LIST REQUEST WITH THE PATH 
    if (send_list_request(connection_socket, path) == -1) {
        log_errno_error("Couldn't send list request to ss: %s\n");
        return;
    }
    
    // RECEIVING RESPONSE WITH HAS AN ADDRESS  
    char response;
    char list[1000];
    if (receive_list_response(connection_socket, &response, &list) == -1) {
        log_errno_error("Couldn't receive response to ss: %s\n");
        return;
    }
    // printing the data
    printf("%s\n", list);
    log_response(response,&address );
    close(ss_connection);
}