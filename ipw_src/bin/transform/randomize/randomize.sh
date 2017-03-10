PATH="$PATH:$IPW/lib"

pgm=`basename $0`
optstring="s:i:"
synopsis='[-s seed] [image ...]'
description='%description'

set -- `getopt "$optstring" $* 2>/dev/null` || {
	exec usage $pgm "$synopsis" "$description"
}

seed=
image=

while :; do
	case $1 in
	--)	shift
		break
		;;
	-H)	exec usage $pgm "$synopsis" "$description"
		;;
	-s)	seed="-s $2"
		shift
		;;
	*)	exec sherror $pgm "'getopt' failed"
		;;
	esac
	shift
done

trap 'rm -f $TMPDIR/*$$' 0
trap 'exit 0' 1 2 3 15

case $# in
0)	cat > $TMPDIR/rmize$$
	image=$TMPDIR/rmize$$
	;;
1)	image=$1
	case $image in
	'-')	cat > $TMPDIR/rmize$$
		image=$TMPDIR/rmize$$
		;;
	*)	test -r $image || {
			exec sherror $pgm "can't open file" $image
		}
		;;
	esac
	;;
*)	rm -f $TMPDIR/rmize$$
	for image in $*
	do
		case $image in
		-)	cat >> $TMPDIR/rmize$$
			;;
		*)	test -r $image || {
				exec sherror $pgm "can't open file" $image
			}
			cat $image >> $TMPDIR/rmize$$
			;;
		esac
	done
	image=$TMPDIR/rmize$$
	;;
esac


random -r 0,1000000 $seed -n `cat $image | wc -l` > $TMPDIR/rnd$$

paste $TMPDIR/rnd$$ $image | sort -n | sed 's/^[0-9]*[ 	]*//'

rm -f $TMPDIR/*$$

exit 0
