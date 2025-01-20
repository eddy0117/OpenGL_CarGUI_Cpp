#include "socket_thread.h"
#include <unistd.h>


#define port 65432
#define ip_addr "127.0.0.1"
#define MAX_CHUNK_SIZE 5000


void App::recv_data() {
	// frame_queue 要以 ref 傳入

	// json j;
	// std::ifstream jfile("../json/result_vec_ordered.json");
	// jfile >> j;
	// jfile.close();
    
    int sockfd, newfd;
    socklen_t addrlen;
	struct sockaddr_in serverAddr, clientAddr;

    
    // 設定 server 端連接資訊
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

	serverAddr.sin_family = AF_INET;
	// serverAddr.sin_addr.s_addr = inet_aton("localhost", &serverAddr.sin_addr);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(sockfd);
        exit(0);
    }
    std::cout << "server start at: " << inet_ntoa(serverAddr.sin_addr) << ":" << port << std::endl;

    listen(sockfd, 1); //  開始監聽來自客戶端的連線，最多允許 1 個排隊中的連線

    std::cout << "wait for connection..." << std::endl;

    addrlen = sizeof(clientAddr);
    while(1){

        char indata[MAX_CHUNK_SIZE] = {0};
        std::string whole_data, data_cat;
        // 阻塞, 直到有新連接
        newfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addrlen);
        std::cout << "connected by: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
        while (1) {
            int nbyte = recv(newfd, indata, sizeof(indata), 0); // 接收資料
            if (nbyte <= 0) {
                close(newfd);
                std::cout << "connection closed" << std::endl;
                break;
            }

            std::vector<std::string> data_split = split(indata, "~");

            if (data_split.size() > 1) {

                whole_data += data_split[0];
                data_cat = data_split[1];
                // sent message
                
                try {
                    auto j = json::parse(whole_data);

                    // 互斥存取區域
                    {
                        std::lock_guard<std::mutex> lock(g_mtx);
                        queue_json.push(j);
                        // std::cout << "[Producer] Produced JSON data." << std::endl;
                    }
                    g_cv.notify_one();  // 通知消費者

                }
                catch (const std::exception& e){
                    std::cout << e.what() << std::endl;
                }
                
                whole_data = "";
                whole_data += data_cat;
            }
            else {
                data_cat = "";
                whole_data += data_split[0];     
            }
            // 搞很久
            // 需手動將 buffer 清空
            memset(indata, 0, sizeof(indata));
        }
    }
}