#include "gfx.h"

#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

uint32_t pixels[WIDTH * HEIGHT];

int main(void) {
    gfx_state s = {
        .pixels = (gfx_color *)pixels, .width = WIDTH, .height = HEIGHT};
    gfx_fill(s,
             (gfx_color){.as_color = {.r = 205, .g = 205, .b = 205, .a = 255}});

    gfx_color c = {.as_color = {.r = 35, .g = 35, .b = 35, .a = 255}};
    gfx_vec3f points[] = {
        {0, 0, 1},     {0, 0, 0.75},     {0, 0.25, 1},     {0, 0.25, 0.75},
        {-0.25, 0, 1}, {-0.25, 0, 0.75}, {-0.25, 0.25, 1}, {-0.25, 0.25, 0.75},
    };
    size_t points_len = sizeof(points) / sizeof(*points);
    for (size_t i = 0; i < points_len; i++) {
        gfx_vec3f point = gfx_rotate_xz(points[i], M_PI / 32);
        printf("point={.x=%02f, .y=%02f, .z=%02f}\n", point.x, point.y,
               point.z);

        gfx_vec2f projected = gfx_project(s, point);
        printf("projected={.x=%02f, .y=%02f}\n", projected.x, projected.y);

        int32_t size = 8;
        gfx_fill_rect(s, (int32_t)projected.x - size / 2,
                      (int32_t)projected.y - size / 2, size, size, c);
    }

    const char *path = "out.png";
    if (!stbi_write_png(path, WIDTH, HEIGHT, 4, pixels, WIDTH * 4)) {
        return 1;
    }
    return 0;
}
