#ifndef OBJ_H_
#define OBJ_H_

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum obj_vertex_type {
    OBJ_VERTEX_TYPE_GEOMETRIC = 0,
    OBJ_VERTEX_TYPE_TEXTURE,
    OBJ_VERTEX_TYPE_NORMAL,
    OBJ_VERTEX_TYPE_PARAMETER,
};

struct obj_vertex {
    enum obj_vertex_type type;
    float *coordinates;
    size_t coordinates_count;
    size_t coordinates_capacity;
};

struct obj_face {
    long long *coordinates;
    size_t coordinates_count;
    size_t coordinates_capacity;
};

struct obj_parse_result {
    struct obj_vertex *vertices;
    size_t vertices_count;
    size_t vertices_capacity;

    struct obj_face *faces;
    size_t faces_count;
    size_t faces_capacity;
};

struct obj_parse_result *obj_parse(const char *content, size_t content_len) {
    struct obj_parse_result *result = malloc(sizeof(struct obj_parse_result));
    if (result == NULL)
        return NULL;

    // there are usually a huge amount of vertices,
    // so it may even be better to choose a larger number right here
    result->vertices_capacity = 4096;
    result->vertices_count = 0;
    result->vertices =
        malloc(sizeof(struct obj_vertex) * result->vertices_capacity);
    if (result->vertices == NULL) {
        return NULL;
    }

    result->faces_capacity = 4096;
    result->faces_count = 0;
    result->faces = malloc(sizeof(struct obj_face) * result->faces_capacity);
    if (result->faces == NULL) {
        return NULL;
    }

    size_t i = 0;
    while (i < content_len && content[i] != '\0') {
        switch (content[i]) {
        case 'v': {
            if (i + 1 >= content_len) {
                printf("unterminated vertex\n");
                return NULL;
            }

            struct obj_vertex vert = {.type = -1,
                                      .coordinates = NULL,
                                      .coordinates_count = 0,
                                      .coordinates_capacity = 0};

            switch (content[i + 1]) {
            case ' ': {
                vert.type = OBJ_VERTEX_TYPE_GEOMETRIC;
                vert.coordinates_capacity = 4;
                i++;
            } break;
            case 't': {
                vert.type = OBJ_VERTEX_TYPE_TEXTURE;
                vert.coordinates_capacity = 3;
                i += 2;
            } break;
            case 'n': {
                vert.type = OBJ_VERTEX_TYPE_NORMAL;
                vert.coordinates_capacity = 3;
                i += 2;
            } break;
            case 'p': {
                vert.type = OBJ_VERTEX_TYPE_PARAMETER;
                vert.coordinates_capacity = 3;
                i += 2;
            } break;
            default: {
                printf("invalid character after 'v': '%c'\n", content[i + 1]);
                return NULL;
            } break;
            }

            vert.coordinates =
                malloc(sizeof(float) * vert.coordinates_capacity);
            if (vert.coordinates == NULL) {
                return NULL;
            }

            while (i < content_len && content[i] == ' ') {
                if (vert.coordinates_count + 1 >= vert.coordinates_capacity) {
                    printf("too much coordinates for vertex\n");
                    return NULL;
                }

                char *stop_str;
                float result = strtof(content + i + 1, &stop_str);
                if (result == HUGE_VALF || result == -HUGE_VALF) {
                    printf(
                        "float overflow on vertex, use smaller coordinates!\n");
                    return NULL;
                }
                vert.coordinates[vert.coordinates_count++] = result;

                size_t diff = stop_str - content;
                i = diff;
            }

            if (result->vertices_count >= result->vertices_capacity) {
                result->vertices_capacity *= 2;
                result->vertices =
                    realloc(result->vertices, sizeof(struct obj_vertex) *
                                                  result->vertices_capacity);
                if (result->vertices == NULL)
                    return NULL;
            }
            result->vertices[result->vertices_count++] = vert;
        } break;
        case 'f': {
            if (i + 1 >= content_len) {
                printf("unterminated face\n");
                return NULL;
            }
            i++;

            struct obj_face face = {.coordinates = NULL,
                                    .coordinates_count = 0,
                                    // faces will usually be triangles
                                    .coordinates_capacity = 3};
            face.coordinates =
                malloc(sizeof(long long) * face.coordinates_capacity);
            if (face.coordinates == NULL) {
                return NULL;
            }

            while (i < content_len && content[i] == ' ') {
                if (face.coordinates_count >= face.coordinates_capacity) {
                    face.coordinates_capacity *= 2;
                    face.coordinates =
                        realloc(face.coordinates,
                                sizeof(long long) * face.coordinates_capacity);
                    if (face.coordinates == NULL)
                        return NULL;
                }

                char *stop_str;
                long long result = strtoll(content + i + 1, &stop_str, 10);
                if (result == LLONG_MIN || result == LLONG_MAX) {
                    printf("invalid face size\n");
                    return NULL;
                }
                face.coordinates[face.coordinates_count++] = result;

                size_t diff = stop_str - content;
                i = diff;
            }

            if (result->faces_count >= result->faces_capacity) {
                result->faces_capacity *= 2;
                result->faces =
                    realloc(result->faces,
                            sizeof(struct obj_face) * result->faces_capacity);
                if (result->faces == NULL)
                    return NULL;
            }
            result->faces[result->faces_count++] = face;
        } break;
        default: {
            printf("unhandled character: %d\n", content[i]);
        } break;
        }
        i++;
    }
    return result;
}

#endif // OBJ_H_
