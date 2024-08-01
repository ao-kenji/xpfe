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

#ifndef	__XPFE_H__
#define	__XPFE_H__

struct xpfe_if_t {
	char magic[4];			/*  0: "XPFE" */
	volatile uint32_t rbuf;		/*  4: tty-out from XP */
	volatile uint32_t tbuf;		/*  8: tty-in to XP */
	char padd[4];			/* 12: padding */
	volatile uint32_t d_command;	/* 16: disk command from XP */
	volatile uint32_t d_flag;	/* 20: disk flag */
	volatile uint8_t  d_buf[512];	/* 24: disk buffer */
} __packed;
#endif
