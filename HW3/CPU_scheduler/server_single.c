#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

// 原文的第一段 serve 邏輯
void serve(int server_fd) {
    while (true) {
        printf("等待新的連線...\n");
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        printf("接通新客戶 (FD: %d)！開始服務...\n", client_fd);
        char buffer[2048];

        while (true) {
            ssize_t len = read(client_fd, buffer, sizeof(buffer));
            if (len <= 0) {
                // 如果回傳 0 代表客戶端斷線，小於 0 代表發生錯誤
                printf("客戶 (FD: %d) 已斷線。\n", client_fd);
                break;
            }
            // 回音：把收到的東西寫回去
            write(client_fd, buffer, len);
        }
        close(client_fd);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 設定伺服器地址與 Port (這裡使用 8080)
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // 綁定與監聽
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10); // 允許最多 10 個排隊等待

    printf("[單執行緒模式] Echo Server 啟動，監聽 Port 8080...\n");
    serve(server_fd);

    return 0;
}