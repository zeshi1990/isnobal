//
// Created by Zeshi Zheng on 3/21/17.
//

#include <h/snobal.h>
#include "snobalio.h"

double ** run_isnobal_1d(long length,
                         model_params * model_params1,
                         model_measure_params_1d * model_measure_params_1d1,
                         double ** model_states_1d1,
                         double ** model_climate_inputs_1d1,
                         double ** model_climate_inputs_1d2,
                         double ** model_precip_inputs_1d1) {

    const int n_states = 6;
//    const int n_climate_inputs = 6;
//    const int n_precip_inputs = 5;

    double ** states_results = (double **) malloc(sizeof(double *) * n_states);
    for (long j = 0; j < n_states; j++) {
        states_results[j] = (double *) malloc(sizeof(double) * length);
    }

    /*
     * Initialize model_measure_params1 and assign spatially static attributes
     */
    model_measure_params * model_measure_params1 = malloc(sizeof(model_measure_params));
    model_measure_params1->relative_hts = model_measure_params_1d1->relative_hts;
    model_measure_params1->z_g = model_measure_params_1d1->z_g;
    model_measure_params1->z_u = model_measure_params_1d1->z_u;
    model_measure_params1->z_T = model_measure_params_1d1->z_T;
    model_measure_params1->z_0 = model_measure_params_1d1->z_0;

    for (long i = 0; i < length; i++) {
        /*
         *  Modify elevation attribute in model_measure_params1
         */
        model_measure_params1->elevation = model_measure_params_1d1->i_elevation[i];
        printf("The elevation of current pixel is: %fm\n", model_measure_params1->elevation);

        /*
         * Initialize following
         * model_states1,
         * model_climate_inputs1,
         * model_climate_inputs2,
         * model_precip_inputs1
         */
        model_states * model_states1 = malloc(sizeof(model_states));
        model_climate_inputs * model_climate_inputs1 = malloc(sizeof(model_climate_inputs));
        model_climate_inputs * model_climate_inputs2 = malloc(sizeof(model_climate_inputs));
        model_precip_inputs * model_precip_inputs1 = malloc(sizeof(model_precip_inputs));

        /*
         * Assign value to model_states1
         */
        model_states1->z_s = model_states_1d1[0][i];
        model_states1->rho = model_states_1d1[1][i];
        model_states1->T_s = model_states_1d1[2][i];
        model_states1->T_s_0 = model_states_1d1[3][i];
        model_states1->T_s_l = model_states_1d1[4][i];
        model_states1->h2o_sat = model_states_1d1[5][i];
//        printf("The snow depth of current pixel is: %fm\n", model_states1->z_s);
//        printf("The snow density of current pixel is: %fkg per cubic meters\n", model_states1->rho);
        printf("The snow temperature of current pixel is: %f K\n", model_states1->T_s);

        /*
         * Assign value to model_climate_inputs1
         */
        model_climate_inputs1->S_n = model_climate_inputs_1d1[0][i];
        model_climate_inputs1->I_lw = model_climate_inputs_1d1[1][i];
        model_climate_inputs1->T_a = model_climate_inputs_1d1[2][i];
        model_climate_inputs1->e_a = model_climate_inputs_1d1[3][i];
        model_climate_inputs1->u = model_climate_inputs_1d1[4][i];
        model_climate_inputs1->T_g = model_climate_inputs_1d1[5][i];
        printf("The air temperature 1 of current pixel is: %f K\n", model_climate_inputs1->T_a);
        printf("The ground temperature from NLDAS is: %f K\n", model_climate_inputs1->T_g);


        /*
         * Assign value to model_climate_inputs2
         */
        model_climate_inputs2->S_n = model_climate_inputs_1d2[0][i];
        model_climate_inputs2->I_lw = model_climate_inputs_1d2[1][i];
        model_climate_inputs2->T_a = model_climate_inputs_1d2[2][i];
        model_climate_inputs2->e_a = model_climate_inputs_1d2[3][i];
        model_climate_inputs2->u = model_climate_inputs_1d2[4][i];
        model_climate_inputs2->T_g = model_climate_inputs_1d2[5][i];

        /*
         * Assign value to model_precip_inputs1
         */
        model_precip_inputs1->precip_now = (int) model_precip_inputs_1d1[0][i];
        model_precip_inputs1->m_pp = model_precip_inputs_1d1[1][i];
        model_precip_inputs1->percent_snow = model_precip_inputs_1d1[2][i];
        model_precip_inputs1->rho_snow = model_precip_inputs_1d1[3][i];
        model_precip_inputs1->T_pp = model_precip_inputs_1d1[4][i];

        /*
         * Test Please comment out when running isnobal
         */
        /*
//        states_results[0][i] = -99.;
//        states_results[1][i] = -98.;
//        states_results[2][i] = -97.;
//        states_results[3][i] = -96.;
//        states_results[4][i] = -95.;
//        states_results[5][i] = -94.;
//
//        if (i == 1) {
//            states_results[0][i] = -94.;
//            states_results[1][i] = -95.;
//            states_results[2][i] = -96.;
//            states_results[3][i] = -97.;
//            states_results[4][i] = -98.;
//            states_results[5][i] = -99.;
//        }
//
//        printf("elevation = %f\n", model_measure_params1->elevation);
//
//        printf("z_s = %f\n", model_states1->z_s);
//        printf("rho = %f\n", model_states1->rho);
//        printf("T_s = %f\n", model_states1->T_s);
//        printf("T_s_0 = %f\n", model_states1->T_s_0);
//        printf("T_s_l = %f\n", model_states1->T_s_l);
//        printf("h2o_sat = %f\n", model_states1->h2o_sat);
//
//        printf("S_n = %f\n", model_climate_inputs1->S_n);
//        printf("I_lw = %f\n", model_climate_inputs1->I_lw);
//        printf("T_a = %f\n", model_climate_inputs1->T_a);
//        printf("e_a = %f\n", model_climate_inputs1->e_a);
//        printf("u = %f\n", model_climate_inputs1->u);
//        printf("T_g = %f\n", model_climate_inputs1->T_g);
//
//        printf("S_n = %f\n", model_climate_inputs2->S_n);
//        printf("I_lw = %f\n", model_climate_inputs2->I_lw);
//        printf("T_a = %f\n", model_climate_inputs2->T_a);
//        printf("e_a = %f\n", model_climate_inputs2->e_a);
//        printf("u = %f\n", model_climate_inputs2->u);
//        printf("T_g = %f\n", model_climate_inputs2->T_g);
//
//        printf("precip_new = %d\n", model_precip_inputs1->precip_now);
//        printf("m_pp = %f\n", model_precip_inputs1->m_pp);
//        printf("percent_snow = %f\n", model_precip_inputs1->percent_snow);
//        printf("rho_snow = %f\n", model_precip_inputs1->rho_snow);
//        printf("T_pp = %f\n", model_precip_inputs1->T_pp);
//
//        printf("%f\n", states_results[0][i]);
//        printf("%f\n", states_results[1][i]);
//        printf("%f\n", states_results[2][i]);
//        printf("%f\n", states_results[3][i]);
//        printf("%f\n", states_results[4][i]);
//        printf("%f\n", states_results[5][i]);
        */

        /*
         * Please uncomment below when running
         */
        if (!snobal_init(model_params1, model_measure_params1,
                         model_states1, model_climate_inputs1,
                         model_climate_inputs2, model_precip_inputs1)) {
            printf("Initialization failed");
            states_results[0][i] = -99.;
            states_results[1][i] = -99.;
            states_results[2][i] = -99.;
            states_results[3][i] = -99.;
            states_results[4][i] = -99.;
            states_results[5][i] = -99.;
            continue;
        }

        init_snow();

        printf("snow temperature %f k \n", T_s);
        printf("Snow lower layer %f \n", T_s_l);
        printf("Snow upper layer %f \n", T_s_0);
        printf("Air temperature 1 %f \n", input_rec1.T_a);
        printf("Air temperature 2 %f \n", input_rec2.T_a);
        printf("The ground temperature %f \n", input_rec1.T_g);
        printf("Precipitation temperature %f \n", T_pp);


        if(!do_data_tstep()) {
            printf("Oops! Something happened when running snobal, please check log files.\n");
            states_results[0][i] = -99.;
            states_results[1][i] = -99.;
            states_results[2][i] = -99.;
            states_results[3][i] = -99.;
            states_results[4][i] = -99.;
            states_results[5][i] = -99.;
            continue;
        };

        states_results[0][i] = z_s;
        states_results[1][i] = rho;
        states_results[2][i] = T_s;
        states_results[3][i] = T_s_0;
        states_results[4][i] = T_s_l;
        states_results[5][i] = h2o_sat;

        /*
         * free temporal objects
         */
        free(model_states1);
        free(model_climate_inputs1);
        free(model_climate_inputs2);
        free(model_precip_inputs1);

        printf("Iteration done!");
    }

    return states_results;

}