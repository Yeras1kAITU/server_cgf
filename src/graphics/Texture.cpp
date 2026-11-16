#include "server_room/graphics/Texture.hpp"
#include <stdexcept>
namespace server_room {
Texture::Texture(int w,int h,const std::vector<unsigned char>& rgba,bool mipmaps):width_(w),height_(h) {
    if(rgba.size()!=size_t(w)*h*4) throw std::runtime_error("Invalid texture dimensions");
    glGenTextures(1,&id_);glBindTexture(GL_TEXTURE_2D,id_);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,rgba.data());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,mipmaps?GL_REPEAT:GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,mipmaps?GL_REPEAT:GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,mipmaps?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR);
    if(mipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}
Texture::~Texture(){glDeleteTextures(1,&id_);}
void Texture::bind(GLuint unit) const {glActiveTexture(GL_TEXTURE0+unit);glBindTexture(GL_TEXTURE_2D,id_);}
void Texture::update(const std::vector<unsigned char>& rgba) {
    if(rgba.size()!=size_t(width_)*height_*4) throw std::runtime_error("Texture update size mismatch");
    glBindTexture(GL_TEXTURE_2D,id_);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width_,height_,GL_RGBA,GL_UNSIGNED_BYTE,rgba.data());
}
std::vector<unsigned char> Texture::floorPixels() {
    std::vector<unsigned char> pixels(128*128*4);
    for(int y=0;y<128;++y) for(int x=0;x<128;++x) {
        const bool seam=x<2 || y<2 || x>125 || y>125;
        const bool screw=(x-8)*(x-8)+(y-8)*(y-8)<5 || (x-120)*(x-120)+(y-120)*(y-120)<5;
        const int value=seam?65:(screw?90:190+((x*13+y*7)%9));
        const size_t p=size_t(y*128+x)*4;
        pixels[p]=pixels[p+1]=pixels[p+2]=static_cast<unsigned char>(value);pixels[p+3]=255;
    }
    return pixels;
}
}
