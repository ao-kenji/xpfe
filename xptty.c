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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "config.h"

#define	RX_OFFSET	4
#define	TX_OFFSET	8

volatile uint32_t *xprbuf;	/* offset 0x7f04: from xptty */
volatile uint32_t *xptbuf;	/* offset 0x7f08: to xptty */

/* external */
extern void *xpshm;		/* in xpfe.c */

/* internal use */
static struct termios termios_saved;

void
xptty_init(void)
{
	if (xpshm == NULL) {
		err(EXIT_FAILURE, "xpshm not initialized");
	}

	xprbuf = (u_int32_t *)(xpshm + XP_TTY_OFFSET + RX_OFFSET);
	xptbuf = (u_int32_t *)(xpshm + XP_TTY_OFFSET + TX_OFFSET);
}

void
xptty_set_rawmode(void)
{
	struct termios termios_new;

	if (tcgetattr(STDIN_FILENO, &termios_saved) == -1) {
		err(EXIT_FAILURE, "tcgetattr");
	}

	termios_new = termios_saved;
	termios_new.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	termios_new.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	termios_new.c_cflag |= (CS8);
	termios_new.c_oflag &= ~(OPOST);
	termios_new.c_cc[VMIN] = 0;     /* per one character */
	termios_new.c_cc[VTIME] = 0;    /* disable timer */

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_new) == -1) {
		err(EXIT_FAILURE, "tcsetattr");
	}
}

void
xptty_reset_mode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_saved) == -1) {
		err(EXIT_FAILURE, "tcsetattr");
	}
}

void
xptty_receive(void)
{
	char c;
	u_int code;

	/* Receive */
	if (*xprbuf & 0x00ff0000) {
		code = (u_int)((*xprbuf & 0xff000000) >> 24);
		*xprbuf &= 0x0000ffff;  /* clear data & flag */
		c = (char)code;
		write(STDOUT_FILENO, &c, 1);
	}
}

void
xptty_send(char c)
{
	/* Transmit */
	if ((*xptbuf & 0x00ff0000) == 0) {
		*xptbuf =((c & 0xff) << 24) | 0x00ff0000;
	}
}
