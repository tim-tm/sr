#include "gfx.h"
#include "obj.h"

#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

uint32_t pixels[WIDTH * HEIGHT];

int main(void) {
    struct obj_parse_result *result =
        obj_parse("v 0.5 0.5 0.5\nv 0.75 0.75 0.5\n", 30);
    printf("result = %p\n", result);
    for (size_t i = 0; i < result->vertices_count; ++i) {
        struct obj_vertex vtx = result->vertices[i];
        printf("vertex:\n  type=%d\n", vtx.type);
        for (size_t j = 0; j < vtx.coordinates_count; ++j) {
            printf("  coord[%zu]=%f\n", j, vtx.coordinates[j]);
        }
    }
    return 0;

    gfx_state s = {
        .pixels = (gfx_color *)pixels, .width = WIDTH, .height = HEIGHT};
    gfx_fill(s, (gfx_color){.as_int = 0xffffffff});

    gfx_color c1 = {.as_color = {.r = 35, .g = 35, .b = 35, .a = 255}};
    gfx_color c2 = {.as_color = {.r = 100, .g = 100, .b = 100, .a = 255}};

    gfx_camera cam = {.position = (gfx_vec3f){.x = 1.3, .y = -0.2, .z = -3}};
    gfx_vec3f points[] = {
        {0.5, 0.5, 0.5},   {0.75, 0.75, 0.5},  {0.75, 0.5, 0.5},
        {0.5, 0.75, 0.5},  {0.5, 0.5, 0.75},   {0.5, 0.75, 0.75},
        {0.75, 0.5, 0.75}, {0.75, 0.75, 0.75},
    };
    size_t points_len = sizeof(points) / sizeof(*points);
    for (size_t i = 0; i < points_len; i++) {
        gfx_vec3f point = points[i];
        gfx_vec2f projected = gfx_project(cam, point);
        printf("x=%f, y=%f\n", projected.x, projected.y);
        gfx_vec2f screen_point = {
            .x = projected.x * s.width,
            .y = projected.y * s.height,
        };

        gfx_color c = c1;
        if (i >= 4)
            c = c2;
        int32_t size = 8;
        gfx_fill_rect(s, (int32_t)screen_point.x - size / 2,
                      (int32_t)screen_point.y - size / 2, size, size, c);
    }

    const char *path = "sr_fig_1.png";
    if (!stbi_write_png(path, WIDTH, HEIGHT, 4, pixels, WIDTH * 4)) {
        return 1;
    }
    return 0;
}
