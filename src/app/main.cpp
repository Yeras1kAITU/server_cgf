#include "server_room/app/Application.hpp"
#include <iostream>
int main(int argc,char** argv) {
    try {
        const auto options=server_room::parseOptions(argc,argv);
        server_room::runApplication(options,std::filesystem::absolute(argv[0]));
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"Server Room: "<<error.what()<<'\n';return 1;
    }
}
