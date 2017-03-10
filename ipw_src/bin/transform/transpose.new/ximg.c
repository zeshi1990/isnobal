#include <math.h>

#include "../transpose.new/pgm.h"
#include "ipw.h"

#include "pixio.h"


/*
 * ximg -- transpose image
 */

void
ximg(
		fpixel_t           *i_buf,		/* input image 	 */
		int             i_nlines,	/* # input lines		 */
		int             i_nsamps,	/* # samples / input line	 */
		fpixel_t           *o_buf)		/* output image 	 */
{
//	int             line;		/* current line #		 */
//	int           *i_bufp;		/* -> input image line		 */
	//	int             i_linesize;	/* # bytes / input line		 */
	//	char           *o_buf;		/* -> output image		 */
//	char           *o_img;		/* -> base of output img array	 */
	//	char           *o_imgp;		/* -> base of output img array	 */
//	int             o_linesize;	/* # bytes / input line		 */
//	REG_4 int       o_lineincr;	/* o_linesize - 1		 */
	int             o_nlines;	/* # output lines		 */
	int             o_nsamps;	/* # samples / output line	 */
	int				i_index;	/* index into input file */
	int 			N;
	int 			i;
//	int				next;

	o_nlines = i_nsamps;
	o_nsamps = i_nlines;
	N = i_nsamps * i_nlines;

	/*
	 * allocate I/O buffers
	 */
//	o_linesize = o_nsamps * samp_nbytes;

	//	o_buf = (char *) ecalloc(o_nlines, o_linesize);
	//	if (o_buf == NULL) {
	//		error("output image won't fit in memory");
	//	}

	//	i_linesize = i_nsamps * samp_nbytes;

	//	i_buf = (char *) ecalloc(i_linesize, 1);
	//	if (i_buf == NULL) {
	//		error("can't allocate input line buffer");
	//	}
	/*
	 * save output image base address (transpose algorithm modifies o_buf)
	 */
	//	o_img  = o_buf;
	//	o_imgp = o_buf;
	/*
	 * output line increment within o_buf
	 */
//	o_lineincr = o_linesize - samp_nbytes;

	/*
	 * transpose
	 */
	for (i = 0; i < N; i++) {
		i_index = (i % o_nsamps) * o_nsamps + (int) floor(i/o_nsamps);
		o_buf[i_index] = i_buf[i];
	}


	//	for (line = 0; line < i_nlines; ++line) {
	//		//		REG_2 char     *i_bufp;	/* -> current input pixel	 */
	//		REG_5 int       i_samp;	/* current input sample #	 */
	//		REG_1 int     *o_bufp;	/* -> current output pixel	 */
	//
	//		i_index = line * i_nsamps;
	//
	//		//		if (uread(i_fd, i_buf, i_linesize) != i_linesize) {
	//		//			error("image read failed, line %d", line);
	//		//		}
	//		/*
	//		 * copy each input sample to first free position in each output line
	//		 */
	//		i_bufp = i_buf;
	//		o_bufp = o_buf;
	//
	//		i_samp = i_nsamps;
	//		do {
	//			REG_3 int       nbytes;	/* current sample byte # */
	//
	//			nbytes = samp_nbytes;
	//			do {
	//				*o_bufp++ = *i_bufp++;
	//				//				i_index++;
	//			} while (--nbytes > 0);
	//
	//			o_bufp += o_lineincr;
	//		} while (--i_samp > 0);
	//
	//		o_buf += samp_nbytes;
	//	}
	//	SAFE_FREE(i_buf);
	//	/*
	//	 * write lines
	//	 */
	//	for (line = 0; line < o_nlines; ++line) {
	//		if (uwrite(o_fd, o_img, o_linesize) != o_linesize) {
	//			error("image write failed, line %d", line);
	//		}
	//
	//		o_img += o_linesize;
	//	}
	//	SAFE_FREE(o_imgp);
}
