#pragma once
#include <glad/gl.h>
#include <vector>
namespace server_room {
class Texture {
public:
    Texture(int width,int height,const std::vector<unsigned char>& rgba,bool mipmaps=false);
    ~Texture();
    Texture(const Texture&)=delete;
    Texture& operator=(const Texture&)=delete;
    void bind(GLuint unit) const;
    void update(const std::vector<unsigned char>& rgba);
    static std::vector<unsigned char> floorPixels();
private:
    GLuint id_=0;
    int width_,height_;
};
}
