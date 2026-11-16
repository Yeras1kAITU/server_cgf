#pragma once
#include "server_room/graphics/Instance.hpp"
#include "server_room/scene/RoomConfig.hpp"
#include <vector>
namespace server_room {
struct SceneNode {
    int parent=-1;
    Mat4 local=modelMatrix({0,0,0},{1,1,1});
};
struct Rack {
    Vec3 center;
    float yaw=0, angle=0;
    bool open=false;
    int root=-1, hinge=-1;
};
class Room {
public:
    explicit Room(RoomConfig config={});
    const std::vector<Instance>& staticInstances() const { return static_; }
    std::vector<Instance> doorInstances(int selected) const;
    Mat4 world(int node) const;
    void update(float dt);
    int pickRack(Vec3 origin, Vec3 direction, float reach=3.0f) const;
    void toggleDoor(int index);
    bool blocked(Vec3 p) const;
    const std::vector<Rack>& racks() const { return racks_; }
    const RoomConfig& config() const { return config_; }
private:
    RoomConfig config_;
    std::vector<Instance> static_;
    std::vector<SceneNode> nodes_;
    std::vector<Rack> racks_;
};
}
