#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#define MAX_CLIENTS 64
#define BUF_SIZE 2048
#define PORT 8080

struct client {
    int fd;
    char buffer[BUF_SIZE];
    ssize_t offset; // -1 表示讀取狀態，其餘值表示寫入狀態的位移
    ssize_t length; // 待寫入的剩餘長度
};

// 將 Socket 設定為非阻塞模式
void set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int server_fd;
    struct sockaddr_in addr;
    struct client clients[MAX_CLIENTS];
    int num_clients = 0;

    // 1. 建立 Socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_nonblock(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // 2. 綁定並監聽
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 10);

    printf("Server listening on port %d (Non-blocking mode)\n", PORT);

    while (true) {
        // 3. 嘗試接受新連線 (Non-blocking accept)
        if (num_clients < MAX_CLIENTS) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd != -1) {
                set_nonblock(client_fd);
                clients[num_clients].fd = client_fd;
                clients[num_clients].offset = -1; // 初始為讀取狀態
                num_clients++;
                printf("New client connected. Total: %d\n", num_clients);
            }
        }

        // 4. 輪詢所有客戶端進行讀寫
        for (int i = 0; i < num_clients; i++) {
            struct client *c = &clients[i];
            bool is_reading = (c->offset == -1);

            if (is_reading) {
                ssize_t n = read(c->fd, c->buffer, sizeof(c->buffer));
                if (n > 0) {
                    c->offset = 0;   // 切換到寫入狀態
                    c->length = n;
                } else if (n == 0 || (n == -1 && errno != EWOULDBLOCK)) {
                    goto close_client;
                }
            } else {
                // 寫入狀態：把 buffer 內容寫回
                ssize_t n = write(c->fd, c->buffer + c->offset, c->length);
                if (n > 0) {
                    c->length -= n;
                    c->offset += n;
                    if (c->length == 0) c->offset = -1; // 寫完，切回讀取
                } else if (n == -1 && errno != EWOULDBLOCK) {
                    goto close_client;
                }
            }
            continue;

        close_client:
            printf("Client disconnected.\n");
            close(c->fd);
            clients[i] = clients[num_clients - 1]; // 移山填海法移除 client
            num_clients--;
            i--;
        }
    }
    return 0;
}