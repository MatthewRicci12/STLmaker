//Object count is long, not int. Also scene count.

/** FILE: 3d.c
 * AUTHOR: Matthew Ricci
 * COURSE: CSC 352
 * DESCRIPTION:
 * The implementing file for 3d.h. I am quite proud of this code for being very
 * organized, decomposed, and easy to follow (unlike the last PA). Uses quite
 * a few helpers.
 */
#include "3d.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define HEADER_ZEROES 80

//Made constants since they're consistent across all facets.
float normal[3] = {0.0, 0.0, 0.0};
uint16_t final_int = 0;

/* Since we must note the total number of triangles, before actually adding facets,
 * the only other option besides making this a global variable, is to loop through
 * every single object and tally up the number of triangles. But then we would of
 * course have to do that in order to write the binary itself, so we're looking at
 * O(N^2) time there. So this is just more efficient.
 */
int total_num_of_triangles = 0;


/**
 * Appends a quadrilateral to an Object3D, by constructing a square made of 4
 * right triangles all stacked on top of each other to avoid ambiguity. Not my
 * code.
 *
 * PARAMS
 * Object3D * object, a pointer to an object, to which we will append the shape.
 * Coordinate3D a-d, arbitrary points representing the 4 corners of the square.
 *
 */
void Object3D_append_quadrilateral(Object3D * object, Coordinate3D a,
                                   Coordinate3D b, Coordinate3D c, Coordinate3D d) {

    Triangle3D triangle_1 = (Triangle3D) {a, b, c};
    Triangle3D triangle_2 = (Triangle3D) {b, c, d};
    Triangle3D triangle_3 = (Triangle3D) {a, c, d};
    Triangle3D triangle_4 = (Triangle3D) {a, b, d};

    Object3D_append_triangle(object, triangle_1);
    Object3D_append_triangle(object, triangle_2);
    Object3D_append_triangle(object, triangle_3);
    Object3D_append_triangle(object, triangle_4);
}

/**
 * A helper method, that takes whatever a, b, c, d, and pyramid top values were
 * calculated by the main pyramid function, and is able to construct any pyramid
 * provided those values are correct. Definitely prevented the main method from being
 * too many lines.
 *
 * PARAMS
 * Object3D * object, a pointer to an object, to which we will append the shape.
 * Coordinate3D a-d, arbitrary points representing the 4 corners of the square
 *      base.
 * Coordinate3D pyramid_top, the top point of the pyramid.
 *
 */
void pyramid_helper(Object3D * new_object, Coordinate3D a, Coordinate3D b,
                    Coordinate3D c, Coordinate3D d, Coordinate3D pyramid_top) {

  //Make quadrilateral based on the values we calculated
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //Connect triangles at the height
    //BASE: AB
    Triangle3D triangle1 = (Triangle3D) {pyramid_top, a, b};


    //BASE: BD
    Triangle3D triangle2 = (Triangle3D) {pyramid_top, b, d};


    //BASE: CA
    Triangle3D triangle3 = (Triangle3D) {pyramid_top, c, a};


    //BASE: CD
    Triangle3D triangle4 = (Triangle3D) {pyramid_top, c, d};

    Object3D_append_triangle(new_object, triangle1);
    Object3D_append_triangle(new_object, triangle2);
    Object3D_append_triangle(new_object, triangle3);
    Object3D_append_triangle(new_object, triangle4);
}

/**
 * Appends a whole pyramid to the object, by construction a quadrilateral, which
 * consists of 4 triangles, then 4 more triangles that meet at the top point. You
 * can pass in an orientation, to influence in which direction the pyramid will point.
 * right/left --> +- respectively on the x axis.
 * forward/backward --> +- respectively on the y axis.
 * up/down --> +- respectively on the z axis.
 *
 * PARAMS
 * Coordinate3D origin, the middle of the square base of the pyramid.
 * double width, the width of the square base of the pyramid.
 * double height, the height of the square base of the pyramid.
 * char * orientation, which direction the pyramid top will point. See main
 *      description for info on the options.
 *
 * RETURNS
 * A pointer to an Object3D, containing the shapes needed for this pyramid.
 *
 */
Object3D* Object3D_create_pyramid(
    Coordinate3D origin,
    double width, double height, char* orientation) {
    //For consistency's sake, I'm passing in the order: (x, y, z)

    Object3D * new_object = malloc(sizeof(Object3D));
    new_object->root = NULL;

    double delta = width/2;

    double origin_x = origin.x;
    double origin_y = origin.y;
    double origin_z = origin.z;

    Coordinate3D a;
    Coordinate3D b;
    Coordinate3D c;
    Coordinate3D d;
    Coordinate3D pyramid_top;

    if (strcmp(orientation, "right") == 0) {
        //x coordinate of all = x + height, x coord does not change between abcd
        a = (Coordinate3D){origin_x, origin_y + delta, origin_z + delta};
        b = (Coordinate3D){origin_x, origin_y + delta, origin_z - delta};
        c = (Coordinate3D){origin_x, origin_y - delta, origin_z + delta};
        d = (Coordinate3D){origin_x, origin_y - delta, origin_z - delta};
        pyramid_top = (Coordinate3D){origin_x + height, origin_y, origin_z};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);

    } else if (strcmp(orientation, "left") == 0) {
        //x coordinate of all = x - height, x coord does not change between abcd
        a = (Coordinate3D){origin_x, origin_y + delta, origin_z + delta};
        b = (Coordinate3D){origin_x, origin_y + delta, origin_z - delta};
        c = (Coordinate3D){origin_x, origin_y - delta, origin_z + delta};
        d = (Coordinate3D){origin_x, origin_y - delta, origin_z - delta};
        pyramid_top = (Coordinate3D){origin_x - height, origin_y, origin_z};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);


    } else if (strcmp(orientation, "forward") == 0) {
        //y coordinate of all = y + height, y coord does not change between abcd
        a = (Coordinate3D){origin_x + delta, origin_y, origin_z + delta};
        b = (Coordinate3D){origin_x + delta, origin_y, origin_z - delta};
        c = (Coordinate3D){origin_x - delta, origin_y, origin_z + delta};
        d = (Coordinate3D){origin_x - delta, origin_y, origin_z - delta};
        pyramid_top = (Coordinate3D){origin_x, origin_y + height, origin_z};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);

    } else if (strcmp(orientation, "backward") == 0) {
        //y coordinate of all = y - height, y coord does not change between abcd
        a = (Coordinate3D){origin_x + delta, origin_y, origin_z + delta};
        b = (Coordinate3D){origin_x + delta, origin_y, origin_z - delta};
        c = (Coordinate3D){origin_x - delta, origin_y, origin_z + delta};
        d = (Coordinate3D){origin_x - delta, origin_y, origin_z - delta};
        pyramid_top = (Coordinate3D){origin_x, origin_y - height, origin_z};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);


    } else if (strcmp(orientation, "up") == 0) {
        //z coordinate of all = z + height, z coord does not change between abcd
        a = (Coordinate3D){origin_x + delta, origin_y + delta, origin_z};
        b = (Coordinate3D){origin_x + delta, origin_y - delta, origin_z};
        c = (Coordinate3D){origin_x - delta, origin_y + delta, origin_z};
        d = (Coordinate3D){origin_x - delta, origin_y - delta, origin_z};
        pyramid_top = (Coordinate3D){origin_x, origin_y, origin_z + height};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);

    } else if (strcmp(orientation, "down") == 0) {
        //z coordinate of all = z - height, z coord does not change between abcd
        a = (Coordinate3D){origin_x + delta, origin_y + delta, origin_z};
        b = (Coordinate3D){origin_x + delta, origin_y - delta, origin_z};
        c = (Coordinate3D){origin_x - delta, origin_y + delta, origin_z};
        d = (Coordinate3D){origin_x - delta, origin_y - delta, origin_z};
        pyramid_top = (Coordinate3D){origin_x, origin_y, origin_z - height};
        pyramid_helper(new_object, a, b, c, d, pyramid_top);

    } else {
        fprintf(stderr, "Invalid orientation string.");
        exit(1);
    }
    new_object->count = 8;
    total_num_of_triangles += 8;

    return new_object;
}



/**
 * Makes a new Object3D from scratch on the heap and returns its pointer.
 *
 * RETURNS
 * A pointer to a newly made Object3D.
 *
 */
Object3D * create_object() {
    Object3D * new_object = malloc(sizeof(Object3D));
    new_object->root = NULL; new_object->count = 0;
    return new_object;
}

/**
 * A helper method that destroys a linked list of triangle nodes.
 *
 * PARAMS
 * Triangle3DNode * to_destroy, the starting Triangle3DNode * to destroy.
 *
 */
void destroy_triangle_nodes(Triangle3DNode * to_destroy) {
    while (to_destroy != NULL) {
        Triangle3DNode * store = to_destroy->next;
        free(to_destroy);
        to_destroy = store;
    }
}

/**
 * A helper method that destroys an individual object.
 *
 * PARAMS
 * Object3D * to_destroy, a pointer to the Object3D to destroy.
 *
 */
void destroy_object(Object3D * to_destroy) {
    destroy_triangle_nodes(to_destroy->root);
    free(to_destroy);
}

/**
 * A helper method that creates a new triangle node to be used in a larger list.
 *
 * PARAMS
 * Triangle3D triangle, a freshly made triangle object to be this node's value.
 *
 * RETURNS
 * A pointer to a newly made Triangle3DNode.
 *
 */
Triangle3DNode * create_triangle_node(Triangle3D triangle) {
    Triangle3DNode * new_node = malloc(sizeof(Triangle3DNode));
    new_node->triangle = triangle; new_node->next = NULL;
    return new_node;
}

/**
 * Adds a triangle node to the linked list of triangle nodes contained within an
 * object.
 *
 * PARAMS
 * Triangle3DNode * node, the root node of the object.
 * Triangle3DNode * new_node, the new node to append.
 *
 */
void add_triangle_node(Triangle3DNode * node, Triangle3DNode * new_node) {
    while (node->next != NULL) {
        node = node->next;
    }
    node->next = new_node;
}

/**
 * Creates a new Scene3D on the heap and returns its pointer.
 *
 * RETURNS
 * A pointer to a freshly made Scene3D.
 *
 */
Scene3D* Scene3D_create() {
    Scene3D * new_scene = malloc(sizeof(Scene3D));
    new_scene->count = 0; new_scene->size = 0;
    new_scene->objects = NULL;
    total_num_of_triangles = 0;
    return new_scene;

}

/**
 * Frees the memory associated with a Scene3D.
 *
 * PARAMS
 * Scene3D* scene, the scene to destroy.
 *
 */
void Scene3D_destroy(Scene3D* scene) {
    Object3D ** ptr = scene->objects;
    Object3D * cur_obj;
    int scene_count = scene->count;
    for (int i = 0; i < scene_count; i++) {
        cur_obj = *(ptr + i);
        destroy_object(cur_obj);
    }
    free(ptr);
    free(scene);
}

/**
 * Appends a new object to the end of the list of objects, that this scene contains.
 *
 * PARAMS
 * Scene3D* scene, a pointer to the scene to append to.
 * Object3D* object, a pointer to the object to append to this scene.
 *
 */
void Scene3D_append(Scene3D* scene, Object3D* object) {
    //Increment count
    scene->count++;
    int count = scene->count;
    Object3D ** objs = scene->objects;

    //Make an array with only one element in it, the new object
    if (objs == NULL) {
        objs = malloc(sizeof(Object3D *));
        *objs = object;
        scene->objects = objs;
        count = scene->count;
    } else {
        //Realloc, free old pointer if we need a new address
        Object3D ** new_obj_array = realloc(objs, sizeof(Object3D*) * count);
        if (new_obj_array == NULL) {
            fprintf(stderr, "Realloc error\n");
            exit(1);
        }
        *(new_obj_array + (count - 1)) = object;
        scene->objects = new_obj_array;
    }
}

/**
 * Append a triangle node to an object. Works pretty much exactly like how a
 * linked list would work.
 *
 * PARAMS
 * Object3D * object, a pointer to the object you want to append a triangle to.
 * Triangle3D triangle, the triangle to append to this object.
 *
 */
void Object3D_append_triangle(
    Object3D* object, Triangle3D triangle) {
    Triangle3DNode * triangle_node = create_triangle_node(triangle);
    if (object->root == NULL) {
        object->root = triangle_node;
    } else {
        add_triangle_node(object->root, triangle_node);
    }
    object->count++;
}


/**
 * Creates an Object3D that will show up like a proper cube. Just specify the
 * dead middle of the cube, its width/height/depth (x-y-z) and you're good to go.
 *
 * PARAMS
 * Coordinate3D origin, the smack middle of the cube.
 * double width, how much it will extend out into the x axis.
 * double height, how much it will extend out into the y axis.
 * double depth, how much it will extend out into the z axis.
 *
 * RETURNS
 * A pointer to an Object3D that will show up as a cube.
 *
 */
Object3D* Object3D_create_cuboid(
    Coordinate3D origin,
    double width, double height, double depth) {

    Object3D * new_object = malloc(sizeof(Object3D));
    new_object->root = NULL; new_object->count = 0;

    //Width --> x axis
    //Height --> y axis
    //Depth --> z axis
    double width_delta = width/2;
    double height_delta = height/2;
    double depth_delta = depth/2;

    double origin_x = origin.x;
    double origin_y = origin.y;
    double origin_z = origin.z;

    Coordinate3D a;
    Coordinate3D b;
    Coordinate3D c;
    Coordinate3D d;

    //Base square: Flat, no height yet, meaning the Y does not change.
    a = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z + depth_delta};
    b = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z + depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //The "roof" of that. Simply increase the y.
    a.y += height;
    b.y += height;
    c.y += height;
    d.y += height;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //X-axis facing wall: X does not change.
    a = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z + depth_delta};
    b = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x - width_delta, origin_y  - height_delta, origin_z + depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y  - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //Increase x
    a.x += width;
    b.x += width;
    c.x += width;
    d.x += width;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //X-axis facing wall: X does not change.
    a = (Coordinate3D){origin_x + width_delta, origin_y + height_delta, origin_z - depth_delta};
    b = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z - depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //Increase x
    a.z += depth;
    b.z += depth;
    c.z += depth;
    d.z += depth;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    new_object->count = 24;
    total_num_of_triangles += 24;

    return new_object;

}

/**
 * A helper method in the writing chain that writes the coordinates of a single
 * Triangle3D.
 *
 * PARAMS
 * Triangle3D triangle, the Triangle3D whose info we must write.
 * FILE * file, the file to write to.
 *
 */
void write_triangle(Triangle3D triangle, FILE * file) {
    fprintf(file, "      vertex %.5lf %.5lf %.5lf\n", triangle.a.x, triangle.a.y, triangle.a.z);
    fprintf(file, "      vertex %.5lf %.5lf %.5lf\n", triangle.b.x, triangle.b.y, triangle.b.z);
    fprintf(file, "      vertex %.5lf %.5lf %.5lf\n", triangle.c.x, triangle.c.y, triangle.c.z);
}

/**
 * A helper method in the writing chain that writes each Triangle3DNode present
 * in the object that called it.
 *
 * PARAMS
 * Triangle3DNode * node, the current Triangle3DNode, whose info we are writing.
 * FILE * file, the file to write to.
 *
 */
void write_triangle_nodes(Triangle3DNode * node, FILE * file) {
    while (node->next != NULL) {
        fprintf(file, "  facet normal 0.0 0.0 0.0\n    outer loop\n");
        write_triangle(node->triangle, file);
        fprintf(file, "    endloop\n");
        node = node->next;
        fprintf(file, "  endfacet\n");
    }
    fprintf(file, "  facet normal 0.0 0.0 0.0\n    outer loop\n");
    write_triangle(node->triangle, file);
    fprintf(file, "    endloop\n");
    fprintf(file, "  endfacet\n");
}

/**
 * A helper method in the writing chain that writes each Object3D present
 * in the scene that called it.
 *
 * PARAMS
 * Object3D * object, the current Object3D whose info we are writing.
 * FILE * file, the file to write to.
 *
 */
void write_object(Object3D * object, FILE * file) {
    write_triangle_nodes(object->root, file);
}


/**
 * The main method in the writing chain that writes the scene to completion,
 * recursing hierarchically and controlling the flow of said recursion.
 *
 * PARAMS
 * Scene3D * scene, a pointer to the scene whose info we will write entirely.
 * char * file, the NAME of the file to write to.
 *
 */
void Scene3D_write_stl_text(Scene3D* scene, char* file_name) {
    FILE * out_file = fopen(file_name, "w");

    fprintf(out_file, "solid scene\n");
    Object3D ** ptr = scene->objects;
    Object3D * cur_obj;
    int scene_count = scene->count;
    for (int i = 0; i < scene_count; i++) {
        cur_obj = *(ptr + i);
        write_object(cur_obj, out_file);
    }
    fprintf(out_file, "endsolid scene");
    fclose(out_file);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////PA9



void write_triangle_binary(Triangle3D triangle, FILE * file) {
    //Write all 9 coordinates
    float coords[9] = {triangle.a.x, triangle.a.y, triangle.a.z, triangle.b.x, triangle.b.y, triangle.b.z,
                       triangle.c.x, triangle.c.y, triangle.c.z};
    fwrite(coords, sizeof(float), 9, file);
    //That final 0
    fwrite(&final_int, sizeof(uint16_t), 1, file);

}


void write_triangle_nodes_binary(Triangle3DNode * node, FILE * file) {

    while (node->next != NULL) {
        //Write the normal
        fwrite(normal, sizeof(float), 3, file);
        write_triangle_binary(node->triangle, file);
        node = node->next;
    }
    fwrite(normal, sizeof(float), 3, file);
    write_triangle_binary(node->triangle, file);
}

/**
 * A helper method in the writing chain that writes each Object3D present
 * in the scene that called it.
 *
 * PARAMS
 * Object3D * object, the current Object3D whose info we are writing.
 * FILE * file, the file to write to.
 *
 */
void write_object_binary(Object3D * object, FILE * file) {
    write_triangle_nodes_binary(object->root, file);
}



void Scene3D_write_stl_binary(Scene3D* scene, char* file_name) {

    FILE * file = fopen(file_name, "wb");

    //Append 80 zeroes out bytes to file.
    char zeroes[HEADER_ZEROES];
    for (int i = 0; i < HEADER_ZEROES; i++) {
        zeroes[i] = '\0';
    }
    fwrite(zeroes, 1, HEADER_ZEROES, file);

    //Write number of triangles (stored as global variable)
    fwrite(&total_num_of_triangles, sizeof(uint32_t), 1, file);
    int test = total_num_of_triangles;

    Object3D ** ptr = scene->objects;
    Object3D * cur_obj;
    int scene_count = scene->count;

    //Iterate thru our objects for writing
    for (int i = 0; i < scene_count; i++) {
        cur_obj = *(ptr + i);
        write_object_binary(cur_obj, file);
    }

    fclose(file);

}


double round_to_four_places(double num) {
   return round(num * 10000) / 10000;
}

double degrees_to_radians(double degrees) {
    return degrees;
}


Object3D* Object3D_create_sphere(Coordinate3D origin, double radius,
                                 double increment) {

    Object3D * new_object = malloc(sizeof(Object3D));
    new_object->root = NULL; new_object->count = 0;

    double origin_x = origin.x;
    double origin_y = origin.y;
    double origin_z = origin.z;


    for (double phi = increment; phi <= 180; phi += increment) {
        for (double theta = 0; theta < 360; theta += increment) {

            //No change
            double x = 0.0; double y = 0.0; double z = 0.0;
            //First step: calculate x, y, z, converting any radians to degrees.
            //Origin x/y/z + the result, to 4 decimal places

            //RADIAN INPUT
            x = radius * degrees_to_radians(sin(phi) * cos(theta));
            y = radius * degrees_to_radians(sin(phi) * sin(theta));
            z = radius * degrees_to_radians(cos(phi));
            Coordinate3D coord1 = (Coordinate3D) {origin_x + round_to_four_places(x),
            origin_y + round_to_four_places(y), origin_z + round_to_four_places(z)};

            //Phi - increment
            x = radius * degrees_to_radians(sin(phi - increment) * cos(theta));
            y = radius * degrees_to_radians(sin(phi - increment) * sin(theta));
            z = radius * degrees_to_radians(cos(phi - increment));
            Coordinate3D coord2 = (Coordinate3D) {origin_x + round_to_four_places(x),
            origin_y + round_to_four_places(y), origin_z + round_to_four_places(z)};

            //Theta - increment
            x = radius * degrees_to_radians(sin(phi) * cos(theta - increment));
            y = radius * degrees_to_radians(sin(phi) * sin(theta - increment));
            z = radius * degrees_to_radians(cos(phi));
            Coordinate3D coord3 = (Coordinate3D) {origin_x + round_to_four_places(x),
            origin_y + round_to_four_places(y), origin_z + round_to_four_places(z)};

            //Phi - increment, theta - increment
            x = radius * degrees_to_radians(sin(phi - increment) * cos(theta - increment));
            y = radius * degrees_to_radians(sin(phi - increment) * sin(theta - increment));
            z = radius * degrees_to_radians(cos(phi - increment));
            Coordinate3D coord4 = (Coordinate3D) {origin_x + round_to_four_places(x),
            origin_y + round_to_four_places(y), origin_z + round_to_four_places(z)};

            Object3D_append_quadrilateral(new_object, coord1, coord2, coord3, coord4);
        }

    }

    total_num_of_triangles += new_object->count;

    return new_object;

}


void Object3D_append_cuboid(Object3D * new_object, Coordinate3D origin, double width, double height, double depth) {


    //Width --> x axis
    //Height --> y axis
    //Depth --> z axis
    double width_delta = width/2;
    double height_delta = height/2;
    double depth_delta = depth/2;

    double origin_x = origin.x;
    double origin_y = origin.y;
    double origin_z = origin.z;

    Coordinate3D a;
    Coordinate3D b;
    Coordinate3D c;
    Coordinate3D d;

    //Base square: Flat, no height yet, meaning the Y does not change.
    a = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z + depth_delta};
    b = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z + depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //The "roof" of that. Simply increase the y.
    a.y += height;
    b.y += height;
    c.y += height;
    d.y += height;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //X-axis facing wall: X does not change.
    a = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z + depth_delta};
    b = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x - width_delta, origin_y  - height_delta, origin_z + depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y  - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //Increase x
    a.x += width;
    b.x += width;
    c.x += width;
    d.x += width;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //X-axis facing wall: X does not change.
    a = (Coordinate3D){origin_x + width_delta, origin_y + height_delta, origin_z - depth_delta};
    b = (Coordinate3D){origin_x - width_delta, origin_y + height_delta, origin_z - depth_delta};
    c = (Coordinate3D){origin_x + width_delta, origin_y - height_delta, origin_z - depth_delta};
    d = (Coordinate3D){origin_x - width_delta, origin_y - height_delta, origin_z - depth_delta};
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    //Increase x
    a.z += depth;
    b.z += depth;
    c.z += depth;
    d.z += depth;
    Object3D_append_quadrilateral(new_object, a, b, c, d);

    new_object->count += 24;
    total_num_of_triangles += 24;


}

void fractal_helper(Object3D * object, Coordinate3D origin, double size, int level) {
    if (level == 1) {
        return;
    } else {

    double origin_x = origin.x;
    double origin_y = origin.y;
    double origin_z = origin.z;

    double delta = size/2;
    int new_level = --level;

    Coordinate3D a = (Coordinate3D) {origin_x + delta, origin_y, origin_z};
    Coordinate3D b = (Coordinate3D) {origin_x - delta, origin_y, origin_z};
    Coordinate3D c = (Coordinate3D) {origin_x, origin_y + delta, origin_z};
    Coordinate3D d = (Coordinate3D) {origin_x, origin_y - delta, origin_z};
    Coordinate3D e = (Coordinate3D) {origin_x, origin_y, origin_z + delta};
    Coordinate3D f = (Coordinate3D) {origin_x, origin_y, origin_z - delta};

    //Object3D * new_object, Coordinate3D origin, double width, double height, double depth
    Object3D_append_cuboid(object, a, delta, delta, delta);
    Object3D_append_cuboid(object, b, delta, delta, delta);
    Object3D_append_cuboid(object, c, delta, delta, delta);
    Object3D_append_cuboid(object, d, delta, delta, delta);
    Object3D_append_cuboid(object, e, delta, delta, delta);
    Object3D_append_cuboid(object, f, delta, delta, delta);

    //Object3D * object, Coordinate3D origin, double size, int level
    fractal_helper(object, a, delta, new_level);
    fractal_helper(object, b, delta, new_level);
    fractal_helper(object, c, delta, new_level);
    fractal_helper(object, d, delta, new_level);
    fractal_helper(object, e, delta, new_level);
    fractal_helper(object, f, delta, new_level);

    }
}

Object3D* Object3D_create_fractal(Coordinate3D origin, double size, int levels) {
    Object3D * new_object = malloc(sizeof(Object3D));
    new_object->root = NULL;

    Object3D_append_cuboid(new_object, origin, size, size, size);

    fractal_helper(new_object, origin, size, levels);

    return new_object;


}
