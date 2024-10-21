#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

#define SERVER_QUEUE_NAME "/mq-server"
#define CLIENT_QUEUE_NAME "/mq-client"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 64
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

void reverseString(char* str) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; ++i)
        std::swap(str[i], str[n - i - 1]);
}

int main() {
    mqd_t server_queue, client_queue;
    struct mq_attr attr;
    char buffer[MSG_BUFFER_SIZE];

    // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create server message queue
    server_queue = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (server_queue == -1) {
        perror("Server: mq_open (server)");
        exit(1);
    }

    // Open client message queue
    client_queue = mq_open(CLIENT_QUEUE_NAME, O_WRONLY);
    if (client_queue == -1) {
        perror("Server: mq_open (client)");
        exit(1);
    }

    std::cout << "I AM SERVER! I WILL RECEIVE MESSAGE FROM CLIENT AND WILL SEND REVERSE OF THIS" << std::endl;

    while (true) {
        // Receive message from client
        ssize_t bytes_read = mq_receive(server_queue, buffer, MSG_BUFFER_SIZE, nullptr);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0'; // Null terminate the message
            std::cout << "MESSAGE RECEIVED FROM CLIENT: " << buffer << std::endl;

            // Reverse the string
            reverseString(buffer);

            // Send reversed message back to client
            if (mq_send(client_queue, buffer, strlen(buffer), 0) == -1) {
                perror("Server: Not able to send message to client");
                continue;
            }
            std::cout << "SENT REVERSE OF MESSAGE TO CLIENT" << std::endl;
        } else {
            perror("Server: mq_receive");
        }
    }

    // Cleanup
    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    mq_close(client_queue);
    return 0;
}
