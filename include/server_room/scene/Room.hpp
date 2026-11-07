#pragma once

#include "server_room/graphics/Math.hpp"
#include "server_room/scene/RoomConfig.hpp"
#include <vector>

namespace server_room {
struct Box {
    Vec3 center, size, color;
    float emission = 0;
};
std::vector<Box> makeRoom(const RoomConfig& config);
}
