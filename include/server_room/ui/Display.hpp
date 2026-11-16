#pragma once
#include "server_room/ui/Canvas.hpp"
#include "server_room/graphics/Shader.hpp"
#include "server_room/graphics/Texture.hpp"
#include "server_room/app/SceneState.hpp"
#include <deque>
namespace server_room {
class Display {
public:
    explicit Display(const std::filesystem::path& shaders);
    ~Display();
    Display(const Display&)=delete;
    Display& operator=(const Display&)=delete;
    void sample(float frameSeconds);
    void refresh(const SceneState& state,bool captured,bool doorOpen,const std::string& caption="");
    void bindMonitor() const { monitor_.bind(1); }
    void draw() const;
    float fps() const {return fps_;}
    float frameMs() const {return frameMs_;}
private:
    Canvas hudCanvas_{640,360},monitorCanvas_{512,256};
    Texture hud_,monitor_;
    Shader shader_;
    GLuint vao_=0;
    std::deque<float> history_;
    double sampleTime_=0;
    int sampleFrames_=0;
    float fps_=0,frameMs_=0;
};
}
