#include "snobalio.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Belows are some foo functions used for testing
 * the interface with ctypes in Python and help
 * Zeshi get familiar with writing c code :).
 */

double dew_point_test(double e) {
    double dew_p = dew_point(e);
    return dew_p;
}

double * multiple_vars_test(double e, double tk) {
    double dew_p = dew_point(e);
    double sat_water = satw(tk);
    static double r[2];
    r[0] = dew_p;
    r[1] = sat_water;
    return r;
}

foo_struct * struct_test(double e, double tk) {
    double dew_p = dew_point(e);
    double sat_water = satw(tk);
    foo_struct r;
    foo_struct *result = malloc(sizeof(foo_struct));
    r.dew_p = dew_p;
    r.sat_water = sat_water;
    *result = r;
    return result;
}

foo_struct * struct_io_test(foo_struct_input * fsi) {
    double e = fsi->e;
    double tk = fsi->tk;
    foo_struct r;
    foo_struct *result = malloc(sizeof(foo_struct));
    r.dew_p = dew_point(e);
    r.sat_water = satw(tk);
    *result = r;
    return result;
}

void null_pointer_test(double *input) {
    if(input) {
        printf("The input pointer has value %f\n", *input);
    } else {
        printf("The input pointer is NULL\n");
    }
}

void pointer_1darray_test(long length, double *data) {
    long i;
    printf("[");
    for (i = 0; i < length; i++) {
        if (i != (length-1)) {
            printf("%f, ", data[i]);
        } else {
            printf("%f", data[i]);
        }
    }
    printf("]\n");
}

void pointer_2darray_test(long nrows, long ncols, double **data1, double **data2) {
    long i;
    long j;
    printf("[");
    for (i = 0; i < nrows; i++) {
        for (j=0; j < ncols; j++) {
            printf("%f, ", data1[i][j]);
        }
        if (i != (nrows - 1)) {
            printf("\b\b\n ");
        }
    }
    printf("\b\b]\n");

    printf("[");
    for (i = 0; i < nrows; i++) {
        for (j=0; j < ncols; j++) {
            printf("%f, ", data2[i][j]);
        }
        if (i != (nrows - 1)) {
            printf("\b\b\n ");
        }
    }
    printf("\b\b]\n");
}

void pointer_struct_1darray_test(long length, model_measure_params_1d *model_measure_params_1d1) {
    length_1d = length;
    long i;

    printf("relative heights is %d\n", model_measure_params_1d1->relative_hts);
    printf("distance from ground for soil is %f\n", model_measure_params_1d1->z_g);
    printf("distance from ground for wind is %f\n", model_measure_params_1d1->z_u);
    printf("distance from ground for temperature is %f\n", model_measure_params_1d1->z_T);
    printf("roughness length is %f\n", model_measure_params_1d1->z_0);

    for (i = 0; i < length_1d; i++) {
        printf("elevation is %f\n", model_measure_params_1d1->i_elevation[i]);
    }
}