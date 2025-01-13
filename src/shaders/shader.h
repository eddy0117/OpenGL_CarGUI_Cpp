#include "../config.h"

class Shader{
    public:
        Shader();
        ~Shader();
        unsigned int create_shader(std::string vert_shader_path, std::string frag_shader_path);

    private:
};
    