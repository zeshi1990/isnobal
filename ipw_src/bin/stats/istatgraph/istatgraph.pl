$SIG{'INT'}  = 'sighandler';
$SIG{'QUIT'} = 'sighandler';

# This should change.  For now we'll use this version because it's the
# one that splits out the right verbose output.
#
$istatcom = "/usr/packages/sparc/ipw/bin/istatrel";

$TMPDIR = ($ENV{'TMPDIR'}) ? $ENV{'TMPDIR'} : '/tmp';

$tdfile = "$TMPDIR/f12.$$";
$tgfile = "$TMPDIR/gnup.$$";
$trfile = "$TMPDIR/istat.$$";

if ($#ARGV == -1 || $ARGV[0] eq "-H" || $ARGV[0] eq "-help") {
  print <<"EOF";

istatgraph -- plot two images and istatrel's interpolations

Usage: istatgraph [-n number] [-r] [-p] image1 image2 [-- istatrel options]

Options:
	-n	Plot close to {number} (default: 1000) points.
	-r	Use resamp instead of zoom to reduce the number of points.
	-p	Print postscript instead of plotting to an X window.
	-c	If generating postscript, generate color postscript.
	--	Treat all further options as file names and/or istatrel options.

Operands:
	image1	The independent image
	image2	The dependent image

EOF
  exit 0;
}

$npoints = 1000;
$zoomc = 'zoom';
$printing = 0;
$verbose = 0;
$colorps = 0;
$keypos = 0;

while (@ARGV) {
  $_ = shift @ARGV;
  /^--$/         && do { push(@files, @ARGV); undef @ARGV; next; };
  /^-r$/         && do { $zoomc = 'resamp'; next; };
  /^-p$/         && do { $printing = 1; next; };
  /^-c$/         && do { $colorps = 1; next; };
  /^-v$/         && do { $verbose = 1; next; };
  /^-k$/         && do { $keypos = shift; next; };
  /^-n$/         && do { $npoints = shift; next; };
  push (@files, $_);
}

$file1 = shift(@files);
$file2 = shift(@files);
$statcoms = join(' ', @files);

die "istatgraph: Can't read $file1.\n" unless -f $file1;
die "istatgraph: Can't read $file2.\n" unless -f $file2;

$verbose && print STDERR "Making ASCII images of $file1 and $file2 for gnuplot.\n";

open (IPWF, "ipwfile -mls $file1 |");
($lines, $samps) = split(" ",<IPWF>);
close (IPWF);

$ipoints = $lines * $samps;

$npoints = 1 if $npoints < 1;
$zooma = int(sqrt($ipoints / $npoints) + 0.5);


if ($zooma == 0) {
  $subsamp = "";
} else {
  $zoomal = $zooma;
  $zoomas = int( ( ( ($lines / $zoomal) * $samps ) / $npoints) + 0.5);
  if ($zoomc eq 'resamp') {
    $subsamp = "resamp -l $zoomal -s $zoomas | ";
  } else {
    $subsamp = "zoom -l -$zoomal -s -$zoomas | ";
  }
}

system "mux $file1 $file2 | $subsamp primg -a > $tdfile" ||
       die "Can't write file";

open (WCP, "wc $tdfile | ");
($olines, $words, $chars, $fname) = split(" ",<WCP>,4);
close (WCP);

$verbose && print STDERR "image has $ipoints points, plotting $olines.\n";

$style = ($olines > 100) ? "dots" : "points";

open (GNUP, ">$tgfile");

$printing && print GNUP "set terminal postscript";
$printing && $colorps && print GNUP " color";
$printing && print GNUP " \"Times-Roman\" 12\n";
print GNUP "set title \"$file1  vs.  $file2, with interpolations\"\n";
print GNUP "set xlabel \"$file1\"\n";
print GNUP "set ylabel \"$file2\"\n";
$keypos && print GNUP "set key $keypos\n";

print GNUP "plot '", "$tdfile", "' title \"interp  (error   )\" with $style";

$verbose && print STDERR "\nRunning istatrel on the images.\n";

open (ISTAT, "$istatcom -g $statcoms $file1 $file2 2>&1 >$trfile | ");

$| = 1;
$repcom = '';

while(<ISTAT>) {
  chop;
  s/ \((\d+\.\d+)\)$//;
  $errp = $1;
  s/^order (\d+): //;
  $order = $1;
  s/ x\^(\d+)/*x**$1/g;

  $repcom .= ", $_ title \"$order order ($errp)\"";
  $verbose && print STDERR "$order .. ";
}
close(ISTAT);
$verbose && print STDERR "done\n";

#$repcom =~ s/replot ,/replot /;
print GNUP $repcom, "\n";
$printing || print GNUP "pause -1 \"Hit Return to end.\"\n";
close GNUP;

$verbose && print STDERR "\nipwinterp says:\n";
$verbose && system "ipwinterp -a < $trfile";
$verbose && print STDERR "\n\n";
unlink "$trfile";

system "gnuplot $tgfile" || die "Can't run gnuplot";

unlink "$tdfile";
unlink "$tgfile";

exit 0;

# close up all the open pipes and unlink the temporary files.
#
sub sighandler {
  local($sig) = @_;

  close(IPWF);
  close(WCP);
  close(GNUP);
  unlink "$trfile", "$tdfile", "$tgfile";
  exit 0;
}
