#include "gfx.h"
#include "obj.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

uint32_t pixels[WIDTH * HEIGHT];

int main(void) {
    Nob_String_Builder sb = {0};
    if (!nob_read_entire_file("./res/cube.obj", &sb))
        return 1;

    struct obj_parse_result *result = obj_parse(sb.items, sb.count);
    printf("result = %p\n", result);
    for (size_t i = 0; i < result->vertices_count; ++i) {
        struct obj_vertex vtx = result->vertices[i];
        printf("vertex:\n  type=%d\n", vtx.type);
        for (size_t j = 0; j < vtx.coordinates_count; ++j) {
            printf("  coord[%zu]=%.2f\n", j, vtx.coordinates[j]);
        }
    }

    for (size_t i = 0; i < result->faces_count; ++i) {
        struct obj_face face = result->faces[i];
        printf("face:\n");
        for (size_t j = 0; j < face.coordinates_count; ++j) {
            struct obj_vertex vert = result->vertices[face.coordinates[j] - 1];
            printf("  coord[%zu]=%lli=(", j, face.coordinates[j]);
            for (size_t k = 0; k < vert.coordinates_count; ++k) {
                printf("%.2f,", vert.coordinates[k]);
            }
            printf(")\n");
        }
    }

    gfx_state s = {
        .pixels = (gfx_color *)pixels, .width = WIDTH, .height = HEIGHT};
    gfx_fill(s, (gfx_color){.as_int = 0xffffffff});

    gfx_color col = {.as_color = {.r = 35, .g = 35, .b = 35, .a = 255}};

    gfx_camera cam = {.position = (gfx_vec3f){.x = 1.3, .y = -0.2, .z = -3}};
    /*
    gfx_vec3f points[] = {
    };
    size_t points_len = sizeof(points) / sizeof(*points);
    */
    for (size_t i = 0; i < result->faces_count; ++i) {
        struct obj_face face = result->faces[i];
        for (size_t j = 0; j < face.coordinates_count; ++j) {
            size_t k = (j + 1) % face.coordinates_count;

            struct obj_vertex vert1 = result->vertices[face.coordinates[j] - 1];
            assert(vert1.coordinates_count == 3);

            struct obj_vertex vert2 = result->vertices[face.coordinates[k] - 1];
            assert(vert2.coordinates_count == 3);

            gfx_vec3f p1 = {.x = vert1.coordinates[0],
                            .y = vert1.coordinates[1],
                            .z = vert1.coordinates[2]};
            gfx_vec2f projected_p1 = gfx_project(cam, p1);

            gfx_vec3f p2 = {.x = vert2.coordinates[0],
                            .y = vert2.coordinates[1],
                            .z = vert2.coordinates[2]};
            gfx_vec2f projected_p2 = gfx_project(cam, p2);

            gfx_line(s, projected_p1.x, projected_p1.y, projected_p2.x,
                     projected_p2.y, col);
        }
    }

    const char *path = "teapot.png";
    if (!stbi_write_png(path, WIDTH, HEIGHT, 4, pixels, WIDTH * 4)) {
        return 1;
    }
    return 0;
}
