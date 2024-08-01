/*
 * Copyright (c) 2024 Kenji Aoyama
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * xpfe: OMRON LUNA's XP (HD647180X I/O processor) front-end
 */

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"

/* global variables */
int a_flag = 0;
int d_flag = 0;
int xpfd;
volatile uint8_t *xpshm;

/* prototypes */
void usage(void);

/* xpdisk.c */
void xpdisk_open(const char*);
void xpdisk_io(void);
void xpdisk_close(void);

/* xpload.c */
void xp_load_reset(int, const char *);
void *xp_mmap(int);

/* xptty.c */
void xptty_init(void);
void xptty_set_rawmode(void);
void xptty_reset_mode(void);
void xptty_send(char);
void xptty_receive(void);

int
main(int argc, char *argv[])
{
	char c, *xpfname;
	int ch, ret, running;
	u_int code;
	extern int optind, opterr;

	setprogname(argv[0]);

	while ((ch = getopt(argc, argv, "ad:")) != -1) {
		switch (ch) {
		case 'a':
			a_flag = 1;
			break;
		case 'd':
			d_flag = 1;
			xpdisk_open(optarg);
			break;
		default:
			usage();
		}
	}

	xpfname = argv[optind];

	argc -= optind;
	argv += optind;

	if (!a_flag && (argc != 1)) {
		usage();
	}

	xpfd = open(XP_DEV, O_RDWR);
	if (xpfd < 0 )
		err(EXIT_FAILURE, "can not open %s", XP_DEV);

	xpshm  = xp_mmap(xpfd);

	if (a_flag)
		printf("XP attached, ");
	else {
		xp_load_reset(xpfd, xpfname);
		printf("XP binary loaded, ");
	}
	printf("type '^\\' to detach.\n");

	xptty_init();
	xptty_set_rawmode();
	running = 1;

	while (running) {
		/* Receive & put to stdout, first */
		xptty_receive();

		/* Check Key in */
		ret = read(STDIN_FILENO, &c, 1);

		if (ret == 0)
			continue;
		if (c == XPFE_QUIT) {	/* default: 'Control-\' */
			running = 0;
			continue;
		}

		if (d_flag)	 /* Disk I/O */
			xpdisk_io();

		/* Send */
		xptty_send(c);
	}

	xptty_reset_mode();
	xpdisk_close();
	close(xpfd);

	return EXIT_SUCCESS;
}

__dead void
usage(void)
{
	printf("Usage: %s [options] firmware_file\n", getprogname());
	printf("\t-a		: attach to running XP\n");
	printf("\t-d diskimage	: disk image file\n");
	exit(EXIT_FAILURE);
}
