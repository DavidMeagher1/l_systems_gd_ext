#include "compute_2d.h"

using namespace godot;
using namespace l_systems;

#define TO_STR__(...) #__VA_ARGS__
#define TO_STR_(...) TO_STR__(__VA_ARGS__)
#define TO_STR(...) TO_STR_(__VA_ARGS__)

const char* compute::setup_compute_2d() {
    static const char* shader_code_2d = TO_STR(
#include "compute_2d.glsl"
    );
    RenderingServer *rs = RenderingServer::get_singleton();
    if (rs == nullptr) {
        return nullptr;
    }

    RenderingDevice *rd = rs->create_local_rendering_device();
    if (rd == nullptr) {
        return nullptr;
    }

    return shader_code_2d;

}



