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

int main() {
    mqd_t server_queue, client_queue;
    struct mq_attr attr;
    char buffer[MSG_BUFFER_SIZE];

    // Set message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create client message queue
    client_queue = mq_open(CLIENT_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr);
    if (client_queue == -1) {
        perror("Client: mq_open (client)");
        exit(1);
    }

    // Open server message queue
    server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_queue == -1) {
        perror("Client: mq_open (server)");
        exit(1);
    }

    std::cout << "I AM CLIENT. ENTER MESSAGE WHICH I WILL PASS TO SERVER (Press <ENTER>):" << std::endl;

    while (true) {
        std::cin.getline(buffer, MAX_MSG_SIZE);

        // Send message to server
        if (mq_send(server_queue, buffer, strlen(buffer), 0) == -1) {
            perror("Client: Not able to send message to server");
            continue;
        }

        // Receive reversed message from server
        ssize_t bytes_read = mq_receive(client_queue, buffer, MSG_BUFFER_SIZE, nullptr);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0'; // Null terminate the message
            std::cout << "THE REVERSED STRING RECEIVED FROM SERVER: " << buffer << std::endl;
            std::cout << "ENTER MESSAGE WHICH I WILL PASS TO SERVER (Press <ENTER>):" << std::endl;
        } else {
            perror("Client: mq_receive");
        }
    }

    // Cleanup
    mq_close(client_queue);
    mq_unlink(CLIENT_QUEUE_NAME);
    mq_close(server_queue);
    return 0;
}
