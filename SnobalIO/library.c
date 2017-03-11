#include "library.h"

#include <stdio.h>
#include "h/envphys.h"

double dew_point_test(double e) {
    double dew_p = dew_point(e);
    return dew_p;
}