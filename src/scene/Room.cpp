#include "server_room/scene/Room.hpp"

namespace server_room {
std::vector<Box> makeRoom(const RoomConfig& c) {
    std::vector<Box> boxes;
    const Vec3 wall{0.22f, 0.28f, 0.35f}, dark{0.035f, 0.055f, 0.08f};
    boxes.push_back({{0,-.1f,0}, {c.width,.2f,c.depth}, {.16f,.21f,.27f}});
    boxes.push_back({{0,c.height+.1f,0}, {c.width,.2f,c.depth}, wall});
    boxes.push_back({{-c.width/2-.1f,c.height/2,0}, {.2f,c.height,c.depth}, wall});
    boxes.push_back({{c.width/2+.1f,c.height/2,0}, {.2f,c.height,c.depth}, wall});
    boxes.push_back({{0,c.height/2,-c.depth/2-.1f}, {c.width,c.height,.2f}, wall});
    boxes.push_back({{0,c.height/2,c.depth/2+.1f}, {c.width,c.height,.2f}, wall});
    for (int z = -7; z <= 7; z += 2)
        boxes.push_back({{0,.005f,float(z)}, {c.width,.008f,.018f}, dark});
    boxes.push_back({{-.65f,.012f,0}, {.035f,.01f,c.depth}, {.12f,.48f,.7f}, .3f});
    boxes.push_back({{.65f,.012f,0}, {.035f,.01f,c.depth}, {.12f,.48f,.7f}, .3f});
    for (int row = 0; row < c.rackRows; ++row) {
        const float x = (row - (c.rackRows-1)*.5f) * (c.aisleWidth + c.rackWidth);
        for (int rack = 0; rack < c.racksPerRow; ++rack) {
            const float z = (rack - (c.racksPerRow-1)*.5f) * 2.5f;
            boxes.push_back({{x,c.rackHeight/2,z}, {c.rackWidth,c.rackHeight,c.rackDepth}, dark});
            for (int unit = 0; unit < 7; ++unit) {
                const float y = .25f + unit*.29f;
                boxes.push_back({{x,y,z+c.rackDepth/2+.025f},
                                 {c.rackWidth*.86f,.23f,.05f}, {.13f,.18f,.24f}});
                boxes.push_back({{x+.3f,y,z+c.rackDepth/2+.06f},
                                 {.035f,.035f,.018f}, {.15f,.9f,.55f}, 1});
            }
        }
    }
    for (float z : {-5.0f, 0.0f, 5.0f})
        boxes.push_back({{0,c.height-.08f,z}, {1.6f,.08f,.35f}, {.35f,.7f,1.0f}, 1});
    return boxes;
}
}
