#include "../config.h"

using json = nlohmann::json;

void recv_data(std::queue<json> &queue_json);

std::string decode_utf8(const char *data, size_t length);