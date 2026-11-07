#pragma once

#include "server_room/graphics/Math.hpp"
#include <algorithm>

namespace server_room {

class Camera {
public:
    Vec3 position{0, 1.7f, 6.4f};
    float yaw = -90, pitch = -5;

    Vec3 direction() const {
        return {std::cos(radians(yaw)) * std::cos(radians(pitch)),
                std::sin(radians(pitch)),
                std::sin(radians(yaw)) * std::cos(radians(pitch))};
    }
    void look(float dx, float dy) {
        yaw += dx * 0.1f;
        pitch = std::clamp(pitch - dy * 0.1f, -85.0f, 85.0f);
    }
    void move(float forward, float right, float dt, float width, float depth) {
        const Vec3 heading = normalized({direction().x, 0, direction().z});
        const Vec3 sideways = normalized(cross(heading, {0, 1, 0}));
        position = position + normalized(heading * forward + sideways * right) * (3.0f * dt);
        position.x = std::clamp(position.x, -width/2 + 0.3f, width/2 - 0.3f);
        position.z = std::clamp(position.z, -depth/2 + 0.3f, depth/2 - 0.3f);
    }
};

} // namespace server_room
