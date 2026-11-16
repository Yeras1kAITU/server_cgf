#include "server_room/scene/Room.hpp"
#include <algorithm>
#include <cmath>

namespace server_room {
Mat4 Room::world(int node) const {
    const auto& n=nodes_.at(size_t(node));
    return n.parent<0 ? n.local : world(n.parent)*n.local;
}
Room::Room(RoomConfig c):config_(c) {
    const Mat4 identity=modelMatrix({0,0,0},{1,1,1});
    const Vec3 wall{.16f,.22f,.30f}, frame{.035f,.055f,.08f};
    auto box=[&](Vec3 p,Vec3 s,Vec3 color,Material m=Metal,float u=1,float v=1) {
        static_.push_back(instance(identity,p,s,color,m,0,u,v));
    };
    box({0,-.1f,0},{c.width,.2f,c.depth},{.43f,.48f,.53f},Floor,c.width,c.depth);
    box({0,c.height+.1f,0},{c.width,.2f,c.depth},wall);
    box({-c.width/2-.1f,c.height/2,0},{.2f,c.height,c.depth},wall);
    box({c.width/2+.1f,c.height/2,0},{.2f,c.height,c.depth},wall);
    box({0,c.height/2,-c.depth/2-.1f},{c.width,c.height,.2f},wall);
    box({0,c.height/2,c.depth/2+.1f},{c.width,c.height,.2f},wall);
    for(float x:{-5.7f,5.7f}) for(int z=-7;z<=7;z+=2)
        box({x,1.8f,float(z)},{.035f,3.5f,.06f},{.08f,.12f,.18f});
    for(float x:{-.65f,.65f})
        box({x,.012f,0},{.025f,.018f,c.depth-.4f},{.15f,.40f,.52f});
    // One rack design repeated with different root transforms.
    for(int row=0;row<c.rackRows;++row) for(int n=0;n<c.racksPerRow;++n) {
        Rack rack;
        rack.center={(row-(c.rackRows-1)*.5f)*(c.aisleWidth+c.rackDepth),0,
                     (n-(c.racksPerRow-1)*.5f)*3.0f};
        rack.yaw=radians(row==0?90.0f:-90.0f);
        rack.root=int(nodes_.size());
        nodes_.push_back({-1,modelMatrix(rack.center,{1,1,1})*rotationY(rack.yaw)});
        rack.hinge=int(nodes_.size());
        nodes_.push_back({rack.root,modelMatrix({-c.rackWidth/2,0,c.rackDepth/2+.06f},{1,1,1})});
        auto part=[&](Vec3 p,Vec3 s,Vec3 color,Material m=Metal,float seed=0) {
            const int child=int(nodes_.size());
            nodes_.push_back({rack.root,modelMatrix(p,s)});
            static_.push_back({world(child),{color.x,color.y,color.z,float(m)},{seed,1,1,0}});
        };
        part({0,c.rackHeight/2,-c.rackDepth/2+.04f},{c.rackWidth,c.rackHeight,.08f},frame);
        for(float x:{-c.rackWidth/2+.04f,c.rackWidth/2-.04f})
            part({x,c.rackHeight/2,0},{.08f,c.rackHeight,c.rackDepth},frame);
        for(float y:{.06f,c.rackHeight-.06f})
            part({0,y,0},{c.rackWidth,.12f,c.rackDepth},frame);
        for(int unit=0;unit<8;++unit) {
            const float y=.25f+unit*.265f;
            part({0,y,0},{c.rackWidth-.16f,.235f,c.rackDepth-.12f},{.11f,.15f,.20f});
            for(int vent=0;vent<7;++vent)
                part({-.29f+vent*.065f,y,c.rackDepth/2-.048f},{.025f,.12f,.012f},{.012f,.018f,.027f});
            for(int led=0;led<3;++led)
                part({.20f+led*.055f,y+.035f,c.rackDepth/2-.025f},{.024f,.024f,.009f},
                     led==2?Vec3{.12f,.45f,1.0f}:Vec3{.12f,1.0f,.40f},Led,
                     float((row*c.racksPerRow+n)*29+unit*3+led+1));
        }
        racks_.push_back(rack);
    }
    for(float z:{-5.0f,0.0f,5.0f}) {
        box({0,c.height-.10f,z},{1.9f,.16f,.52f},frame);
        box({0,c.height-.19f,z},{1.65f,.035f,.32f},{.4f,.75f,1},CeilingLight);
    }
    // Cable trays, cable bundles, and supports above both rack rows.
    for(float x:{-1.6f,1.6f}) {
        for(float edge:{-.25f,.25f}) box({x+edge,3.12f,0},{.045f,.13f,13.5f},frame);
        for(int z=-6;z<=6;++z) box({x,3.07f,float(z)},{.5f,.04f,.045f},{.12f,.16f,.2f});
        for(float cable:{-.12f,0.0f,.12f}) box({x+cable,3.12f,0},{.055f,.055f,13.2f},{.06f,.11f,.15f});
        for(float z:{-5.5f,0.0f,5.5f}) box({x,3.55f,z},{.035f,.75f,.04f},frame);
    }
    box({0,1.85f,-7.86f},{2.7f,1.65f,.18f},frame);
    box({0,1.85f,-7.75f},{2.48f,1.40f,.035f},{1,1,1},Monitor);
    box({0,.77f,-7.80f},{.18f,.50f,.18f},frame);
}
void Room::update(float dt) {
    for(auto& r:racks_) {
        const float target=r.open?radians(-105.0f):0.0f;
        const float step=std::max(0.0f,dt)*radians(100.0f);
        r.angle+=std::clamp(target-r.angle,-step,step);
        nodes_[size_t(r.hinge)].local=modelMatrix({-config_.rackWidth/2,0,config_.rackDepth/2+.06f},{1,1,1})*rotationY(r.angle);
    }
}
std::vector<Instance> Room::doorInstances(int selected) const {
    std::vector<Instance> result;
    for(size_t i=0;i<racks_.size();++i) {
        const Mat4 hinge=world(racks_[i].hinge);
        const float w=config_.rackWidth,h=config_.rackHeight;
        const Vec3 color=int(i)==selected?Vec3{.15f,.65f,.85f}:Vec3{.10f,.16f,.21f};
        for(float x:{.025f,w-.025f}) result.push_back(instance(hinge,{x,h/2,0},{.05f,h,.055f},color));
        for(float y:{.05f,h-.05f}) result.push_back(instance(hinge,{w/2,y,0},{w,.10f,.055f},color));
        // Opaque grille strips avoid alpha-sorting artifacts and reveal the LEDs.
        for(int s=1;s<12;++s) result.push_back(instance(hinge,{w/2,s*h/12,0},{w-.09f,.018f,.025f},{.05f,.075f,.10f}));
        result.push_back(instance(hinge,{w-.12f,h*.48f,.05f},{.035f,.28f,.055f},{.6f,.67f,.73f}));
    }
    return result;
}
void Room::toggleDoor(int index) {
    if(index>=0 && size_t(index)<racks_.size()) racks_[size_t(index)].open=!racks_[size_t(index)].open;
}
int Room::pickRack(Vec3 origin,Vec3 direction,float reach) const {
    int selected=-1; float nearest=reach;
    for(size_t i=0;i<racks_.size();++i) {
        const auto& r=racks_[i];
        const Mat4 inverse=rotationY(-r.yaw);
        const Vec3 o=transformPoint(inverse,origin-r.center);
        const Vec3 d=transformPoint(inverse,direction);
        if(d.z>=-.001f) continue;
        const float t=(config_.rackDepth/2+.06f-o.z)/d.z;
        const Vec3 p=o+d*t;
        if(t>=0 && t<nearest && std::abs(p.x)<=config_.rackWidth/2+.06f && p.y>=0 && p.y<=config_.rackHeight) {
            nearest=t; selected=int(i);
        }
    }
    return selected;
}
bool Room::blocked(Vec3 p) const {
    constexpr float radius=.20f;
    if(std::abs(p.x)>config_.width/2-radius || std::abs(p.z)>config_.depth/2-radius) return true;
    for(const auto& r:racks_) {
        const Vec3 local=transformPoint(rotationY(-r.yaw),p-r.center);
        if(std::abs(local.x)<config_.rackWidth/2+radius && std::abs(local.z)<config_.rackDepth/2+radius) return true;
        const Vec3 door=transformPoint(rotationY(-r.angle),local-Vec3{-config_.rackWidth/2,0,config_.rackDepth/2+.06f});
        if(door.x>-radius && door.x<config_.rackWidth+radius && std::abs(door.z)<radius+.03f) return true;
    }
    return false;
}
}
