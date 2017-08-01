from ctypes import *
import numpy as np
from numpy.ctypeslib import ndpointer, as_ctypes, as_array

nd_double_2d = ndpointer(np.uintp, ndim=1, flags='C')


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
        ("z_0", c_double)
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


class model_measure_params_1d(Structure):
    """
    The model_measure_params struct in c
    """
    _fields_ = [
        ("relative_hts", c_int),
        ("z_g", c_double),
        ("z_u", c_double),
        ("z_T", c_double),
        ("z_0", c_double),
        ("i_elevation", POINTER(c_double))
    ]

def _construct_model_measure_params_1d(measure_params, i_elevation):
    """

    :param measure_params: relative_hts, z_g, z_u, z_T, z_0
    :param i_elevation:
    :return:
    """
    return model_measure_params_1d(int(measure_params[0]),
                                   float(measure_params[1]),
                                   float(measure_params[2]),
                                   float(measure_params[3]),
                                   float(measure_params[4]),
                                   as_ctypes(i_elevation.astype(np.float64).flatten()))

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

def _convert_2d(array):
    return (array.ctypes.data + np.arange(array.shape[0]) * array.strides[0]).astype(np.uintp)

def _parse_states(state_result):
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

def _read_isnobal_results(p_results, n_pixels):
    arr_results = np.zeros((6, n_pixels))
    for i in range(6):
        arr_results[i] = as_array(p_results[i], shape=(n_pixels, ))
    return arr_results


_snobal = CDLL('/Users/zeshizheng/Google Drive/Research/ISNOBAL/SnobalIO/cmake-build-debug/libSnobalIO.so')

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

    res_states = _parse_states(result)
    return res_states

_snobal.run_isnobal_1d.argtypes = (
    c_long,
    POINTER(model_params),
    POINTER(model_measure_params_1d),
    nd_double_2d,
    nd_double_2d,
    nd_double_2d,
    nd_double_2d
)

_snobal.run_isnobal_1d.restype = POINTER(POINTER(c_double))


def _run_isnobal_1d(params, measure_params, i_elevation, i_states, i_inputs1, i_inputs2, i_precips):
    global _snobal
    model_params_obj = _construct_model_params(params)
    model_measure_params_1d = _construct_model_measure_params_1d(measure_params, i_elevation)
    p_results = _snobal.run_isnobal_1d(c_long(i_states.shape[1]),
                                       byref(model_params_obj),
                                       byref(model_measure_params_1d),
                                       _convert_2d(i_states),
                                       _convert_2d(i_inputs1),
                                       _convert_2d(i_inputs2),
                                       _convert_2d(i_precips))
    arr_results = _read_isnobal_results(p_results, i_states.shape[1])
    return arr_results


def _compile_states_dict(states_dict):

    fields = ["z_s", "rho", "T_s", "T_s_0", "T_s_l", "h2o_sat"]

    assert all(k in states_dict for k in fields), \
        "User has missed one field in states!"

    # Assume the snowdepth pixel length is the standard by default
    pixel_length = states_dict["z_s"].shape[0]
    i_states = np.zeros((6, pixel_length))
    for i, k in enumerate(fields):
        assert len(states_dict[k].shape) == 1, \
            "Processing error, each attribute of states has to be 1D array"
        assert states_dict[k].shape[0] == pixel_length, \
            "Processing error, states length are not the same!"
        i_states[i] = states_dict[k]
    return pixel_length, i_states

def _compile_input_dict(input_dict, pixel_length, type='climate'):
    if type == 'climate':
        fields = ["S_n", "I_lw", "T_a", "e_a", "u", "T_g"]
    else:
        fields = ["precip_now", "m_pp", "percent_snow", "rho_snow", "T_pp"]

    assert all(k in input_dict for k in fields), \
        "User has missed some attribute(s) in input!"

    i_input = np.zeros((len(fields), pixel_length))
    for i, k in enumerate(fields):
        assert len(input_dict[k].shape) == 1, \
            "Processing error, each attribute of input has to be 1D array"
        assert input_dict[k].shape[0] == pixel_length, \
            "Processing error, input length are not the same!"
        i_input[i] = input_dict[k]
    return i_input


def run_isnobal_1d(**kwargs):
    # Assertion control on states
    assert "states" in kwargs, "User need to specify the initial states of the snowpack!"
    i_states = kwargs.get("states")
    pixel_length = i_states.shape[1]

    # Assertion control on input1
    assert "climate_inputs1" in kwargs, "User need to specify the climate inputs 1!"
    i_inputs1 = kwargs.get("climate_inputs1")

    # Assertion control on precip
    assert "precip_inputs" in kwargs, "User need to specify the precip inputs"
    i_precips = kwargs.get("precip_inputs")

    # Assertion control on DEM
    assert "elevations" in kwargs, "User need to specify the elevation of all locations"
    i_elevation = kwargs.get("elevations")
    assert len(i_elevation.shape) == 1, \
        "Processing error, elevation has to be 1D array"
    assert i_elevation.shape[0] == pixel_length, \
        "Processing error, elevation pixel number not consistent"

    # Assertion control on measure
    assert "measure_params" in kwargs, "User need to specify the measurement params"
    assert len(kwargs.get("measure_params")) == 5, \
        "Some attribute(s) in measurement params is missing"
    measure_params = kwargs.get("measure_params")

    if "params" in kwargs:
        assert len(kwargs.get("params")) == 7, "Please check the length of model_params"
        params = kwargs.get("params")
    else:
        params = None

    if "climate_input2" in kwargs:
        i_inputs2 = kwargs.get("climate_inputs2")
    else:
        i_inputs2 = np.copy(i_inputs1)

    result = _run_isnobal_1d(params=params,
                    measure_params=measure_params,
                    i_elevation=i_elevation,
                    i_states=i_states,
                    i_inputs1=i_inputs1,
                    i_inputs2=i_inputs2,
                    i_precips=i_precips)
    return result