from ctypes import *
import numpy as np

class model_params(Structure):
    """
    The model_params struct in c
    """
    _fields_ = [
        ("run_no_snow", c_int),
        ("stop_no_snow", c_int),
        ("max_z_s_0", c_double),
        ("max_h2o_vol", c_double),
        ("time_step", c_double),
        ("current_time", c_double),
        ("time_since_out", c_double)
    ]

def _construct_model_params(params):
    """
    A constructor of model_params object
    :param params: Could be a list/1-D array of the values specified in model_params
    :return: an instance of object model_params
    """
    return model_params(int(params[0]),
                        int(params[1]),
                        float(params[2]),
                        float(params[3]),
                        float(params[4]),
                        float(params[5]),
                        float(params[6]))

class model_states(Structure):
    """
    The model_states struct in c
    """
    _fields_ = [
        ("z_s", c_double),
        ("rho", c_double),
        ("T_s", c_double),
        ("T_s_0", c_double),
        ("T_s_l", c_double),
        ("h2o_sat", c_double)
    ]

def _construct_model_states(states):
    """
    A constructor of model_states object
    :param states: Could be a list/1-D array of the values specified in model_states
    :return: an instance of object model_states
    """
    return model_states(float(states[0]),
                        float(states[1]),
                        float(states[2]),
                        float(states[3]),
                        float(states[4]),
                        float(states[5]))

class model_climate_inputs(Structure):
    """
    The model_climate_inputs struct in c
    """
    _fields_ = [
        ("S_n", c_double),
        ("I_lw", c_double),
        ("T_a", c_double),
        ("e_a", c_double),
        ("u", c_double),
        ("T_g", c_double)
    ]

def _construct_model_climate_inputs(climate_inputs):
    """
    A constructor of model_climate_inputs object
    :param climate_inputs: Could be a list/1-D array of the values specified in
    model_climate_inputs.

    :return: an instance of object model_climate_inputs
    """
    return model_climate_inputs(float(climate_inputs[0]),
                                float(climate_inputs[1]),
                                float(climate_inputs[2]),
                                float(climate_inputs[3]),
                                float(climate_inputs[4]),
                                float(climate_inputs[5]))

class model_measure_params(Structure):
    """
    The model_measure_params struct in c
    """
    _fields_ = [
        ("relative_hts", c_int),
        ("elevation", c_double),
        ("z_g", c_double),
        ("z_u", c_double),
        ("z_T", c_double),
        ("T_g", c_double)
    ]

def _construct_model_measure_params(measure_params):
    """
    A constructor of model_measure_params object
    :param measure_params: Could be a list/1-D array of the values specified in
    model_measure_params.

    :return: an instance of object model_measure_params
    """
    return model_measure_params(int(measure_params[0]),
                                float(measure_params[1]),
                                float(measure_params[2]),
                                float(measure_params[3]),
                                float(measure_params[4]),
                                float(measure_params[5]))

class model_precip_inputs(Structure):
    """
    The model_precip_inputs struct in c
    """
    _fields_ = [
        ("precip_now", c_int),
        ("m_pp", c_double),
        ("percent_snow", c_double),
        ("rho_snow", c_double),
        ("T_pp", c_double)
    ]

def _construct_model_precip_inputs(precip_inputs):
    """
    A constructor of model_precip_inputs object
    :param precip_inputs: Could be a list/1-D array of the values specified in
    model_precip_inputs

    :return: an instance of object model_precip_inputs
    """
    return model_precip_inputs(int(precip_inputs[0]),
                               float(precip_inputs[1]),
                               float(precip_inputs[2]),
                               float(precip_inputs[3]),
                               float(precip_inputs[4]))

def parse_states(state_result):
    try:
        result = np.array([state_result.contents.z_s,
                           state_result.contents.rho,
                           state_result.contents.T_s,
                           state_result.contents.T_s_0,
                           state_result.contents.T_s_l,
                           state_result.contents.h2o_sat])
        return result
    except ValueError:
        print "The model returned NULL pointer, check input values."
        return None


_snobal = CDLL('cmake-build-debug/libSnobalIO.so')

_snobal.run_snobal.argtypes = (POINTER(model_params),
                               POINTER(model_measure_params),
                               POINTER(model_states),
                               POINTER(model_climate_inputs),
                               POINTER(model_climate_inputs),
                               POINTER(model_precip_inputs))

_snobal.run_snobal.restype = POINTER(model_states)

def run_snobal(**kwargs):
    """
    Running the snobal model through this wrapper
    :param kwargs:
    states: list/1-D array, length = 6
    climate_inputs1: list/1-D array
    params = None


    :return:
    """
    # Assertion of correctness of states kwarg
    assert "states" in kwargs, "User must specify the initial states of the snowpack!"
    assert len(kwargs.get("states")) == 6, "Please check the length of states!"

    # Assertion of correctness of climate_inputs1 kwarg
    assert "climate_inputs1" in kwargs, ("User must specify the climate condition " +
                                         "at the start of the time step!")
    assert len(kwargs.get("climate_inputs1")) == 6, "Please check the length of climate_inputs1"

    # Assertion of correctness of precip_inputs kwarg
    assert "precip_inputs" in kwargs, "User must specify the precipitation condition!"
    assert len(kwargs.get("precip_inputs")) == 5, "Please check the length of precip_inputs"

    if "params" in kwargs:
        assert len(kwargs.get("params")) == 7, "Please check the length of model_params"
        params = byref(_construct_model_params(kwargs.get("params")))
    else:
        params = None

    if "measure_params" in kwargs:
        assert len(kwargs.get("measure_params")) == 6, "Please check the length of measure_params"
        measure_params = byref(_construct_model_measure_params(kwargs.get("measure_params")))
    else:
        measure_params = None


    states = byref(_construct_model_states(kwargs.get("states")))
    climate_inputs1 = byref(_construct_model_climate_inputs(kwargs.get("climate_inputs1")))
    precip_inputs = byref(_construct_model_precip_inputs(kwargs.get("precip_inputs")))

    if "climate_inputs2" in kwargs:
        assert len(kwargs.get("climate_inputs2")) == 6, "Please check the length of climate_inputs2"
        climate_inputs2 = byref(_construct_model_climate_inputs(kwargs.get("climate_inputs2")))
    else:
        climate_inputs2 = None

    global _snobal

    result = _snobal.run_snobal(params,
                                measure_params,
                                states,
                                climate_inputs1,
                                climate_inputs2,
                                precip_inputs)

    res_states = parse_states(result)
    return res_states