import gdal
import ulmo
import numpy as np
import os
from matplotlib import pyplot as plt
from ..SnobalIO import snobalio

gdal.SetConfigOption('GRIB_NORMALIZE_UNITS', 'NO')

def get_NLDAS(spatial=False):
    coords = [40.031669, -121.191593]
    band_info = {}
    path = os.getcwd()
    for h in range(24):
        fn = os.path.join(path, "ISNOBAL/SnobalIO_test/NLDAS/NLDAS_FORA0125_H.A20170301." + str(h).zfill(2) + \
                          "00.002.grb")
        ds = gdal.Open(fn)
        ds_geo = ds.GetGeoTransform()
        pixel_idx = (int((coords[0] - ds_geo[3])/ds_geo[5]), int((coords[1] - ds_geo[0])/ds_geo[1]))
        if spatial:
        	pixel_idx_x, pixel_idx_y = np.meshgrid(np.arange(pixel_idx[0] - 1, pixel_idx[0] + 2), np.arange(pixel_idx[1] - 1, pixel_idx[1] + 2), indexing='ij')
        	pixel_idx = (pixel_idx_x.flatten(), pixel_idx_y.flatten())
        for i in range(1, ds.RasterCount + 1):
            band = ds.GetRasterBand(i)
            metadata = band.GetMetadata()
            array = band.ReadAsArray()
            if metadata["GRIB_ELEMENT"] not in band_info:
                band_info[metadata["GRIB_ELEMENT"]] = []
            band_info[metadata["GRIB_ELEMENT"]].append(array[pixel_idx])

    for key in band_info.iterkeys():
        band_info[key] = np.array(band_info[key])
    return band_info

def get_cdec():
    data_3lk = ulmo.cdec.historical.get_data(["3LK"], [3, 18], ["monthly"],
                                             start="2017-02-27",
                                             end="2017-03-02")
    snow_density = data_3lk["3LK"]["SNOW, WATER CONTENT monthly"]["value"].values / \
                   data_3lk["3LK"]["SNOW DEPTH monthly"]["value"].values
    hmb = ulmo.cdec.historical.get_data(["HMB"], [3], ["hourly"], start="2017-03-01", end="2017-03-01")
    swe = hmb["HMB"]["SNOW, WATER CONTENT hourly"]["value"].values
    snowdepth = swe / snow_density
    return snow_density, snowdepth

def calc_e_sat_air(T_a):
    return 611.2 * np.exp(17.67 * (T_a - 273.15) / (T_a - 29.65))

def calc_rh(P_a, spfh, T_a):
    return 0.263 * P_a * spfh * (1. / np.exp(17.67 * (T_a - 273.15) / (T_a - 29.65)))

def calc_e_air(P_a, spfh, T_a):
    return (calc_rh(P_a, spfh, T_a) / 100.0) * calc_e_sat_air(T_a)

def format_snobalData(band_info, snow_density, snowdepth):

    # climate input
    nldas_input_fields = ["DSWRF", "DLWRF", "TMP", "E_A", "U"]
    climate_inputs = []
    for field in nldas_input_fields:
        if field == "E_A":
            e_a = calc_e_air(band_info["PRES"], band_info["SPFH"], band_info["TMP"])
            climate_inputs.append(e_a)
        elif field == "U":
            u = np.sqrt(band_info["UGRD"]**2 + band_info["VGRD"]**2)
            climate_inputs.append(u)
        elif field == "DSWRF":
            climate_inputs.append(band_info[field] * 0.2)
        else:
            climate_inputs.append(band_info[field])

    climate_inputs.append(np.ones(climate_inputs[0].shape)*273.15)
    climate_inputs = np.array(climate_inputs).T

    # model params
    params = np.array([1, 0, 0.15, 0.01, 2400, 0., 0.])
    measure_params = np.array([0., 2000., 0.15, 10., 10.0, 0.4])
    precip_inputs = np.array([0, 0, 0, 0, 0])
    states = np.array([snowdepth, snow_density, 270.16, 270.16, 270.16, 0.0])
    return climate_inputs, states, params, measure_params, precip_inputs


def format_snobalData_1D(band_info, snow_density, snowdepth):
    # climate input
    nldas_input_fields = ["DSWRF", "DLWRF", "TMP", "E_A", "U"]
    climate_inputs = []
    for field in nldas_input_fields:
        if field == "E_A":
            e_a = calc_e_air(band_info["PRES"], band_info["SPFH"], band_info["TMP"])
            climate_inputs.append(e_a)
        elif field == "U":
            u = np.sqrt(band_info["UGRD"]**2 + band_info["VGRD"]**2)
            climate_inputs.append(u)
        elif field == "DSWRF":
            climate_inputs.append(band_info[field] * 0.2)
        else:
            climate_inputs.append(band_info[field])

    climate_inputs.append(np.ones(climate_inputs[0].shape)*273.15)
    climate_inputs = np.array(climate_inputs)

    # model params
    params = np.array([1, 0, 0.15, 0.01, 3600, 0., 0.])
    measure_params = np.array([0., 0.15, 10., 10.0, 0.4])
    elevations = np.ones(climate_inputs.shape[-1])*2000. + np.random.randint(low=-10, high=10, size=climate_inputs.shape[-1]).astype(float)

    precip_inputs = np.zeros((5, climate_inputs.shape[-1]))
    states = np.array([snowdepth, snow_density, 270.16, 270.16, 270.16, 0.0])[:, np.newaxis]
    states = np.tile(states, (1, climate_inputs.shape[-1]))
    states += np.random.randn(states.shape[0], states.shape[1]) * 0.1
    return climate_inputs, states, params, measure_params, elevations, precip_inputs


def run_model_1d():
	print "Load band info from NLDAS"
	band_info = get_NLDAS(spatial=True)
	snow_density, snowdepth = 400., 1.15

	print "Compile iSNOBAL input data"
	climate_inputs, states, params, measure_params, elevations, precip_inputs = \
		format_snobalData_1D(band_info, snow_density, snowdepth)

	print "Run model for 24 hours"
	for i in range(1):
		states = snobalio.run_isnobal_1d(states=states, climate_inputs1=climate_inputs[:, i, :],
										climate_inputs2=climate_inputs[:, i+1, :], precip_inputs=precip_inputs,
										params=params, measure_params=measure_params, elevations=elevations)
		print states[0, :]




def run_model_point():
	band_info = get_NLDAS()
	snow_density, snowdepth = 400., 1.15


	climate_inputs, states, params, measure_params, precip_inputs = \
	    format_snobalData(band_info, snow_density, snowdepth)

	climate_inputs = np.vstack((climate_inputs, climate_inputs))
	climate_inputs = np.vstack((climate_inputs, climate_inputs))

	sd_list = []
	sd_list.append(states[0])

	for i in range(90):
	    states = snobalio.run_snobal(states=states, climate_inputs1=climate_inputs[i],
	                                 climate_inputs2=climate_inputs[i+1], precip_inputs=precip_inputs,
	                                 params=params, measure_params=measure_params)
	    sd_list.append(states[0])

	print sd_list
	plt.plot(range(len(sd_list)), np.array(sd_list), '-k')
	plt.show()

run_model_1d()


