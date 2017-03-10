#include	<unistd.h>

#include	"ipw.h"
#include 	"pgm.h"
#include        "envphys.h"


int
main(
	int	argc,
	char	**argv)
{
	static OPTION_T opt_z = {
		'z', "elevation (m) for calculation of pa",
		REAL_OPTARGS, "elev",
		OPTIONAL, 1, 1
	};

	static OPTION_T opt_p = {
		'p', "air pressure (Pa) (default: 1 atmos)",
		REAL_OPTARGS, "pressure",
		OPTIONAL, 1, 1
	};

	static OPTION_T opt_m = {
		'm', "output vapor pressure in mb (default: Pa)",
	};

	static OPTION_T opt_d = {
		'd', "also output dewpoint temp. in K",
	};

	static OPTION_T *optv[] = {
		&opt_z,
		&opt_p,
		&opt_m,
		&opt_d,
		0
	};

	int	n;
	int	mb;
	int	dp;

	double	airpress;
	double	d_point = 0.0;
	double	tdry;
	double	twet;
	double	vpress;
	double	z;


	ipwenter (argc, argv, optv, IPW_DESCRIPTION);

	/* see if more args */

        if (got_opt(opt_z)) {
                z = real_arg(opt_z, 0);
        }
        else {
                z = 0.0;
        }

        if (got_opt(opt_p)) {
                airpress = real_arg(opt_p, 0);
        }
        else {
		if (z == 0.0) {
			airpress = SEA_LEVEL;
		}
		else {
			airpress = HYSTAT (SEA_LEVEL, STD_AIRTMP, STD_LAPSE,
				   (z / 1000.0), GRAVITY, MOL_AIR);
		}
	}

	mb = got_opt(opt_m);
	dp = got_opt(opt_d);

	/*	check stdin for re-direct	*/

	if (isatty(STDIN_FILENO))
		fprintf(stderr,"Input wet/dry bulb pair;\n");

	/*	read input data and do calculations  	*/

	n = 0;

	while (scanf("%lf %lf", &twet, &tdry) == 2) {

		n++;

		/*	convert wet and dry temps to Kelvin	*/
		twet += FREEZE;
		tdry += FREEZE;

		/*	calculate vapor pressure	*/
		vpress = psychrom(tdry, twet, airpress);

		if(dp)
			d_point = dew_point(vpress);

		/*	output results to stdout	*/
		if(mb) {
			if(dp)
				printf("%6.2f %6.2f\n", 
						 vpress/100.0 ,d_point);
			else
				printf("%6.2f\n", vpress/100.0);
		}
		else {
			if(dp)
				printf("%6.2f %6.2f\n", vpress, d_point);
			else
				printf("%6.2f\n", vpress);
		}
	}

	if (n <= 0)
		error("bad or empty infile");
	
	ipwexit(EXIT_SUCCESS);
}
