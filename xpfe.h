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

struct xpfe_if_t {			/* T:tty, R:RTC, D:disk */
	char magic[4];			/* magic "XPFE" */
	volatile uint32_t t_rxbuf;	/* T: tty-out from XP */
	volatile uint32_t t_txbuf;	/* T: tty-in to XP */
	volatile uint8_t  rtc[8];	/* R: RTC value in YYYYMMDDHHMMSS */
	volatile uint32_t xpd_lba;	/* D: disk LBA */
	volatile uint32_t xpd_dir_addr;	/* D: transfer direction and XP addr */
	volatile uint32_t xpd_flag;	/* D: flag */
	volatile uint32_t xpd_blknum;	/* D: disk size (in blk) */
};

extern struct xpfe_if_t *xpfe_if;
extern void *xpshm;

#endif
