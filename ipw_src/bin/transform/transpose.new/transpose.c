#include "ipw.h"

#include <string.h>

#include "../transpose.new/pgm.h"
#include "bih.h"
#include "gethdrs.h"
#include "geoh.h"
#include "orh.h"
#include "winh.h"


/*
 * transpose -- orchestrate image transpose
 */

void
transpose(
		int             i_fd,		/* input image file descriptor	 */
		int             o_fd)		/* output image file descriptor	 */
{
	/* NOSTRICT */
	static GETHDR_T h_or = {ORH_HNAME, (ingest_t) orhread};
	/* NOSTRICT */
	static GETHDR_T h_win = {WINH_HNAME, (ingest_t) winhread};
	/* NOSTRICT */
	static GETHDR_T h_geo = {GEOH_HNAME, (ingest_t) geohread};
	static GETHDR_T *hv[] = {&h_or, &h_win, &h_geo, NULL};

	BIH_T         **i_bihpp;	/* -> input BIH			 */
	BIH_T         **o_bihpp;	/* -> output BIH		 */
	GEOH_T         *geohp;		/* -> GEOH header		 */
	GEOH_T        **o_geohpp;	/* -> output GEOH		 */
	ORH_T          *orhp;		/* -> ORH header		 */
	ORH_T         **o_orhpp;	/* -> output ORH		 */
	WINH_T         *winhp;		/* -> WINH header		 */
	WINH_T        **o_winhpp;	/* -> output WINH		 */
	bool_t          need_orh;	/* ? write ORH			 */
	double          hold;		/* temporary storage		 */
	int             band;		/* current band #		 */
	int             nlines;	/* # input lines		 */
	int             nsamps;	/* # samples / input line	 */
	int             nbands;		/* # image bands		 */
	int				N;			/* number of pixels */
	fpixel_t		*i_buf;		/* -> input image buffer		 */
	fpixel_t		*o_buf;		/* -> output image buffer		 */
	int             i_size;		/* # bytes / input image		 */
	int				samp_size;

	/*
	 * read BIH
	 */
	i_bihpp = bihread(i_fd);
	if (i_bihpp == NULL) {
		error("can't read basic image header");
	}

	nbands = hnbands(i_fd);
	nlines = hnlines(i_fd);
	nsamps = hnsamps(i_fd);
	N = nlines * nsamps;

	/*
	 * reset o_byteorder from hostorder() to byte order of i_fd
	 * because this program will bypass the pixio layer
	 */
	o_byteorder = hbyteorder(i_fd);

	/*
	 * create and write new BIH
	 */
	o_bihpp = bihdup(i_bihpp);
	if (o_bihpp == NULL) {
		error("can't create basic image header");
	}

	bih_nlines(o_bihpp[0]) = nsamps;
	bih_nsamps(o_bihpp[0]) = nlines;

	if (bihwrite(o_fd, o_bihpp) == ERROR) {
		error("can't write basic image header");
	}

	/*
	 * process remaining headers
	 */
	gethdrs(i_fd, hv, nbands, o_fd);

	need_orh = FALSE;

	/*
	 * if no orientation header then create one
	 */
	if (!got_hdr(h_or)) {
		orhp = orhmake(XT_ORIENT, IPW_ORIGIN);
		if (orhp == NULL) {
			error("can't make OR header");
		}
		/* NOSTRICT */
		o_orhpp = (ORH_T **) hdralloc(nbands, sizeof(ORH_T *),
				ERROR, ORH_HNAME);
		if (o_orhpp == NULL) {
			error("can't allocate OR header array");
		}

		for (band = 0; band < nbands; ++band) {
			o_orhpp[band] = orhp;
		}

		need_orh = TRUE;
	}
	/*
	 * if existing orientation header then modify it
	 */
	else {
		/* NOSTRICT */
		o_orhpp = (ORH_T **) hdr_addr(h_or);

		for (band = 0; band < nbands; ++band) {
			if ((orhp = o_orhpp[band]) == NULL) {
				continue;
			}

			if ( streq(orh_orient(orhp), ROW) ) {
				SAFE_FREE( orh_orient(orhp) );
				orh_orient(orhp) = strdup(COLUMN);
			} else {
				SAFE_FREE( orh_orient(orhp) );
				orh_orient(orhp) = strdup(ROW);
			}

			if (strdiff(orh_orient(orhp), IPW_ORIENT) ||
					strdiff(orh_origin(orhp), IPW_ORIGIN)) {
				need_orh = TRUE;
			}
		}
	}

	/*
	 * if output image not IPW standard orientation then write ORH header
	 */
	if (need_orh) {
		if (orhwrite(o_fd, o_orhpp) == ERROR) {
			error("can't write orientation header");
		}
	}
	(void) orhfree(o_orhpp, nbands);

	/*
	 * Modify window header if there is one. Exchange bline & bsamp, dline
	 * & dsamp.
	 */
	if (got_hdr(h_win)) {
		/* NOSTRICT */
		o_winhpp = (WINH_T **) hdr_addr(h_win);

		for (band = 0; band < nbands; ++band) {
			if ((winhp = o_winhpp[band]) != (WINH_T *) NULL) {
				hold = winh_bline(winhp);
				winh_bline(winhp) = winh_bsamp(winhp);
				winh_bsamp(winhp) = hold;
				hold = winh_dline(winhp);
				winh_dline(winhp) = winh_dsamp(winhp);
				winh_dsamp(winhp) = hold;
			}
		}

		if (winhwrite(o_fd, o_winhpp) == ERROR) {
			error("can't write window header");
		}
		(void) winhfree(o_winhpp, nbands);
	}

	/*
	 * Modify geodetic header if there is one. Exchange bline & bsamp,
	 * dline & dsamp.
	 */
	if (got_hdr(h_geo)) {
		/* NOSTRICT */
		o_geohpp = (GEOH_T **) hdr_addr(h_geo);

		for (band = 0; band < nbands; ++band) {
			if ((geohp = o_geohpp[band]) != (GEOH_T *) NULL) {
				hold = geoh_bline(geohp);
				geoh_bline(geohp) = geoh_bsamp(geohp);
				geoh_bsamp(geohp) = hold;
				hold = geoh_dline(geohp);
				geoh_dline(geohp) = geoh_dsamp(geohp);
				geoh_dsamp(geohp) = hold;
			}
		}

		if (geohwrite(o_fd, o_geohpp) == ERROR) {
			error("can't write geodetic header");
		}
		(void) geohfree(o_geohpp, nbands);
	}

	/*
	 * no more headers
	 */
	if (boimage(o_fd) == ERROR) {
		error("can't terminate header output");
	}

	/*
	 * allocate buffers and read image data in
	 */
	samp_size = sizeof(fpixel_t);
	i_size = N * samp_size;
	i_buf = (fpixel_t *) ecalloc(N, samp_size);
	if (i_buf == NULL) {
		error("can't allocate input line buffer");
	}

	o_buf = (fpixel_t *) ecalloc(N, samp_size);
	if (o_buf == NULL) {
		error("output image won't fit in memory");
	}

	//	if (uread(i_fd, i_buf, i_size) != i_size) {
	//		error("input image read failed");
	//	}
	if (fpvread (i_fd, i_buf, N) != N) {
		error ("input image read error");
	}

	/*
	 * transpose image
	 */
	ximg(i_buf, nlines, nsamps, o_buf);

	/*
	 * write out data
	 */
	//	for (line = 0; line < o_nlines; ++line) {
	//	if (uwrite(o_fd, i_buf, i_size) != i_size) {
	//		error("output image write failed");
	//	}
	if (fpvwrite (o_fd, o_buf, N) != N) {
		error ("write error output image");
	}
	//		o_img += o_linesize;
	//	}
//	SAFE_FREE(i_buf);
//	SAFE_FREE(o_buf);
}











