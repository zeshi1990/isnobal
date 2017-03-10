# Create necessary files for converting IPW image data 
# to Arc/Info via IMAGEGRID.
#   Output files:  
#      out_prefix.lq has "lqmin,lqmax,nbits", and is read by ipw2grid.aml
#      out_prefix.bip and .hdr files are read by the arc IMAGEGRID command.
#

if (@ARGV < 2) {
    $0 =~ s:.*/::;
    print <<EOF;

    $0 -- Called by IPW2GRID.AML for converting an IPW image to Arc/Info

    Usage: $0 ipw_img out_prefix  

	ipw_img    - input IPW image
	out_prefix - prefix for output files:
		     .bip, .hdr: read by the IMAGEGRID Arc command.
		     .lq:        read by IPW2GRID.AML

EOF
exit 1;
}
$ipwfile = $ARGV[0];
$outpre  = $ARGV[1];

$bip = "$outpre.bip"; 	# bip will be a symlink to the image
$hdr = "$outpre.hdr";
$lq  = "$outpre.lq";
unlink("$outpre.bip", "$outpre.hdr", "$outpre.lq");

(! -e $ipwfile) && die "ERROR: $0:\n\t $ipwfile not found.\n";
symlink($ipwfile, $bip) || die "ERROR: $0:\n\t can't symlink to $bip.\n";

# get BIH and LQ info
($bands, $lines, $samps, $bytes) = split(' ',`ipwfile -blsym $ipwfile`);
($bands > 1) && die "ERROR: $0:\n\t image is multi-band.  Use IPW2STACK\n"; 
($lqmin, $lqmax) = split(' ',`lqmm $bip`);

# read GEO, LQ hdrs
open(HDR, "demux -b 0 $ipwfile | prhdr | rmhdr -d win|")
  || die "ERROR: $0:\n\t can't read ipw headers for $ipwfile.\n";
$lqflag = 0;
while(<HDR>) {
    (/^map/) && ($lqflag++);
    ($key, $val) = split(' = ');
    if($key eq "bline")  { $bline = $val; }
    if($key eq "bsamp")  { $bsamp = $val; }
    if($key eq "dline")  { $dline = $val; }
    if($key eq "dsamp")  { $dsamp = $val; }
    if($key eq "bits")   { $lqbits  = $val; }
}
if(! $bline || ! $bsamp || ! $dline || ! $dsamp) {
    warn "WARN: $0:\n\t no geo header, using default 1,1,1,1 ...\n"; 
    $bline = $bsamp = $dline = $dsamp = 1;
}
($lqflag > 2) && die "ERROR: $0:\n\t can't handle > 2 lq breakpoints.\n" .
	"\t Use the old ASCII-transfer version (IPW2GRID_OLD).\n";

$imgsize   = -s $bip;
$skipbytes = $imgsize - ($lines * $samps * $bytes * $bands);

# write lq info
if(! $lqflag) { 
    warn "WARN: $0:\n\t no LQ header found, using raw values.\n";
    $lqmin = 0; $lqmax = 2 ** $lqbits - 1;
}
open(LQ, ">$lq") || die "ERROR: $0:\n\t can't open $lq\n";
printf LQ "%.16f,%.16f,%d\n", $lqmin, $lqmax, $lqbits;
close(LQ);

# write header info
system "cat <<EOF >$hdr
COMMENT: this file was written by $0
COMMENT: arguments:  @ARGV
layout        BIP
nrows         $lines
ncols         $samps
nbands        $bands
nbits         ${\(8 * $bytes)}
skipbytes     $skipbytes
xdim          ${\abs($dsamp)}
ydim          ${\abs($dline)}
ulxmap        $bsamp
ulymap        $bline
EOF";
