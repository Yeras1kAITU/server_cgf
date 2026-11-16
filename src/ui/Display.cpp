#include "server_room/ui/Display.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
namespace server_room {
namespace {
const Color cyan{95,212,245,255}, white{215,231,243,255}, muted{126,161,185,255},red{255,89,73,255};
std::string decimal(float value) {std::ostringstream s;s<<std::fixed<<std::setprecision(1)<<value;return s.str();}
}
Display::Display(const std::filesystem::path& path):
    hud_(640,360,hudCanvas_.pixels),monitor_(512,256,monitorCanvas_.pixels),
    shader_(path/"overlay.vert",path/"overlay.frag") {glGenVertexArrays(1,&vao_);}
Display::~Display(){glDeleteVertexArrays(1,&vao_);}
void Display::sample(float seconds) {
    if(seconds<=0) return;
    sampleTime_+=seconds;++sampleFrames_;
    if(sampleTime_>=.1) {
        fps_=float(sampleFrames_/sampleTime_);
        frameMs_=float(1000*sampleTime_/sampleFrames_);
        history_.push_back(fps_);
        if(history_.size()>240) history_.pop_front();
        sampleTime_=0;sampleFrames_=0;
    }
}
void Display::refresh(const SceneState& s,bool captured,bool doorOpen,const std::string& caption) {
    auto& h=hudCanvas_;
    h.clear({0,0,0,0});
    h.rect(10,10,248,59,{5,13,23,225});
    const Color accent=s.alarmEnabled?red:cyan;
    h.rect(10,10,3,59,accent);
    h.text(20,18,"SERVER ROOM",white,2);
    h.text(20,39,s.alarmEnabled?"OVERHEAT ALARM":"COOLING NORMAL",accent);
    h.text(20,54,"FPS "+decimal(fps_)+"  MS "+decimal(frameMs_)+"  AIR "+std::to_string(int(s.coolingLevel*100))+"%",muted);
    h.rect(316,176,8,1,white);h.rect(320,172,1,9,white);
    if(s.selectedRack>=0) {
        h.rect(182,209,276,20,{5,13,23,230});
        h.text(193,215,"E: "+std::string(doorOpen?"CLOSE":"OPEN")+" RACK "+std::to_string(s.selectedRack+1),accent);
    }
    if(s.showHelp) {
        h.rect(10,298,620,52,{5,13,23,230});
        h.text(20,306,"WASD WALK | MOUSE LOOK | E RACK DOOR | F ALARM",white);
        h.text(20,320,"[ / ] COOLING | R RESET VIEW | H HELP | Q QUIT",muted);
        h.text(20,334,captured?"ESC RELEASES MOUSE":"LEFT CLICK TO CAPTURE MOUSE",accent);
    } else h.text(12,342,"H: CONTROLS",white);
    if(!caption.empty()) {
        h.rect(10,258,620,29,{5,13,23,240});
        h.text(20,269,caption,white);
    }
    hud_.bind(0);hud_.update(h.pixels);
    auto& m=monitorCanvas_;
    m.clear({5,13,23,255});m.rect(0,0,512,4,accent);
    m.text(20,18,"SERVER ROOM / LIVE",white,2);
    m.text(20,47,s.alarmEnabled?"OVERHEAT ALARM":"COOLING NORMAL",accent,2);
    m.text(20,78,"FPS "+decimal(fps_)+"  MS "+decimal(frameMs_),white,2);
    float maximum=120;
    for(float value:history_) maximum=std::max(maximum,value*1.1f);
    maximum=std::ceil(maximum/60)*60;
    for(int row=0;row<=3;++row) {
        const int y=112+row*32;
        m.line(58,y,490,y,{25,47,62,255});
        m.text(5,y-3,std::to_string(int(maximum*(3-row)/3)),muted);
    }
    int lastX=58,lastY=208;
    for(size_t i=0;i<history_.size();++i) {
        const int x=58+int(i)*432/239;
        const int y=208-int(std::clamp(history_[i]/maximum,0.0f,1.0f)*96);
        if(i>0) m.line(lastX,lastY,x,y,accent);
        lastX=x;lastY=y;
    }
    m.text(20,231,"24 SECOND HISTORY / ACTUAL FRAME TIMING",muted);
    monitor_.bind(1);monitor_.update(m.pixels);
}
void Display::draw() const {
    glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    shader_.use();shader_.integer("uHUD",0);hud_.bind(0);
    glBindVertexArray(vao_);glDrawArrays(GL_TRIANGLES,0,3);
    glDisable(GL_BLEND);glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);
}
}
