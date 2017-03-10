synopsis='[path]'
description="%description"
 
PATH="$PATH:$IPW/lib"

while getopts "H" opt
    do
    case $opt in
	-H)	#  Standard convention for IPW commands: -H to display usage
		exec usage $0 "$synopsis" "$description"
		;;
	*)	exec usage $0 "$synopsis" "$description"
		;;
    esac
    done

#  Get the name of the directory
case $# in
    0)	dir="."
	;;
    1)	dir=$1
	;;
    *)	exec usage $0 "$synopsis" "$description"
	;;
esac

#  Create the directory if it doesn't exist
if [ -n "`ls $dir 2>/dev/null`" ] ; then
    if [ ! -d $dir ] ; then
        exec sherror $0 "$dir already exists, but is not a directory"
    fi
else
    mkdir -p $dir || exec sherror $0 "cannot make directory: $dir"
fi

#  Go to that directory
cd $dir

#  Loop through the list of directories to create.
#  For each directory, check if it exists as a directory already.
#  If it doesn't exist, then make it; otherwise, it's an error
for name in aux bin h lib man/man1 man/man3 man/man5 sbin src/bin src/lib \
		src/sbin tests www/man1 www/man3 www/man5
    do
    if [ -n "`ls $name 2>/dev/null`" ] ; then
        if [ ! -d $name ] ; then
            exec sherror $0 "$dir/$name already exists, but is not a directory"
        fi
    else
        mkdir -p $name || exec sherror $0 "cannot make directory: $name"
    fi
    done

#  Always exit with a successful status
exit 0
