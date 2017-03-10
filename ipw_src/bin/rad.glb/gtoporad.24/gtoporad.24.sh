PATH="$PATH:$IPW/lib"

pgm=`basename $0`
optstring='nz:w:g:s:x:d:i:o:m:'
synopsis='[-n] -z elev -w omega -g gfact [-s S0] [-x w1,w2] \
          -d y,m,d[,h,m,s] -i delta_t -o out_prefix [-m mask] [image]'
description='%description'

set -- `getopt "$optstring" $* 2>/dev/null` || {
	exec usage $pgm "$synopsis" "$description"
}

elev=
omega=
gfact=
S0=
wrange=
year=
month=
day=
hour=
min=
sec=
date=
delta_t=
mask=
net=
out_prefix=

# parse options, if none print description

case $# in
0|1)	exec usage $pgm "$synopsis" "$description"
	;;
esac

while :; do
	case $1 in
	--)	shift
		break
		;;
	-H)	exec usage $pgm "$synopsis" "$description"
		;;
	-n)	net="-n"
		;;
	-z)	elev=$2
		shift
		;;
	-w)	omega=$2
		shift
		;;
	-g)	gfact=$2
		shift
		;;
	-s)	S0=$2
		shift
		;;
	-x)	wrange=$2
		shift
		;;
	-d)	date=$2
		shift
		;;
	-i)	delta_t=$2
		shift
		;;
	-o)	out_prefix=$2
		shift
		;;
	-m)	mask=$2
		shift
		;;
	*)	exec sherror $pgm '"getopt" failed'
		;;
	esac
	shift
done

tdir=$TMPDIR/$pgm.$$

trap 'rm -f -r $tdir' 0
trap 'exit 0' 1 2 3 15

mkdir $tdir

# list of temporary files used (i.. image; a.. ascii)
# directory for all is $TMPDIR/$pgm.$$
#
# iX - copy of input if stdin
# iZ - elevation image (1 band)
# iGR - gradient image (2 bands)
# iVF - view factor/albedo image (3 bands)
# iR0 - reflectance of substrate (1 band)
# iOD - optical depth image (1 band)
#
# the following files are re-created for each time step
#
# az - solar azimuth/zenith image output from gsunlight
# shade - cosine illumination angle, corrected for horizons
# hor - horizon mask
# erad - beam and diffuse radiation over elevation grid

# Can only have 1 input image.  If stdin must make duplicate copy
# because we need multiple access.

img=$1
case $# in
0)	test -t 0 && {
	exec sherror $pgm "can't read image data from terminal"
	}
	;;
1)	;;
*)	exec usage $pgm "$synopsis" "$description"
	;;
esac

case $img in
''|'-')
	cat $img > $tdir/iX
	image=$tdir/iX
	;;
*)	test -r $img || {
		exec sherror $pgm "can't open file", $image
	}
	image=$img
	;;
esac

# parse date into year, month, day, hour, min, sec

case $date in
'')	exec usage $pgm "$synopsis" "$description"
	;;
*)	year=` echo $date | awk -F, '{print $1}'`
	month=`echo $date | awk -F, '{print $2}'`
	day=`  echo $date | awk -F, '{print $3}'`
	hour=` echo $date | awk -F, '{print $4}'`
	min=`  echo $date | awk -F, '{print $5}'`
	sec=`  echo $date | awk -F, '{print $6}'`
	;;
esac

echo "year = $year, month = $month, day = $day, hour = $hour, min = $min, sec = $sec"
echo "delta_t = $delta_t"

# make sure all essential arguments in

case $elev in
'')	exec sherror $pgm "-z arg missing"
	;;
esac

case $omega in
'')	exec sherror $pgm "-w arg missing"
	;;
esac

case $gfact in
'')	exec sherror $pgm "-g arg missing"
	;;
esac

case $S0 in
'')	case $wrange in
	'')	exec sherror $pgm "-s and -x args missing, one must be present"
		;;
	*)	S0=`solar -d "$year,$month,$day" -w $wrange -a`
		;;
	esac
esac

case $year in
'')	exec sherror $pgm "-d arg missing or incomplete"
	;;
esac

case $month in
'')	exec sherror $pgm "-d arg missing or incomplete"
	;;
esac

case $day in
'')	exec sherror $pgm "-d arg missing or incomplete"
	;;
esac

case $hour in
'')	hour=0
	;;
esac

case $min in
'')	min=0
	;;
esac

case $sec in
'')	sec=0
	;;
esac

case $delta_t in
'')	exec sherror $pgm "-i arg missing or incomplete"
	;;
esac

case $out_prefix in
'')	exec sherror $pgm "-o arg missing"
	;;
esac

case $mask in
'')	maskf=
	;;
*)	test -r $mask || {
		exec sherror $pgm "can't open file", $mask
	}
	maskf="-m $mask"
esac


# elevation file
demux -b 0 $image > $tdir/iZ

# slope/aspect file
demux -b 1,2 $image > $tdir/iGR

# sky view, terrain view, and albedo
demux -b 3,4,5 $image > $tdir/iVF

# reflectance of substrate
demux -b 6 $image > $tdir/iR0

# optical depth
demux -b 7 $image > $tdir/iOD

# sun angles file (overwritten each time step)
azfile=$tdir/az
mufile=$tdir/mu

# horizon output file (overwritten each time step)
hfile=$tdir/hor

# shade output file (overwritten each time step)
sfile=$tdir/shade


j=0
currdate="$year,$month,$day,$hour,$min,$sec"
currtime=`addtime -d $currdate -h 0 -c`
endtime=`addtime -d $currdate -h 24 -c`

# Loop on times

while [ $currtime -lt $endtime ]
do

#   Extension for output files

	ext=`echo $j | awk '{printf "%03d", $1}'`
	tfile=$out_prefix.$ext
	rfile=$tdir/erad

# be slightly verbose

	echo -n "date: $currdate, file: $ext -- "
	date

#   Get sun angles for all pixels for this time

	gsunlight -t $currdate $maskf $tdir/iZ > $azfile
	demux -b 0 $azfile > $mufile

#   Run gelevrad to get beam & diffuse

	mux $tdir/iZ $mufile $tdir/iR0 $tdir/iOD | \
		gelevrad -n 8,8 -z $elev -w $omega -g $gfact -s $S0 $maskf \
			> $rfile

#   Run ghorizon to get sun-below-horizon mask

	ghorizon -b -s $azfile $maskf $tdir/iZ > $hfile

#   Run shade to get cossine local illumination angle; mask by horizon mask

	gshade -s $azfile -i $tdir/iGR $maskf | \
		mux - $hfile | \
		bitcom -m -a > $sfile

#   Form input file and run gtoporad

	mux $mufile $rfile $sfile $tdir/iVF $tdir/iR0 | \
		gtoporad -b 8 $net > $tfile

#   Increment time and index

	currdate=`addtime -d $currdate -h $delta_t`
	currtime=`addtime -d $currdate -h 0 -c`
	j=`expr $j + 1`

done

# remove temporary files

rm -rf $tdir

exit 0
