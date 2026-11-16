#include "server_room/ui/Canvas.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
namespace server_room {
namespace {
// Original compact 5x7 bitmap alphabet; no external font dependency.
std::array<unsigned char,7> glyph(char c) {
    switch(c) {
    case 'A':return {14,17,17,31,17,17,17}; case 'B':return {30,17,17,30,17,17,30};
    case 'C':return {14,17,16,16,16,17,14}; case 'D':return {30,17,17,17,17,17,30};
    case 'E':return {31,16,16,30,16,16,31}; case 'F':return {31,16,16,30,16,16,16};
    case 'G':return {14,17,16,23,17,17,15}; case 'H':return {17,17,17,31,17,17,17};
    case 'I':return {14,4,4,4,4,4,14}; case 'J':return {7,2,2,2,18,18,12};
    case 'K':return {17,18,20,24,20,18,17}; case 'L':return {16,16,16,16,16,16,31};
    case 'M':return {17,27,21,21,17,17,17}; case 'N':return {17,25,21,19,17,17,17};
    case 'O':return {14,17,17,17,17,17,14}; case 'P':return {30,17,17,30,16,16,16};
    case 'Q':return {14,17,17,17,21,18,13}; case 'R':return {30,17,17,30,20,18,17};
    case 'S':return {15,16,16,14,1,1,30}; case 'T':return {31,4,4,4,4,4,4};
    case 'U':return {17,17,17,17,17,17,14}; case 'V':return {17,17,17,17,17,10,4};
    case 'W':return {17,17,17,21,21,21,10}; case 'X':return {17,17,10,4,10,17,17};
    case 'Y':return {17,17,10,4,4,4,4}; case 'Z':return {31,1,2,4,8,16,31};
    case '0':return {14,17,19,21,25,17,14}; case '1':return {4,12,4,4,4,4,14};
    case '2':return {14,17,1,2,4,8,31}; case '3':return {30,1,1,14,1,1,30};
    case '4':return {2,6,10,18,31,2,2}; case '5':return {31,16,16,30,1,1,30};
    case '6':return {14,16,16,30,17,17,14}; case '7':return {31,1,2,4,8,8,8};
    case '8':return {14,17,17,14,17,17,14}; case '9':return {14,17,17,15,1,1,14};
    case ':':return {0,4,4,0,4,4,0}; case '.':return {0,0,0,0,0,4,4};
    case '-':return {0,0,0,31,0,0,0}; case '+':return {0,4,4,31,4,4,0};
    case '/':return {1,2,2,4,8,8,16}; case '[':return {14,8,8,8,8,8,14};
    case ']':return {14,2,2,2,2,2,14}; case '%':return {17,2,4,4,8,16,17};
    case '|':return {4,4,4,4,4,4,4}; case '?':return {14,17,1,2,4,0,4};
    default:return {};
    }
}
}
void Canvas::clear(Color color){rect(0,0,width,height,color);}
void Canvas::rect(int x,int y,int w,int h,Color c) {
    for(int j=std::max(y,0);j<std::min(y+h,height);++j)
        for(int i=std::max(x,0);i<std::min(x+w,width);++i) {
            const size_t p=size_t(j*width+i)*4;
            std::copy(c.begin(),c.end(),pixels.begin()+p);
        }
}
void Canvas::line(int x0,int y0,int x1,int y1,Color c) {
    const int steps=std::max(std::abs(x1-x0),std::abs(y1-y0));
    for(int s=0;s<=steps;++s) {
        const float t=steps?float(s)/steps:0;
        rect(int(std::round(x0+(x1-x0)*t)),int(std::round(y0+(y1-y0)*t)),1,1,c);
    }
}
void Canvas::text(int x,int y,const std::string& value,Color c,int scale) {
    for(unsigned char ch:value) {
        const auto bits=glyph(char(std::toupper(ch)));
        for(int row=0;row<7;++row) for(int col=0;col<5;++col)
            if(bits[size_t(row)]&(1<<(4-col))) rect(x+col*scale,y+row*scale,scale,scale,c);
        x+=6*scale;
    }
}
}
