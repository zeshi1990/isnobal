#include "library.h"
#include <stdio.h>
#include <stdlib.h>

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
