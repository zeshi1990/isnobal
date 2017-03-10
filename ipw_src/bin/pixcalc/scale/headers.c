#include "ipw.h"

#include "bih.h"
#include "gethdrs.h"
#include "lqh.h"

#include "pgm.h"

/*
 * headers -- process image headers
 */

extern void     head1(void);
extern void     head2(void);

void
headers(bool_t already)
{
	if (!already) {
		head1();
	}
	else {
		head2();
	}
}

/*
 * first pass: read/write BIH; read/convert LQH
 */

void
head1(void)
{
	BIH_T         **i_bihpp;	/* -> input BIH array		 */
	BIH_T         **m_bihpp;	/* -> mask BIH array		 */
	BIH_T         **o_bihpp;	/* -> output BIH array		 */
	int             nbands;		/* # input bands		 */

 /* NOSTRICT */
	static GETHDR_T h_lqh = {LQH_HNAME, (ingest_t) lqhread};
	static GETHDR_T *request[] = {&h_lqh, 0};

 /*
  * read BIH
  */
	i_bihpp = bihread(parm.i_fd);
	if (i_bihpp == NULL) {
		error("can't read basic image header");
	}
	nbands = hnbands(parm.i_fd);
	if (nbands != 1)
		error("image has %d bands; single band images only", nbands);
 /*
  * read mask BIH
  */
	if (parm.m_fd != ERROR) {
		m_bihpp = bihread(parm.m_fd);
		if (m_bihpp == NULL) {
			error("can't read mask basic image header");
		}
		if (hnlines(parm.m_fd) != hnlines(parm.i_fd) ||
		    hnsamps(parm.m_fd) != hnsamps(parm.i_fd))
			error("input and mask images have different dimensions");
		if (hnbands(parm.m_fd) != 1)
			error("mask image must be single band image");
		skiphdrs (parm.m_fd);
	}
 /*
  * if nbits not specified on command line, use maximum among input
  * bands
  */
	parm.nbits = i_bihpp[0]->nbits;
 /*
  * create and write output BIH
  */
 /* NOSTRICT */
	o_bihpp = (BIH_T **) hdralloc(1, sizeof(BIH_T *), parm.o_fd, BIH_HNAME);
	assert(o_bihpp != NULL);
	o_bihpp[0] = bihmake(0, parm.nbits,
		       bih_history(i_bihpp[0]), bih_annot(i_bihpp[0]),
			     NULL,
		       bih_nlines(i_bihpp[0]), bih_nsamps(i_bihpp[0]),
			     1);
	if (o_bihpp == NULL) {
		error("can't make output BIH");
	}
	if (bihwrite(parm.o_fd, o_bihpp) == ERROR) {
		error("can't write basic image header");
	}
 /*
  * read the input LQH, band 0 of other headers get passed
  */
	gethdrs(parm.i_fd, request, 1, parm.o_fd);

}

/*
 * write output LQH
 */

void
head2(void)
{
	pixel_t         ival[2];	/* min, max pixel values	 */
	LQH_T         **o_lqhpp;	/* -> output LQH array		 */

 /*
  * make and write the output LQH
  */
	ival[0] = 0;
	ival[1] = ipow2(parm.nbits) - 1;
	if (parm.mmval[0] == parm.mmval[1])
		parm.mmval[1] = parm.mmval[0] + 1;
 /* NOSTRICT */
	o_lqhpp = (LQH_T **) hdralloc(1, sizeof(LQH_T *),
				      parm.o_fd, LQH_HNAME);
	o_lqhpp[0] = lqhmake(parm.nbits, 2, ival,
		      &(parm.mmval[0]), (char *) NULL, (char *) NULL);
	if (lqhwrite(parm.o_fd, o_lqhpp) == ERROR) {
		error("can't write LQH");
	}
 /*
  * prepare to write image data
  */
	if (boimage(parm.o_fd) == ERROR) {
		error("can't terminate header output");
	}
}
