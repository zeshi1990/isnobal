pgm=%name
synopsis='[ user | system ]'
description='%description'

cmdList=$IPW/src/bin/Commands.txt
sysCmdList=$IPW/src/sbin/Commands.txt

if [ \! -r $cmdList ] ; then
	printf "ERROR:  The file with the list of user commands is missing.\n"
	printf "        Notify the IPW system administrator.\n"
	printf "        (file: $cmdList)\n"
	exit 1
fi

if [ \! -r $sysCmdList ] ; then
	printf "ERROR:  The file with the list of system commands is missing.\n"
	printf "        Notify the IPW system administrator.\n"
	printf "        (file: $sysCmdList)\n"
	exit 1
fi

while getopts ":" opt
    do
    case $opt in
	*)  exec usage $pgm "$synopsis" "$description"
	    ;;
    esac
    done

case ${1:-user} in
    user)	${PAGER:-more} $cmdList
		;;
    system)	${PAGER:-more} $sysCmdList
		;;
    *)		exec usage $pgm "$synopsis" "$description"
		;;
esac

exit 0
