import ctypes
from ctypes import *
import numpy as np
from numpy.ctypeslib import ndpointer, as_ctypes, as_array

nd_double_1d = ndpointer(np.double, ndim=1, flags='C')
nd_double_2d = ndpointer(np.uintp, ndim=1, flags='C')

_snobal = ctypes.CDLL('cmake-build-debug/libSnobalIO.so')
_snobal.dew_point_test.argtype = ctypes.c_double
_snobal.dew_point_test.restype = ctypes.c_double

def dew_point_test(val):
    global _snobal
    result = _snobal.dew_point_test(ctypes.c_double(val))
    return result

_snobal.multiple_vars_test.argtypes = (ctypes.c_double, ctypes.c_double)
_snobal.multiple_vars_test.restype = ctypes.POINTER(ctypes.c_double)

def multiple_vars_test(e_val, tk_val):
    global _snobal
    result = _snobal.multiple_vars_test(ctypes.c_double(e_val), ctypes.c_double(tk_val))
    return result

class foo_struct(ctypes.Structure):
    _fields_ = [("dew_p", ctypes.c_double), ("sat_water", ctypes.c_double)]

class foo_struct_input(ctypes.Structure):
    _fields_ = [("e", ctypes.c_double), ("tk", ctypes.c_double)]

_snobal.struct_test.argtypes = (ctypes.c_double, ctypes.c_double)
_snobal.struct_test.restype = ctypes.POINTER(foo_struct)

def struct_test(e_val, tk_val):
    global _snobal
    result = _snobal.struct_test(ctypes.c_double(e_val), ctypes.c_double(tk_val))
    return result

_snobal.struct_io_test.argtype = ctypes.POINTER(foo_struct_input)
_snobal.struct_io_test.restype = ctypes.POINTER(foo_struct)

def struct_io_test(e_val, tk_val):
    global _snobal
    struct_input = foo_struct_input(e_val, tk_val)
    result = _snobal.struct_io_test(ctypes.byref(struct_input))
    return result

_snobal.pointer_1darray_test.argtypes = (ctypes.c_long, np.ctypeslib.ndpointer(np.float64, ndim=1, flags='C'))
_snobal.pointer_1darray_test.restype = None

def pointer_1darray_test(array):
    global _snobal
    _snobal.pointer_1darray_test(ctypes.c_long(array.shape[0]), array)

_snobal.pointer_2darray_test.argtypes = (ctypes.c_long,
                                         ctypes.c_long,
                                         np.ctypeslib.ndpointer(np.uintp,
                                                                ndim=1,
                                                                flags='C'),
                                         np.ctypeslib.ndpointer(np.uintp,
                                                                ndim=1,
                                                                flags='C'))
_snobal.pointer_2darray_test.restype = None

def convert_2d(array):
    return (array.ctypes.data + np.arange(array.shape[0]) * array.strides[0]).astype(np.uintp)

def pointer_2darray_test(array1, array2):
    global _snobal
    _snobal.pointer_2darray_test(ctypes.c_long(array1.shape[0]), ctypes.c_long(array1.shape[1]),
                                 convert_2d(array1), convert_2d(array2))


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

_snobal.pointer_struct_1darray_test.argtypes = (ctypes.c_long, POINTER(model_measure_params_1d))

def pointer_struct_1darray_test(measure_params, i_elevation):
    global _snobal
    model_measure_params_obj = _construct_model_measure_params_1d(measure_params, i_elevation)
    result = _snobal.pointer_struct_1darray_test(c_long(i_elevation.shape[0]),
                                                 byref(model_measure_params_obj))
    return result


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


def read_isnobal_results(p_results, n_pixels):
    arr_results = np.zeros((6, n_pixels))
    for i in range(6):
        arr_results[i] = as_array(p_results[i], shape=(n_pixels, ))
    return arr_results

def run_isnobal_1d(params, measure_params, i_elevation, i_states, i_inputs1, i_inputs2, i_precips):
    global _snobal
    model_params_obj = _construct_model_params(params)
    model_measure_params_1d = _construct_model_measure_params_1d(measure_params, i_elevation)
    p_results = _snobal.run_isnobal_1d(c_long(i_states.shape[1]),
                                     byref(model_params_obj),
                                     byref(model_measure_params_1d),
                                     convert_2d(i_states),
                                     convert_2d(i_inputs1),
                                     convert_2d(i_inputs2),
                                     convert_2d(i_precips))
    arr_results = read_isnobal_results(p_results, i_states.shape[1])
    return arr_results


# print dew_point_test(101325.0)
# multi_result = multiple_vars_test(101325.0, 373.0)
# print multi_result[0]
# print multi_result[1]
#
# struct_result = struct_test(101325.0, 373.0)
# print struct_result.contents.dew_p
# print struct_result.contents.sat_water
#
# struct_io_result = struct_io_test(101325.0, 373.0)
# print struct_io_result.contents.dew_p
# print struct_io_result.contents.sat_water
# print type(struct_io_result)
# print isinstance(struct_io_result, ctypes.POINTER(foo_struct))
#
# a = np.array([[1,2,3],[4,5,6],[7,8,9]])
# b = np.transpose(np.array([[2,3,4],[3,4,5],[5,6,7]]))
#
# pointer_1darray_test(np.array([[1,2,3],[4,5,6],[7,8,9]]).astype(np.float64).flatten())
# pointer_2darray_test(np.matrix(a).astype(np.float64),
#                      np.matrix(b).astype(np.float64))
#
# measure_params = np.array([1, 0.25, 10, 10, 0.1])
# i_elevation = np.array([10., 12., 15., 20., 25., 10.])
#
# pointer_struct_1darray_test(measure_params, i_elevation)
#
# print b

params = np.array([1,2,3,4,5,6,7])
measure_params = np.array([1, 0.25, 10, 10, 0.1])
i_elevation = np.array([2000., 3000.])

i_states = np.random.randn(6, 5)
i_inputs1 = np.random.randn(6, 5)
i_inputs2 = np.random.randn(6, 5)
i_precips = np.random.randn(5, 5)
i_precips[0, 0] = 0
i_precips[0, 1] = 1

print "States\n", i_states
print "Inputs1\n", i_inputs1
print "Inputs2\n", i_inputs2
print "Precips\n", i_precips

results = run_isnobal_1d(params, measure_params, i_elevation,
                         i_states, i_inputs1,
                         i_inputs2, i_precips)

print results
