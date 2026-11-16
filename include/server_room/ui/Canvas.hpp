#pragma once
#include <array>
#include <string>
#include <vector>
namespace server_room {
using Color=std::array<unsigned char,4>;
class Canvas {
public:
    Canvas(int w,int h):width(w),height(h),pixels(size_t(w)*h*4) {}
    void clear(Color color);
    void rect(int x,int y,int w,int h,Color color);
    void line(int x0,int y0,int x1,int y1,Color color);
    void text(int x,int y,const std::string& value,Color color,int scale=1);
    int width,height;
    std::vector<unsigned char> pixels;
};
}
