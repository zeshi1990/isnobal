import ctypes

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




print dew_point_test(101325.0)
multi_result = multiple_vars_test(101325.0, 373.0)
print multi_result[0]
print multi_result[1]

struct_result = struct_test(101325.0, 373.0)
print struct_result.contents.dew_p
print struct_result.contents.sat_water

struct_io_result = struct_io_test(101325.0, 373.0)
print struct_io_result.contents.dew_p
print struct_io_result.contents.sat_water

_snobal.init()

# from ctypes import *
#
# global _snobal
# _snobal = CDLL('smake-build-debug/libSnobalIO.so')
