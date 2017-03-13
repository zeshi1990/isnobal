#ifndef SNOBALIO_LIBRARY_H
#define SNOBALIO_LIBRARY_H

#include <omp.h>
#include "h/ipw.h"
#include "h/snobal.h"
#include "h/envphys.h"
#include "h/snow.h"
#include "h/radiation.h"

typedef struct {

    /*
     * Model parameters specified by the user
     *
     * run_no_snow: Run even if there is no snow
     * stop_no_snow: Stop running if there is no snow
     *
     * max_z_s_0: maximum active layer thickness (m)
     * max_h20_vol: max liquid h2o content as volume ratio
     *
     * time_step: length of current timestep, (sec)
     * current_time: start time of the current time step (sec), for our usage set it to 0.
     * time_since_out: time since the last output record (sec), for our usage set it to 0.
     *
     * Please note that tstep_info will not be specified by the user
     */

    int run_no_snow;
    int stop_no_snow;

    double max_z_s_0;
    double max_h2o_vol;

    double time_step;
    double current_time;
    double time_since_out;

} model_params;

typedef struct {

    /*
     * Model states
     *
     * z_s: snowdepth (m)
     * rho: snow density (kg/m^3)
     * T_s: snow temperature (K)
     * T_s_0: active snow layer temperature (K)
     * T_s_l: lower layer snow temperature (K)
     * h2o_sat: percentage of liquid h2o saturation (%)
     */

    double z_s;
    double rho;
    double T_s;
    double T_s_0;
    double T_s_l;
    double h2o_sat;

} model_states;

typedef struct {

    /*
     *  Climate-data input records
     *  S_n: net solar radiation (W/m^2)
     *  I_lw: Incoming longwave (thermal) radiation (W/m^2)
     *  T_a: air temperature (K)
     *  e_a: vapor pressure (Pa)
     *  u: wind speed (m/sec)
     *  T_g: Soil temperature (K)
     *  P_a: Air pressure (Pa)
     *  Note: we are not initializing ro here, we will set ro_data to False
     */

    double S_n;
    double I_lw;
    double T_a;
    double e_a;
    double u;
    double T_g;

} model_climate_inputs;

typedef struct {

    /*
     * measurement heights/depths
     * relative_hts: TRUE if the data given is relative to snow cover, FALSE if not
     * elevation: elevation of the mountain, m
     * z_g: soil temperature measurement depth, m
     * z_u: wind speed measurement height, m
     * z_T: temperature measurement height, m
     * z_0: roughness length, m
     */

    int relative_hts;
    double elevation;
    double z_g;
    double z_u;
    double z_T;
    double z_0;
} model_measure_params;

typedef struct {

    /* precipitation inputs
     * precip_now: precipitation occur for current timestep?
     * m_pp: specific mass of total precip (kg/m^2)
     * percent_snow: % of total mass that is snow (0 to 1.0)
     * rho_snow: density of snowfall, (kg/m^3)
     * T_pp: precip temperature, K
     */

    int precip_now;
    double m_pp;
    double percent_snow;
    double rho_snow;
    double T_pp;
} model_precip_inputs;

model_states * run_snobal(model_params * model_params1,
                          model_measure_params * model_measure_params1,
                          model_states * model_states1,
                          model_climate_inputs * model_climate_inputs1,
                          model_climate_inputs * model_climate_inputs2,
                          model_precip_inputs * model_precip_inputs1);

int snobal_init(model_params * model_params1,
                model_measure_params * model_measure_params1,
                model_states * model_states1,
                model_climate_inputs * model_climate_inputs1,
                model_climate_inputs * model_climate_inputs2,
                model_precip_inputs * model_precip_inputs1);

typedef struct {
    double dew_p;
    double sat_water;
} foo_struct;

typedef struct {
    double e;
    double tk;
} foo_struct_input;

double dew_point_test(double e);

double * multiple_vars_test(double e, double tk);

foo_struct * struct_test(double e, double tk);

foo_struct * struct_io_test(foo_struct_input * fsi);

void null_pointer_test(double *input);

#endif