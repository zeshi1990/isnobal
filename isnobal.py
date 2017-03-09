import threading
import numpy as np
import gdal
from scipy.optimize import fsolve


class isnobal:
	def __init__(self, max_sd, z_u, z_T, z_g, z, z_0, z_s, rho, T_s_0, T_s, w_c):
		"""
		max_sd: thickness of the active (surface) snowcover layer (m)
		z_u:	height above the ground of the wind speed estimate (m)
		z_T:	height above the ground of the air temperature and vapor pressure estimate (m)
		z_g:	depth of soil temperature estimate (m)
		z:		elevation (m)
		z_0: 	roughness length (m)
		Aboves can be scalars, we could assume they are constant the across the entire 
		spatial extent.
		z_s:	snowdepth (m)
		rho: 	average snow density (kg m^(-3))
		T_s_0:	active snow layer temperature
		T_s: 	average snowcover temperature
		w_c:	percentage of liquid H2O saturation (ratio)
		Aboves are the initial conditions of the states variables, should be the length of
		pixels we have
		"""
		self.max_sd = max_sd
		self.z_u = z_u
		self.z_T = z_T
		self.z_q = z_T
		self.z_g = z_g
		self.z = z
		self.z_0 = z_0
		self.d_0 = 2./3. * 7.35 * self.z_0
		self.z_s = z_s
		# self.z_s_1 = 0.5 * self.z_s
		# self.z_s_0 = 0.5 * self.z_s
		self.rho = rho
		self.T_s_0 = T_s_0
		# self.T_s_1 = T_s_0
		self.T_s = T_s
		self.w_c = w_c

	def run(self, states, dt):
		"""
		The first step is to load precipitation data from the model and convert the 
		ideally precipitation from PRISM and use.
		Note: All the gdal functions could be changed when using the data are already
		formated in numpy or pandas objects.
		"""
		# initialize global variables:
		global m_pp, T_pp, psnow_pp, rho_pp, I_lw, T_a, e_a, u, T_g, S_n, q, q_s_0, HEAT_FLUX, R_n

		# Precipitation input and estimate
		m_pp = gdal.Open().ReadAsArray().flatten()			# PRISM ppt
		T_pp = gdal.Open().ReadAsArray().flatten()			# PRISM Tmin
		psnow_pp = self.__precip_snow_percentage(T_pp) 		# Estimate precipitation snow percentage
		rho_pp = self.__precip_snow_density(T_pp) 			# Estimate precipitation snow density

		# Input energy forcing data
		I_lw = gdal.Open().ReadAsArray().flatten()			# longwave radiation
		T_a = gdal.Open().ReadAsArray().flatten()			# Air temperature, PRISM Tmean
		e_a = gdal.Open().ReadAsArray().flatten()			# vapor pressure
		u = gdal.Open().ReadAsArray().flatten()				# wind speed
		T_g = gdal.Open().ReadAsArray().flatten()			# soil temperature
		S_n = gdal.Open().ReadAsArray().flatten()			# Net solar radiation

		# Calculate specific humidity in air and on snow layer 0, snow layer 1 and soil
		q, q_s_0, q_s_1, q_g = self.__specific_humidity(T_a, T_g, e_a)		

		# When calculating L, u_star, H, E, never initialize them with zero.
		# It will cause zero division error
		#################################### reimplement this #################################
		# Some thoughts: use threading and declare global variables for 
		HEAT_FLUX_PRIOR = np.ones((len(m_pp), 4))
		HEAT_FLUX = np.zeros((len(m_pp), 4))
		nThreads = 24
		i = 0
		while i < len(m_pp):
			thread_list = []
			for j in range(nThreads):
				t = threading.Thread(target=self.__fsolve_wrapper, args=(HEAT_FLUX_PRIOR[i], i, HEAT_FLUX))
				thread_list.append(t)
				t.start()
				i += 1
				if i == len(m_pp):
					break

			for t in thread_list:
				t.join()
		#################################### reimplement this #################################
		
		# Calculating shortwave solar radiation
		R_n = S_n + I_lw - 0.99 * 5.6697 * 10 ** (-8) * self.T_s_0 ** 4

		# Heat conduction from soil
			
		K_g = 	# Davis (1980)
		K_s_0 = # Yen (1965)
		K_s_1 = # Yen (1965)
		D_e_g = # Anderson (1976)
		D_e_0 = # Anderson (1976)
		D_e_1 = # Anderson (1976)

		K_es_0 = K_s_0 + L_v * D_e_0 * q_s_0
		K_es_1 = K_s_1 + L_v * D_e_1 * q_s_1
		K_eg = K_g + L_v * D_e_g * q_g

		# Calculating ground to layer 1
		G = 2 * K_es_1 * K_eg * (T_g - self.T_s_1) / (K_eg * z_s_1 + K_es_1 * z_g)
		# Calculting layer 1 to layer 0
		G_0 = 2 * K_es_0 * K_es_1 * (T_s_1 - T_s_0) / (K_es_1 * z_s_0 + K_es_0 * z_s_1)

		# Calculating rain over snow
		C_pp = __specific_heat_precip()
		M = C_pp * rho_pp * m_pp * (T_pp - T_s_0) / self.t_step

		# Calculating net energy transfer
		delta_Q_0 = R_n + H + L_v * E + M + G_0
		delta_Q = delta_Q_0 + G

		








	def __specific_heat_precip(self):
		pass

	def __specific_humidity(self, T_a, T_g, e_a):
		"""
		need implementation
		"""
		humidity_air = 0.
		humidity_s_0 = 0.
		humidity_s_1 = 0.
		humidity_g = 0.
		return humidity_air, humidity_s_0, humidity_s_1, humidity_g


	def __fsolve_wrapper(self, input_data, spatial_idx):
		self.HEAT_FLUX[spatial_idx] = fsolve(self.__latent_sensible_heat_equations, 
			input_data, args=spatial_idx)


	def __latent_sensible_heat_equations(self, input_data, spatial_idx):
		rho_air = 1.225 									# kg/m^3
		k = 0.4 											# von Karman constant
		g = 9.80616											# gravity constant
		C_p = 1005. 										# J kg^-1 K^-1
		a_H = 1.
		a_E = 1.

		L, u_star, H, E = input_data
		phi_sm, phi_sh, phi_sv = self.__stability_function(L)

		eq_1 = L - u_star**3 * rho_air / (k * g * (H / (T_a[spatial_idx] * C_p) + 0.61 * E))
		eq_2 = u_star - u[spatial_idx] * k / (np.log((self.z_u - self.d_0) / self.z_0) - phi_sm)
		eq_3 = (H - (T_a[spatial_idx] - self.T_s_0[spatial_idx] * a_H * k * u_star * rho_air * C_p) / 
			(np.log((self.z_T - self.d_0) / self.z_0) - phi_sh))
		eq_4 = (E - (q[spatial_idx] - q_s_0[spatial_idx]) * a_E * k * u_star * rho_air / 
			(np.log((self.z_q - self.d_0) / self.z_0) - phi_sv))

		return eq_1, eq_2, eq_3, eq_4


	def __stability_function(self, L):
		si_u = self.z_u / L
		si_T = self.z_T / L
		si_q = self.z_q / L
		if si_u > 0:
			phi_sm = -5.
		else:
			x = (1. - 16. * si_u) ** (0.25)
			phi_sm = (2. * np.log((1.+x)/2.) + np.log((1.+x**2)/2.) - 
				2. * np.arctan(x) + np.pi / 2.)
		
		if si_T > 0:
			phi_sh = -5.
		else:
			x = (1. - 16. * si_T) ** (0.25)
			phi_sh = 2. * np.log((1. + x**2) / 2.)

		if si_q > 0:
			phi_sv = -5.
		else:
			x = (1. - 16. * si_q) ** (0.25)
			phi_sv = 2. * np.log((1. + x**2) / 2.)

		return phi_sm, phi_sh, phi_sv


	def __precip_snow_percentage(self, T_pp):
		psnow_pp = np.zeros(T_pp.shape)
		psnow_pp[T_pp < -0.5] = 1.
		psnow_pp[np.logical_and(T_pp >= -0.5, T_pp < 0.5)] = \
			0.5 - T_pp[np.logical_and(T_pp >= -0.5, T_pp < 0.5)]
		psnow_pp[T_pp >= 0.5] = 0.
		return psnow_pp
		

	def __precip_snow_density(self, T_pp):
		rho_pp = np.zeros(T_pp.shape)
		rho_pp[T_pp < -5.] = 75.
		rho_pp[np.logical_and(T_pp >= -5., T_pp < -3.)] = 100.
		rho_pp[np.logical_and(T_pp >= -3., T_pp < -1.5)] = 150.
		rho_pp[np.logical_and(T_pp >= -1.5, T_pp < -0.5)] = 175.
		rho_pp[np.logical_and(T_pp >= -0.5, T_pp < 0.)] = 200.
		rho_pp[np.logical_and(T_pp >= 0., T_pp < 0.5)] = 250.
		return rho_pp
