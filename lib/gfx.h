#ifndef DRAW_H_
#define DRAW_H_

#include <assert.h>
#include <math.h>
#include <stdint.h>

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } as_color;
    uint32_t as_int;
} gfx_color;

typedef struct {
    gfx_color *pixels;
    uint32_t width;
    uint32_t height;
} gfx_state;

typedef struct {
    int32_t x;
    int32_t y;
} gfx_vec2i;

typedef struct {
    float x;
    float y;
} gfx_vec2f;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} gfx_vec3i;

typedef struct {
    float x;
    float y;
    float z;
} gfx_vec3f;

typedef struct {
    gfx_vec3f position;
} gfx_camera;

#define GFX_PIXEL_AT(s, x, y) (s).pixels[(y) * (s).width + (x)]

void gfx_fill(gfx_state state, gfx_color color);
void gfx_fill_rect(gfx_state state, uint32_t x, uint32_t y, uint32_t w,
                   uint32_t h, gfx_color color);
void gfx_line(gfx_state state, uint32_t start_x, uint32_t start_y,
              uint32_t end_x, uint32_t end_y, gfx_color color);

gfx_vec2f gfx_project(gfx_camera camera, gfx_vec3f point);
gfx_vec2f gfx_rotate(gfx_vec2f point, float theta);
gfx_vec3f gfx_rotate_xz(gfx_vec3f point, float theta);

void gfx_fill(gfx_state state, gfx_color color) {
    for (uint32_t x = 0; x < state.width; x++) {
        for (uint32_t y = 0; y < state.height; y++) {
            GFX_PIXEL_AT(state, x, y) = color;
        }
    }
}

void gfx_fill_rect(gfx_state state, uint32_t x, uint32_t y, uint32_t w,
                   uint32_t h, gfx_color color) {
    for (uint32_t x1 = x; x1 <= x + w; x1++) {
        for (uint32_t y1 = y; y1 <= y + h; y1++) {
            GFX_PIXEL_AT(state, x1, y1) = color;
        }
    }
}

void gfx_line(gfx_state state, uint32_t start_x, uint32_t start_y,
              uint32_t end_x, uint32_t end_y, gfx_color color) {
    if (start_x >= end_x || start_y >= end_y)
        return;

    uint32_t dx = end_x - start_x;
    uint32_t dy = end_y - start_y;
    float m = (float)dy / (float)dx;
    for (uint32_t x = start_x; x <= end_x; x++) {
        uint32_t y = m * (x - start_x) + start_y;
        GFX_PIXEL_AT(state, x, y) = color;
    }
}

gfx_vec2f gfx_project(gfx_camera camera, gfx_vec3f point) {
    assert(point.z != camera.position.z);

    float factor = 1 / (point.z - camera.position.z);
    return (gfx_vec2f){.x = factor * (camera.position.x * point.z -
                                      camera.position.z * point.x),
                       .y = factor * (camera.position.y * point.z -
                                      camera.position.z * point.y)};
}

gfx_vec2f gfx_rotate(gfx_vec2f point, float theta) {
    float s = sinf(theta);
    float c = cosf(theta);
    return (gfx_vec2f){.x = point.x * c - point.y * s,
                       .y = point.x * s + point.y * c};
}

gfx_vec3f gfx_rotate_xz(gfx_vec3f point, float theta) {
    float s = sinf(theta);
    float c = cosf(theta);
    return (gfx_vec3f){.x = point.x * c - point.z * s,
                       .y = point.y,
                       .z = point.x * s + point.z * c};
}

#endif // GFX_H_
