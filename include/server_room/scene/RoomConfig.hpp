#pragma once

namespace server_room {

// Initial layout values for the team to adjust before creating geometry.
// Dimensions use meters; Y is up.
struct RoomConfig {
    float width = 12.0f;
    float depth = 16.0f;
    float height = 4.0f;

    int rackRows = 2;
    int racksPerRow = 4;
    float rackWidth = 1.0f;
    float rackDepth = 1.2f;
    float rackHeight = 2.4f;
    float aisleWidth = 2.0f;
};

} // namespace server_room
