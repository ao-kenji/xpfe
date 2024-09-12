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
int v_flag = 0;		/* verbose */
int has_disk = 0;
int xpfd;
struct xpfe_if_t *xpfe_if;
volatile void *xpshm;

/* prototypes */
void usage(void);

/* xpdisk.c */
void xpdisk_open(const char*);
void xpdisk_io(void);
void xpdisk_close(void);
void xpdisk_register(void);

/* xpload.c */
void xp_load_reset(int, const char *);
void *xp_mmap(int);

/* xprtc.c */
void xprtc_sync(void);

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

	while ((ch = getopt(argc, argv, "av")) != -1) {
		switch (ch) {
		case 'a':
			a_flag = 1;
			break;
		case 'v':
			v_flag = 1;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if ((argc == 0) || (argc > 2))
		usage();

	xpfname = argv[0];
	xpfd = open(XP_DEV, O_RDWR);
	if (xpfd < 0 )
		err(EXIT_FAILURE, "can not open %s", XP_DEV);

	if (argc == 2) {
		xpdisk_open(argv[1]);
		has_disk = 1;
	}

	xpshm  = xp_mmap(xpfd);

	if (a_flag)
		printf("XP attached, ");
	else {
		xp_load_reset(xpfd, xpfname);
		printf("XP binary loaded, ");
	}
	printf("type '^\\' to detach.\n");

	xptty_init();
	xpdisk_register();
	xptty_set_rawmode();
	running = 1;

	while (running) {
		/* Sync RTC */
		xprtc_sync();

		/* Receive & put to stdout, first */
		xptty_receive();

		/* Disk I/O */
		if (has_disk)
			xpdisk_io();

		/* Check Key in */
		ret = read(STDIN_FILENO, &c, 1);

		if (ret == 0)
			continue;
		if (c == XPFE_QUIT) {	/* default: 'Control-\' */
			running = 0;
			continue;
		}

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
	printf("Usage: %s [options] xp_prog_file [disk_image]\n",
	    getprogname());
	printf("\t-a		: attach to running XP\n");
	printf("\t-v		: verbose mode\n");
	exit(EXIT_FAILURE);
}
