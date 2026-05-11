#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h> // 多了這個標頭檔來處理執行緒

// 原文中的 serve_client 邏輯
void serve_client(int client_fd) {
    char buffer[2048];
    while (true) {
        ssize_t len = read(client_fd, buffer, sizeof(buffer));
        if (len <= 0) {
            printf("客戶 (FD: %d) 已斷線。\n", client_fd);
            break;
        }
        write(client_fd, buffer, len);
    }
    close(client_fd);
}

// POSIX Thread 需要的包裝函數，用來接起參數並呼叫 serve_client
void* thread_wrapper(void* arg) {
    int client_fd = *(int*)arg;
    free(arg); // 釋放記憶體避免 Memory Leak
    serve_client(client_fd);
    return NULL;
}

// 原文的第二段 serve 邏輯
void serve(int server_fd) {
    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        printf("接通新客戶 (FD: %d)！老闆指派新執行緒服務他...\n", client_fd);

        // 必須動態配置記憶體來傳遞 fd，避免多執行緒的競爭危害 (Race Condition)
        int* pclient = malloc(sizeof(int));
        *pclient = client_fd;

        // 建立 Thread (也就是原文中的 run_thread)
        pthread_t tid;
        pthread_create(&tid, NULL, thread_wrapper, pclient);
        
        // 分離 Thread，讓它結束後自動回收資源，主程式不用等它
        pthread_detach(tid); 
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081); // 這次我們用 8081 Port 免得打架

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    printf("[多執行緒模式] Echo Server 啟動，監聽 Port 8081...\n");
    serve(server_fd);

    return 0;
}