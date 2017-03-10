import numpy as np

# /*
#  * Environmental physics.
#  */
 
# /* ----------------------------------------------------------------------- */
 
# /*
#  *  Constants
#  */
 
# /*
#  *  molecular weight of air (kg / kmole)
#  */

# Molecular weight of air (kg/kmole)
MOL_AIR = 28.9644

# molecular weight of water vapor (kg/kmole)
MOL_H2O = 18.0153

# gas constant (J / kmole / deg)
RGAS = 8.31432e3

# Triple point of water at standard pressure (deg K)
FREEZE = 2.7316e2
BOIL = 3.7315e2


# specific heat of air at constant pressure (J / kg / deg)
CP_AIR = 1.005e3

# specific heat of water at 0C (J / (kg K))
CP_W0 = 4217.7

# density of water at 0C (kg/m^3)
# (from CRC handbook pg F-11)
RHO_W0 = 999.87

# density of ice - no air (kg/m^3)
# (from CRC handbook pg F-1)
RHO_ICE = 917.0

# thermal conductivity of wet sandy soil (J/(m sec K))
# (from Oke, 1978, pg. 38)
KT_WETSAND = 2.2

# thermal conductivity of dry sandy soil (J/(m sec K))
# (from Peixoto & Oort, 1992, pg. 221)
KT_DRYSAND = 0.3

# thermal conductivity of moist sandy soil (J/(m sec K))
# (estimated from C. Luces heat flux data at RMSP,
# by A. Winstral and D. Marks, NWRC)
KT_MOISTSAND = 1.65

# standard sea level pressure (Pa)
SEA_LEVEL = 1.013246e5

# standard sea level air temp (K)
STD_AIRTMP = 2.88e2

# standard lapse rate (K/m)
STD_LAPSE_M = -0.0065

# standard lapse rate (K/km)
STD_LAPSE = -6.5

# gravity acc
GRAVITY = 9.80665

# dry adiabatic lapse rate (deg / m)
DALR = GRAVITY / CP_AIR

# Earth equivalent spherical radius (km)
EARTH_RADIUS = 6.37122e3

# velocity of light, m/s (Marx, Nature, 296, 11, 1982)
LIGHT_SPEED = 2.99722458e8

# Von Karman constant
VON_KARMAN = 0.41

# /*
#  *  Macros
#  */

# /*
#  *  equation of state, to give density of a gas (kg/m^3) 
#  *
#  *	p = pressure (Pa)
#  *	m = molecular weight (kg/kmole)
#  *	t = temperature (K)	
#  *
#  *  or, inversely, to give pressure (Pa)
#  *
#  *      rho = density (kg/m^3)
#  *	m   = molecular weight (kg/kmole)
#  *	t   = temperature (K)	
#  */

def GAS_DEN(p, m, t):
	return ((p) * m / (RGAS) * t)

def EQ_STATE(rho, m, t):
	return ((rho) * (RGAS) * (t) / (m))


# /*
#  *  virtual temperature, i.e. the fictitious temperature that air must
#  *  have at the given pressure to have the same density as a water vapor
#  *  and air mixture at the same pressure with the given temperature and
#  *  vapor pressure.
#  *
#  *      t = temperature (K)
#  *      e = vapor pressure
#  *	P = pressure (e and P in same units),
def VIR_TEMP(t, e, P):
	return t / (1. - (1. - MOL_H2O / MOL_AIR) * (e / P))

# /*
#  *  inverse of VIR_TEMP
#  *
#  *      tv = virtual temperature (K)
#  *      e  = vapor pressure
#  *	P  = pressure (e and P in same units),
#  */
def INV_VIR_TEMP(tv, e, P):
	return ((tv)*(1.-(1.-MOL_H2O/MOL_AIR)*((e)/(P))))

# /*
#  *  potential temperature
#  *
#  *      t = temperature (K)
#  *	p = pressure (Pa)
#  */
def POT_TEMP(t,p):
	return ((t)*np.power(1.e5/(p),RGAS/(MOL_AIR*CP_AIR)))

# /*
#  *  inverse of POT_TEMP
#  *
#  *	theta = potential temperature (K)
#  *	p     = pressure (Pa)
#  */
def	INV_POT_TEMP(theta,p):
	return ((theta)/np.power(1.e5/(p),RGAS/(MOL_AIR*CP_AIR)))

# calories to J
def CAL_TO_J(c):
	return ((c) * 4.186798188)

# g to kg
def G_TO_KG(g):
	return 0.001 * g

# /*
#  *  specific heat of ice (J/(kg K))
#  *    (from CRC table D-159; most accurate from 0 to -10 C)
#  *
#  *	t = temperature (K)
#  */
def CP_ICE(t):
	return (CAL_TO_J(0.024928 + (0.00176*(t))) / G_TO_KG(1))

# /*
#  *  integral of hydrostatic equation over layer with linear temperature
#  *  variation
#  *
#  *	pb = base level pressure
#  *	tb = base level temp (K)
#  *	L  = lapse rate (deg/km)
#  *	h  = layer thickness (km)
#  *      g  = grav accel (m/s^2)
#  *	m  = molec wt (kg/kmole)
#  *
#  *	(the factors 1.e-3 and 1.e3 are for units conversion)
#  */
def HYSTAT(pb,tb,L,h,g,m):
	return ((pb) * np.exp(-(g)*(m)*(h)*1.e3/(RGAS*(tb))) if (((L)==0.) else \
		np.power((tb)/((tb)+(L)*(h)),(g)*(m)/(RGAS*(L)*1.e-3))))

# /*
#  *  inverse of integral of hydrostatic equation over layer with linear
#  *  temperature variation
#  *
#  *      pb = base level pressure
#  *	tb = base level temp (K)
#  *      hb = base level geopotential altitude (km)
#  *      p  = level pressure
#  *	t  = level temperature (K)
#  *      g  = grav accel (m/s^2)
#  *	m  = molec wt (kg/kmole)
#  *
#  *      (the factor 1.e-3 is for units conversion)
#  */
def INV_HYSTAT(pb,tb,hb,p,t,g,m):
	return ((hb)+1.e-3*log((p)/(pb))*(RGAS/((g)*(m)))*(-(t)) \
		if (((tb)==(t)) else \
			((t)-(tb))/log((tb)/(t))))

# /*
#  *  specific heat of water (J/(kg K))
#  *    (from CRC table D-158; most accurate from 0 to +10 C)
#  *    (incorrect at temperatures above 25 C)
#  *
#  *	t = temperature (K)
#  */
def CP_WATER(t):
	return (CP_W0 - 2.55*((t)-FREEZE))

# /*
#  *  specific humidity from vapor pressure
#  *
#  *	e = vapor pressure
#  *	P = pressure (same units as e)
#  */
def SPEC_HUM(e,P):
	return ((e)*MOL_H2O/(MOL_AIR*(P)+(e)*(MOL_H2O-MOL_AIR)))
# /*
#  *  vapor pressure from specific humidity
#  *
#  *	q = specific humidity
#  *	P = pressure
#  */
def INV_SPEC_HUM(q,P):
	return (-MOL_AIR*(P)*(q)/((MOL_H2O-MOL_AIR)*(q)-MOL_H2O))

# /*
#  *  mixing ratio
#  *
#  *	e = vapor pressure
#  *	P = pressure (same units as e)
#  */
def MIX_RATIO(e, P):
	return ((MOL_H2O/MOL_AIR)*(e)/((P)-(e)))

# /*
#  *  vapor pressure from mixing ratio
#  *
#  *	w = mixing ratio
#  *	P = pressure
#  */
def INV_MIX_RATIO(w,P):
	return ((w)*(P)/((w)+(MOL_H2O/MOL_AIR)))

# /*
#  *  vapor pressure from absolute humidity
#  *
#  *	ah = absolute humidity (gm/m^3)
#  *	ta = air temperature (K)
#  */
def AH2VP(ah,ta):
	return ((ah)*(RGAS/MOL_H2O)*(ta))

# /*
#  *  absolute humidity from vapor pressure
#  *
#  *	e = vapor pressure (Pa)
#  *	ta = air temperature (K)
#  */
def VP2AH(e,ta):
	return ((e)*(MOL_H2O/(RGAS*(ta))))

# /*
#  *  latent heat of vaporization
#  *
#  *	t = temperature (K)
#  */
def LH_VAP(t):
	return (2.5e6 - 2.95573e3 *((t) - FREEZE))

# /*
#  *  latent heat of fusion
#  *
#  *	t = temperature (K)
#  */
def LH_FUS(t):
	return (3.336e5 + 1.6667e2 * (FREEZE - (t)))


# /*
#  *  latent heat of sublimination (J/kg)
#  *    from the sum of latent heats of vaporization and fusion,
#  *
#  *	t = temperature (K)
#  */
def LH_SUB(t):
	return (LH_VAP(t) + LH_FUS(t))


# /*
#  *  effectuve diffusion coefficient (m^2/sec) for saturated porous layer
#  *  (like snow...).  See Anderson, 1976, pg. 32, eq. 3.13.
#  *
#  *	pa = air pressure (Pa)
#  *	ts = layer temperature (K)
#  */
def DIFFUS(pa,ts):
	return ((0.65*(SEA_LEVEL/(pa)) * np.power(((ts)/FREEZE),14.0)) * (0.01*0.01))

# /*
#  *  water vapor flux (kg/(m^2 sec)) between two layers
#  *
#  *	air_d = air density (kg/m^3)
#  *	k     = diffusion coef. (m^2/sec)
#  *	q_dif = specific hum. diff between layers (kg/kg)
#  *	z_dif = absolute distance between layers (m)
#  *
#  *	note:   q_dif controls the sign of the computed flux
#  */
def EVAP(air_d,k,q_dif,z_dif):
	return (air_d * k * (q_dif/z_dif))

# /*
#  *  dry static energy (J/kg)
#  *    (pg. 332, "Dynamic Meteorology", Holton, J.R., 1979)
#  *
#  *	t = air temperature (K)
#  *	z = elevation (m)
#  */
def DSE(t,z):
	return ( (CP_AIR * (t) ) + (GRAVITY * (z) ) )

# /*
#  *  inverse of DSE: air temperature (K) from dry static energy
#  *    (pg. 332, "Dynamic Meteorology", Holton, J.R., 1979)
#  *
#  *	dse = dry static energy (J/kg)
#  *	z   = elevation (m)
#  */

def INV_DSE(dse,z):
	return ((dse) - (GRAVITY * (z)) / CP_AIR )

# /*
#  *  moist static energy (J/kg)
#  *    (pg. 332, "Dynamic Meteorology", Holton, J.R., 1979)
#  *
#  *	z = elevation (m)
#  *	t = air temperature (K)
#  *	w = mixing ratio
#  */
def MSE(z,t,w):
	return ( DSE((t),(z)) + LH_VAP(t) * (w) )

# /*
#  *  inverse of MSE: vapor press (Pa) from moist static energy
#  *    (pg. 332, "Dynamic Meteorology", Holton, J.R., 1979)
#  *
#  *	z   = elevation (m)
#  *	t   = air temperature (K)
#  *	mse = moist static energy (J/kg)
#  */
def INV_MSE(z,t,mse):
	return (((mse) - DSE((t),(z))) / LH_VAP(t))

