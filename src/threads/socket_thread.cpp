#include "socket_thread.h"
#include <unistd.h>


#define port 65432
#define ip_addr "127.0.0.1"

using json = nlohmann::json;

void recv_data(std::queue<std::vector<std::unordered_map<std::string, std::string>>> &frame_queue) {
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
	serverAddr.sin_addr.s_addr = inet_aton("localhost", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(sockfd);
        exit(0);
    }
    std::cout << "server start at: " << inet_ntoa(serverAddr.sin_addr) << ":" << port << std::endl;

    listen(sockfd, 1);

    std::cout << "wait for connection..." << std::endl;

    // std::vector<std::string> sp_test = split("11sadfgsafasdf1~123asdfasgasgasdgasgda3", "~");
    // std::cout << sp_test[0] << sp_test[1].substr(0, 5) << std::endl;

    addrlen = sizeof(clientAddr);
    while(1){
        char indata[5002] = {0};
        std::string whole_data, data_cat;
        // 阻塞, 直到有新連接
        newfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addrlen);
        std::cout << "connected by: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
        while (1) {
            int nbyte = recv(newfd, indata, 5000, 0);
            if (nbyte <= 0) {
                close(newfd);
                std::cout << "connection closed" << std::endl;
                break;
            }
            // std::cout << "indata: " << indata << std::endl;

            std::vector<std::string> data_split = split(indata, "~");

            if (data_split.size() > 1) {

                whole_data += data_split[0];
                data_cat = data_split[1];
                // sent message
                
                try {
                    auto j = json::parse(whole_data);
                	std::vector<std::unordered_map<std::string, std::string>> queue_frame_objs;
                 
                	for(auto& obj: j["obj"]){
                		queue_frame_objs.push_back({
                			{"x", std::to_string(obj["x"].get<float>())},
                			{"y", std::to_string(obj["y"].get<float>())},
                			{"cls", obj["cls"].get<std::string>()},
                			{"ang", std::to_string(obj["ang"].get<float>())},
                		});
                	}
                    frame_queue.push(queue_frame_objs);
                }
                catch (const std::exception& e){
                    std::cout << e.what() << std::endl;
                }
                
            
                whole_data = "";
                whole_data += data_cat;

                // 需手動將 buffer 清空
                memset(indata, 0, sizeof(indata));
            }
            else {
                data_cat = "";
                whole_data += data_split[0];

                // 搞很久
                // 需手動將 buffer 清空
                memset(indata, 0, sizeof(indata));
            }
            
            // std::cout << "================================" << std::endl;
        }
    }
	// for(auto& frame: j){
		
	// 	std::vector<std::unordered_map<std::string, std::string>> queue_frame_objs;
	// 	for(auto& obj: frame){

	// 		queue_frame_objs.push_back({
	// 			{"x", std::to_string(obj["x"].get<float>() / 682)},
	// 			{"y", std::to_string(obj["y"].get<float>() / 682)},
	// 			{"cls", obj["class"].get<std::string>()},
	// 			{"ang", std::to_string(obj["distance_ang"].get<float>() + 90)},
	// 		});
	// 	}
	// 	frame_queue.push(queue_frame_objs);
	// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// }
}