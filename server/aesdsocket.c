/**
 * @file aesdsocket.c
 * @brief Simplified AESD Socket Server Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 9000
#define BUFFER_SIZE 1024
#define DATA_FILE "/var/tmp/aesdsocketdata"

static int server_fd = -1;
static int running = 1;

void signal_handler(int sig) {
    syslog(LOG_INFO, "Caught signal, exiting");
    running = 0;
    if (server_fd != -1) {
        shutdown(server_fd, SHUT_RDWR);
    }
}

void cleanup(void) {
    if (server_fd != -1) {
        close(server_fd);
    }
    unlink(DATA_FILE);
    closelog();
}

int main(int argc, char *argv[]) {
    int daemon_mode = 0;
    
    // Parse -d flag
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        daemon_mode = 1;
    }
    
    // Open syslog
    openlog("aesdsocket", LOG_PID, LOG_USER);
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return -1;
    }
    
    // Allow socket reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        cleanup();
        return -1;
    }
    
    // Bind to port 9000
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        cleanup();
        return -1;
    }
    
    // Listen for connections
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        cleanup();
        return -1;
    }
    
    // Daemonize if requested (after binding)
    if (daemon_mode) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            cleanup();
            return -1;
        }
        if (pid > 0) {
            exit(0);  // Parent exits
        }
        
        // Child continues as daemon
        setsid();
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    
    // Main server loop
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (running) {
                perror("accept");
            }
            continue;
        }
        
        // Log client connection
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);
        
        // Handle client data
        char *packet = NULL;
        size_t packet_size = 0;
        size_t packet_capacity = 0;
        char buffer[BUFFER_SIZE];
        ssize_t bytes;
        
        while ((bytes = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            // Process each byte
            for (int i = 0; i < bytes; i++) {
                // Grow packet buffer if needed
                if (packet_size == packet_capacity) {
                    packet_capacity = packet_capacity ? packet_capacity * 2 : BUFFER_SIZE;
                    char *new_packet = realloc(packet, packet_capacity);
                    if (!new_packet) {
                        syslog(LOG_ERR, "malloc failed");
                        free(packet);
                        packet = NULL;
                        packet_size = 0;
                        packet_capacity = 0;
                        break;
                    }
                    packet = new_packet;
                }
                
                packet[packet_size++] = buffer[i];
                
                // Check for newline (end of packet)
                if (buffer[i] == '\n') {
                    // Append to file
                    int fd = open(DATA_FILE, O_CREAT | O_WRONLY | O_APPEND, 0644);
                    if (fd != -1) {
                        write(fd, packet, packet_size);
                        close(fd);
                        
                        // Send entire file back to client
                        fd = open(DATA_FILE, O_RDONLY);
                        if (fd != -1) {
                            char send_buf[BUFFER_SIZE];
                            ssize_t read_bytes;
                            while ((read_bytes = read(fd, send_buf, sizeof(send_buf))) > 0) {
                                send(client_fd, send_buf, read_bytes, 0);
                            }
                            close(fd);
                        }
                    }
                    
                    // Reset for next packet
                    packet_size = 0;
                }
            }
        }
        
        free(packet);
        close(client_fd);
        syslog(LOG_INFO, "Closed connection from %s", client_ip);
    }
    
    cleanup();
    return 0;
}