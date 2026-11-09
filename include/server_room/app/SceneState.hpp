#pragma once

namespace server_room {

// Shared interaction values. Input changes them; rendering reads them.
struct SceneState {
    bool alarmEnabled = false;

    // Normalized range [0, 1]. Rendering will map this to fog density.
    float coolingLevel = 0.35f;
};

} // namespace server_room
