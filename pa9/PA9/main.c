#include "3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NUM_TRIANGLES 136


char * out_file_name = "C:\\Users\\matth\\Desktop\\sample.stl";

void read(char * in_file_name);

double round_to_four_places(double num);


int main()
{
    Scene3D * scene = Scene3D_create();

    Coordinate3D origin = (Coordinate3D) {0.0, 0.0, 0.0};
    Object3D * object = Object3D_create_fractal(origin, 50, 4);
    Scene3D_append(scene, object);

    Scene3D_write_stl_binary(scene, out_file_name);

    Scene3D_destroy(scene);

    return 0;
}

void read(char * in_file_name) {
    FILE * in_file = fopen(in_file_name, "rb");

    char buffer[84];

    fread(buffer, sizeof(char), 84, in_file);

    //FIRST 80
    for (int i = 0; i < 84; i++) {
        printf("%i ", buffer[i]);
    }

    //NORMAL
    int x = (12 * NUM_TRIANGLES) + 1;
    float buffer2[x];

    fread(buffer2, sizeof(float), x, in_file);

    for (int i = 0; i < x; i++) {
        printf("%.1f ", buffer2[i]);
    }



    fclose(in_file);

}
