#include "server_room/app/Application.hpp"
#include <stdexcept>
namespace server_room {
Options parseOptions(int argc,char** argv) {
    Options result;
    for(int i=1;i<argc;++i) {
        const std::string argument=argv[i];
        auto value=[&](){if(i+1>=argc) throw std::runtime_error("Missing value for "+argument);return std::string(argv[++i]);};
        if(argument=="--smoke-test") result.smoke=true;
        else if(argument=="--benchmark") {result.benchmark=true;result.smoke=true;result.frames=360;}
        else if(argument=="--screenshot") result.screenshot=value();
        else if(argument=="--scenario") result.scenario=value();
        else if(argument=="--1080p") {result.width=1920;result.height=1080;}
        else if(argument=="--720p") {result.width=1280;result.height=720;}
        else throw std::runtime_error("Usage: server_room [--smoke-test] [--benchmark] [--scenario normal|alarm|doors|fog|monitor] [--screenshot image.ppm] [--1080p|--720p]");
    }
    if(result.scenario!="normal" && result.scenario!="alarm" && result.scenario!="doors" && result.scenario!="fog" && result.scenario!="monitor")
        throw std::runtime_error("Unknown scenario: "+result.scenario);
    if(!result.screenshot.empty() && !result.smoke) throw std::runtime_error("--screenshot requires --smoke-test");
    return result;
}
}
