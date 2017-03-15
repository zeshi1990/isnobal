//
// Created by Zeshi Zheng on 3/13/17.
//
#include "snobalio.h"

model_states * run_snobal(model_params * model_params1,
                          model_measure_params * model_measure_params1,
                          model_states * model_states1,
                          model_climate_inputs * model_climate_inputs1,
                          model_climate_inputs * model_climate_inputs2,
                          model_precip_inputs * model_precip_inputs1) {

    if (!snobal_init(model_params1, model_measure_params1,
                     model_states1, model_climate_inputs1,
                     model_climate_inputs2, model_precip_inputs1)) {
        printf("Initialization failed");
        return NULL;
    }

    init_snow();

    if(!do_data_tstep()) {
        printf("Oops! Something happened when running snobal, please check log files.\n");
        return NULL;
    };

    model_states r;
    model_states * result = malloc(sizeof(model_states));

    r.z_s = z_s;
    r.rho = rho;
    r.T_s = T_s;
    r.T_s_0 = T_s_0;
    r.T_s_l = T_s_l;
    r.h2o_sat = h2o_sat;

    *result = r;
    return result;

}