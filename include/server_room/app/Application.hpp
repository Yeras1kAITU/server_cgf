#pragma once
#include <filesystem>
#include <string>
namespace server_room {
struct Options {
    bool smoke=false,benchmark=false;
    int width=1920,height=1080,frames=180;
    std::string scenario="normal";
    std::filesystem::path screenshot;
};
Options parseOptions(int argc,char** argv);
void runApplication(const Options& options,const std::filesystem::path& executable);
}
