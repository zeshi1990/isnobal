//
// Created by Zeshi Zheng on 3/13/17.
//
#include <h/snobal.h>
#include "snobalio.h"

int snobal_init(model_params * model_params1,
                model_measure_params * model_measure_params1,
                model_states * model_states1,
                model_climate_inputs * model_climate_inputs1,
                model_climate_inputs * model_climate_inputs2,
                model_precip_inputs * model_precip_inputs1) {

    /*
     * If user pass model_params of the model then use them
     * else use default
     */

    if (model_params1) {
        run_no_snow = model_params1 -> run_no_snow;
        stop_no_snow = model_params1 -> stop_no_snow;

        max_z_s_0 = model_params1 -> max_z_s_0;
        max_h2o_vol = model_params1 -> max_h2o_vol;
        time_step = model_params1 -> time_step;
        current_time = model_params1 -> current_time;
        time_since_out = model_params1 -> time_since_out;
    } else {
        run_no_snow = 1;
        stop_no_snow = 0;
        max_z_s_0 = DEFAULT_MAX_Z_S_0;
        max_h2o_vol = DEFAULT_MAX_H2O_VOL;
        time_step = 86400.;
        current_time = 0.;
        time_since_out = 0.;
    }

    /*
     * If user pass measurement_params of the model then use them
     * else use default
     */

    double elevation;

    if (model_measure_params1) {
        relative_hts = model_measure_params1 -> relative_hts;
        elevation = model_measure_params1 -> elevation;
        z_g = model_measure_params1 -> z_g;
        z_u = model_measure_params1 -> z_u;
        z_T = model_measure_params1 -> z_T;
        z_0 = model_measure_params1 -> z_0;
    } else {
        relative_hts = 0;
        elevation = 2000.0;
        z_g = DEFAULT_Z_G;
        z_u = 10.;
        z_T = 10.;
        z_0 = 1.0;
    }

    P_a = HYSTAT(SEA_LEVEL, STD_AIRTMP, STD_LAPSE,
                 (elevation / 1000.), GRAVITY, MOL_AIR);

    /*
     * User has to pass in the original states of the model;
     */

    if (model_states1) {
        z_s = model_states1 -> z_s;
        rho = model_states1 -> rho;
        T_s = model_states1 -> T_s;
        T_s_0 = model_states1 -> T_s_0;
        T_s_l = model_states1 -> T_s_l;
        h2o_sat = model_states1 -> h2o_sat;
    } else {
        printf("The user has to specify the initial states of the SNOBAL model\n");
        return FALSE;
    }


    /*
     * User has to pass in the climate inputs 1 of the model;
     */

    if (model_climate_inputs1) {
        input_rec1.S_n = model_climate_inputs1 -> S_n;
        input_rec1.I_lw = model_climate_inputs1 -> I_lw;
        input_rec1.T_a = model_climate_inputs1 ->  T_a;
        input_rec1.e_a = model_climate_inputs1 -> e_a;
        input_rec1.u = model_climate_inputs1 -> u;
        input_rec1.T_g = model_climate_inputs1 -> T_g;
        ro_data = FALSE;
    } else {
        printf("The user has to specify the climatological inputs\n");
        return FALSE;
    }


    /*
     * User does not need to pass in the climate inputs 2,
     * if NULL pointer, climate inputs 2 = climate inputs 1
     */

    if (model_climate_inputs2) {
        input_rec2.S_n = model_climate_inputs2 -> S_n;
        input_rec2.I_lw = model_climate_inputs2 -> I_lw;
        input_rec2.T_a = model_climate_inputs2 ->  T_a;
        input_rec2.e_a = model_climate_inputs2 -> e_a;
        input_rec2.u = model_climate_inputs2 -> u;
        input_rec2.T_g = model_climate_inputs2 -> T_g;
        ro_data = FALSE;
    } else {
        input_rec2.S_n = model_climate_inputs1 -> S_n;
        input_rec2.I_lw = model_climate_inputs1 -> I_lw;
        input_rec2.T_a = model_climate_inputs1 ->  T_a;
        input_rec2.e_a = model_climate_inputs1 -> e_a;
        input_rec2.u = model_climate_inputs1 -> u;
        input_rec2.T_g = model_climate_inputs1 -> T_g;
        ro_data = FALSE;
    }

    /*
     * User has to put precipitation input
     */

    if (model_precip_inputs1) {
        precip_now = model_precip_inputs1 -> precip_now;
        m_pp = model_precip_inputs1 -> m_pp;
        percent_snow = model_precip_inputs1 -> percent_snow;
        rho_snow = model_precip_inputs1 -> rho_snow;
        T_pp = model_precip_inputs1 -> T_pp;
    } else {
        printf("The user has to specify the precipitation inputs\n");
        return FALSE;
    }

    // tstep_info initialization
    int level;

    for (level = DATA_TSTEP; level <= SMALL_TSTEP; level++) {
        tstep_info[level].level = level;
        tstep_info[level].output = 0;
    }

    tstep_info[DATA_TSTEP].time_step = time_step;
    tstep_info[NORMAL_TSTEP].time_step = time_step;
    tstep_info[MEDIUM_TSTEP].time_step = MIN_TO_SEC(DEFAULT_MEDIUM_TSTEP);
    tstep_info[SMALL_TSTEP].time_step = MIN_TO_SEC(DEFAULT_SMALL_TSTEP);

    tstep_info[DATA_TSTEP].threshold = DEFAULT_NORMAL_THRESHOLD;
    tstep_info[NORMAL_TSTEP].threshold = DEFAULT_NORMAL_THRESHOLD;
    tstep_info[MEDIUM_TSTEP].threshold = DEFAULT_MEDIUM_THRESHOLD;
    tstep_info[SMALL_TSTEP].threshold = DEFAULT_SMALL_THRESHOLD;

    for (level = NORMAL_TSTEP; level <= SMALL_TSTEP; level++) {
            tstep_info[level].intervals = (int) (tstep_info[level-1].time_step /
                    tstep_info[level].time_step);
    }

    return TRUE;

}