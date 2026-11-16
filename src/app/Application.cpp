
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "server_room/app/Application.hpp"
#include "server_room/app/SceneState.hpp"
#include "server_room/graphics/CubeMesh.hpp"
#include "server_room/input/Camera.hpp"
#include "server_room/scene/Room.hpp"
#include "server_room/ui/Display.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

// Contribution of all team members

namespace server_room {
namespace {
struct Input {
    Camera camera;
    SceneState state;
    Room* room=nullptr;
    bool captured=false,firstMouse=true;
    double lastX=0,lastY=0;
};
Input& input(GLFWwindow* window){return *static_cast<Input*>(glfwGetWindowUserPointer(window));}
void capture(GLFWwindow* window,bool enabled) {
    auto& i=input(window);i.captured=enabled;i.firstMouse=true;
    glfwSetInputMode(window,GLFW_CURSOR,enabled?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
    if(glfwRawMouseMotionSupported()) glfwSetInputMode(window,GLFW_RAW_MOUSE_MOTION,enabled?GLFW_TRUE:GLFW_FALSE);
}
void cursor(GLFWwindow* window,double x,double y) {
    auto& i=input(window);
    if(!i.captured) return;
    if(!i.firstMouse) i.camera.look(float(x-i.lastX),float(y-i.lastY));
    i.lastX=x;i.lastY=y;i.firstMouse=false;
}
void key(GLFWwindow* window,int code,int,int action,int) {
    auto& i=input(window);
    if((code==GLFW_KEY_LEFT_BRACKET || code==GLFW_KEY_RIGHT_BRACKET) && action!=GLFW_RELEASE) {
        i.state.coolingLevel=std::clamp(i.state.coolingLevel+(code==GLFW_KEY_RIGHT_BRACKET?.05f:-.05f),0.0f,1.0f);
        return;
    }
    if(action!=GLFW_PRESS) return;
    if(code==GLFW_KEY_ESCAPE) capture(window,false);
    if(code==GLFW_KEY_Q) glfwSetWindowShouldClose(window,GLFW_TRUE);
    if(code==GLFW_KEY_R) {i.camera=Camera{};i.firstMouse=true;}
    if(code==GLFW_KEY_F) i.state.alarmEnabled=!i.state.alarmEnabled;
    if(code==GLFW_KEY_H) i.state.showHelp=!i.state.showHelp;
    if(code==GLFW_KEY_E) {
        // Pick again at the key event, rather than using the previous frame's target.
        const int rack=i.room->pickRack(i.camera.position,i.camera.direction());
        i.room->toggleDoor(rack);
    }
}
void mouse(GLFWwindow* w,int button,int action,int) {
    if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS) capture(w,true);
}
void focus(GLFWwindow* w,int focused){if(!focused) capture(w,false);}
void movement(GLFWwindow* w,Input& i,float dt) {
    if(!i.captured || !glfwGetWindowAttrib(w,GLFW_FOCUSED)) return;
    const float forward=float(glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS)-float(glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS);
    const float right=float(glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS)-float(glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS);
    const Vec3 before=i.camera.position;
    i.camera.move(forward,right,dt,i.room->config().width,i.room->config().depth);
    const Vec3 desired=i.camera.position;
    // Resolve X and Z separately to slide along cabinet sides and door grilles.
    i.camera.position=before;
    if(!i.room->blocked({desired.x,before.y,before.z})) i.camera.position.x=desired.x;
    if(!i.room->blocked({i.camera.position.x,before.y,desired.z})) i.camera.position.z=desired.z;
}
void screenshot(const std::filesystem::path& path,int w,int h) {
    std::vector<unsigned char> pixels(size_t(w)*h*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::ofstream file(path,std::ios::binary);
    if(!file) throw std::runtime_error("Cannot write screenshot: "+path.string());
    file<<"P6\n"<<w<<' '<<h<<"\n255\n";
    for(int y=h-1;y>=0;--y) file.write(reinterpret_cast<const char*>(pixels.data()+size_t(y)*w*3),w*3);
    if(!file) throw std::runtime_error("Screenshot write failed");
}
void scenario(Input& i,const Options& options) {
    if(options.scenario=="alarm") i.state.alarmEnabled=true;
    if(options.scenario=="fog") i.state.coolingLevel=1;
    if(options.scenario=="monitor") {i.camera.position={0,1.75f,-4.7f};i.camera.pitch=2;}
    if(options.scenario=="doors") {
        i.camera.position={0,1.7f,4.5f};i.camera.yaw=180;i.camera.pitch=-8;
        const int rack=i.room->pickRack(i.camera.position,i.camera.direction());
        if(rack<0) throw std::runtime_error("Door scenario cannot select rack");
        i.room->toggleDoor(rack);
    }
}
void renderLoop(GLFWwindow* window,const Options& options,const std::filesystem::path& folder) {
    Room room;
    Input i;i.room=&room;scenario(i,options);
    glfwSetWindowUserPointer(window,&i);
    glfwSetCursorPosCallback(window,cursor);glfwSetKeyCallback(window,key);
    glfwSetMouseButtonCallback(window,mouse);glfwSetWindowFocusCallback(window,focus);
    if(!options.smoke) capture(window,true);
    Shader shader(folder/"room.vert",folder/"room.frag");
    CubeMesh cube;
    std::vector<Instance> opaque,leds;
    for(const auto& item:room.staticInstances())
        (int(item.colorKind[3])==Led?leds:opaque).push_back(item);
    InstanceBatch staticBatch(opaque),ledBatch(leds),doors(room.doorInstances(-1),true);
    Texture floor(128,128,Texture::floorPixels(),true);
    Display display(folder);
    glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(options.smoke?0:1);
    std::cout<<"OpenGL: "<<glGetString(GL_VERSION)<<"\nRenderer: "<<glGetString(GL_RENDERER)
             <<"\nResolution: "<<options.width<<'x'<<options.height
             <<"\nStatic instances: "<<opaque.size()<<" | LED quads: "<<leds.size()<<"\n";
    double previous=glfwGetTime(),lastUI=-1,titleTime=previous;
    float animationTime=0;
    int frames=0;
    std::vector<double> durations;
    std::vector<float> oldAngles(room.racks().size(),-1);
    int oldSelected=-2;
    while(!glfwWindowShouldClose(window)) {
        const double frameStart=glfwGetTime();
        glfwPollEvents();
        const double now=glfwGetTime();
        const double elapsed=now-previous;previous=now;
        const float dt=options.smoke?1.0f/60:float(std::clamp(elapsed,0.0,.05));
        animationTime+=dt;
        int width,height;glfwGetFramebufferSize(window,&width,&height);
        if(width<=0 || height<=0){glfwWaitEventsTimeout(.1);continue;}
        movement(window,i,dt);
        room.update(dt);
        i.state.selectedRack=room.pickRack(i.camera.position,i.camera.direction());
        bool dirty=i.state.selectedRack!=oldSelected;
        for(size_t r=0;r<oldAngles.size();++r) {
            dirty=dirty || room.racks()[r].angle!=oldAngles[r];oldAngles[r]=room.racks()[r].angle;
        }
        if(dirty) doors.update(room.doorInstances(i.state.selectedRack));
        oldSelected=i.state.selectedRack;
        display.sample(float(elapsed));
        if(now-lastUI>=.1 || frames==0) {
            const bool open=i.state.selectedRack>=0 && room.racks()[size_t(i.state.selectedRack)].open;
            display.refresh(i.state,i.captured,open);lastUI=now;
        }
        glViewport(0,0,width,height);glClearColor(.025f,.045f,.075f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.matrix("uView",lookAt(i.camera.position,i.camera.direction()));
        shader.matrix("uProjection",perspective(float(width)/height));
        shader.vector("uCameraPosition",i.camera.position);
        shader.scalar("uTime",animationTime);shader.scalar("uAlarm",i.state.alarmEnabled?1.0f:0.0f);
        shader.scalar("uCooling",i.state.coolingLevel);
        shader.integer("uFloor",0);shader.integer("uMonitor",1);
        floor.bind(0);display.bindMonitor();
        staticBatch.draw(cube);ledBatch.draw(cube,true);doors.draw(cube);
        display.draw();
        if(now-titleTime>=.5) {
            const std::string title="Server Room | "+std::to_string(int(display.fps()))+" FPS | "+(i.state.alarmEnabled?"OVERHEAT ALARM":"COOLING NORMAL");
            glfwSetWindowTitle(window,title.c_str());titleTime=now;
        }
        ++frames;
        if(options.smoke && frames==options.frames && !options.screenshot.empty()) screenshot(options.screenshot,width,height);
        const GLenum error=glGetError();
        if(error!=GL_NO_ERROR) throw std::runtime_error("OpenGL error "+std::to_string(error));
        glfwSwapBuffers(window);
        if(options.benchmark) glFinish();
        if(options.benchmark && frames>30) durations.push_back(glfwGetTime()-frameStart);
        if(options.smoke && frames>=options.frames) glfwSetWindowShouldClose(window,GLFW_TRUE);
    }
    if(options.benchmark && !durations.empty()) {
        const double sum=std::accumulate(durations.begin(),durations.end(),0.0);
        std::sort(durations.begin(),durations.end());
        std::cout<<std::fixed<<std::setprecision(2)<<"BENCHMARK scenario="<<options.scenario
                 <<" frames="<<durations.size()<<" average_fps="<<durations.size()/sum
                 <<" p95_ms="<<durations[size_t((durations.size()-1)*.95)]*1000
                 <<" worst_ms="<<durations.back()*1000<<'\n';
    }
    if(options.smoke) std::cout<<"Smoke test passed: "<<frames<<" frames, scenario="<<options.scenario<<", no OpenGL errors.\n";
}
}
void runApplication(const Options& options,const std::filesystem::path& executable) {
    glfwSetErrorCallback([](int code,const char* message){std::cerr<<"GLFW "<<code<<": "<<message<<'\n';});
    if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES,4);
    if(options.smoke) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
    GLFWwindow* window=glfwCreateWindow(options.width,options.height,"Server Room",nullptr,nullptr);
    if(!window){glfwTerminate();throw std::runtime_error("OpenGL 3.3 window creation failed");}
    glfwMakeContextCurrent(window);
    try {
        if(!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) throw std::runtime_error("OpenGL loader failed");
        renderLoop(window,options,executable.parent_path()/"shaders");
    } catch(...) {glfwDestroyWindow(window);glfwTerminate();throw;}
    glfwDestroyWindow(window);glfwTerminate();
}
}
