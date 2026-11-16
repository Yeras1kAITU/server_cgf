#include "server_room/scene/Room.hpp"
#include "server_room/input/Camera.hpp"
#include "server_room/app/Application.hpp"
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
using namespace server_room;
namespace {
void expect(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
bool near(float a,float b){return std::abs(a-b)<.0001f;}
}
int main() {
    try {
        Camera a,b;
        a.move(1,0,1,12,16);
        for(int f=0;f<120;++f)b.move(1,0,1.0f/120,12,16);
        expect(near(a.position.z,b.position.z),"Frame-rate-independent movement");
        a=Camera{};a.move(1,1,1,12,16);
        const Vec3 offset=a.position-Camera{}.position;
        expect(near(dot(offset,offset),9),"Normalized diagonal speed");
        a.look(0,100000);expect(near(a.pitch,-85),"Pitch clamp");
        Room room;
        expect(room.racks().size()==8,"Eight racks");
        expect(!room.blocked({0,1.7f,0}),"Center aisle clear");
        expect(room.blocked(room.racks()[0].center),"Rack collision");
        expect(room.blocked({6,1.7f,0}),"Wall collision");
        const int target=room.pickRack({0,1.7f,4.5f},{-1,0,0});
        expect(target>=0,"Front rack selection");
        expect(room.pickRack({0,1.7f,4.5f},{0,1,0})<0,"Ray miss");
        expect(room.pickRack({0,1.7f,4.5f},{-1,0,0},.1f)<0,"Interaction reach");
        const auto& rack=room.racks()[size_t(target)];
        const Vec3 closed=transformPoint(room.world(rack.hinge),{1,1,0});
        const Vec3 pivot=transformPoint(room.world(rack.hinge),{0,0,0});
        room.toggleDoor(target);room.update(2);
        const Vec3 opened=transformPoint(room.world(rack.hinge),{1,1,0});
        const Vec3 openPivot=transformPoint(room.world(rack.hinge),{0,0,0});
        expect(dot(opened-closed,opened-closed)>.5f,"Door rotates through parent-child transform");
        expect(near(dot(openPivot-pivot,openPivot-pivot),0),"Hinge pivot remains fixed");
        expect(near(room.racks()[size_t(target)].angle,radians(-105)),"Door reaches open angle");
        room.toggleDoor(target);room.update(2);
        expect(near(room.racks()[size_t(target)].angle,0),"Door closes");
        room.toggleDoor(-1);room.toggleDoor(999);
        const auto doors=room.doorInstances(target);
        expect(doors.size()==128,"Door mesh assembly");
        std::set<float> seeds;
        int floors=0,monitors=0,lights=0;
        for(const auto& instance:room.staticInstances()) {
            const int kind=int(instance.colorKind[3]);
            if(kind==Led)seeds.insert(instance.parameters[0]);
            if(kind==Floor) ++floors;
            if(kind==Monitor) ++monitors;
            if(kind==CeilingLight) ++lights;
        }
        expect(seeds.size()==192,"Unique independent LED seeds");
        expect(floors==1 && monitors==1 && lights==3,"Required scene materials");
        const char* argv[]={"server_room","--benchmark","--scenario","alarm"};
        const Options options=parseOptions(4,const_cast<char**>(argv));
        expect(options.benchmark && options.smoke && options.scenario=="alarm","Benchmark CLI");
        expect(options.width==1920 && options.height==1080,"Full HD default");
        const char* lowArgs[]={"server_room","--720p"};
        const auto low=parseOptions(2,const_cast<char**>(lowArgs));
        expect(low.width==1280 && low.height==720,"Course resolution override");
        std::cout<<"PASS: camera timing/speed, picking/reach, collision, door hierarchy/pivot, LED seeds, scene materials, CLI.\n";
        return 0;
    } catch(const std::exception& error){std::cerr<<"FAIL: "<<error.what()<<'\n';return 1;}
}
