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
#include <endian.h>		/* le16toh() */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* memcpy() */
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <machine/xpio.h>

#include "config.h"
#include "xpfe.h"

#define XPFE_BLKSIZE 512
#define	XPFE_DISKMAX_BLK (256 * 256 * 256) /* 8GB at this moment */

/* extern */
extern struct xpfe_if_t *xpfe_if;
extern void *xpshm;
extern int v_flag;

/* static */
int xpdisk_fd;
off_t xpdisk_size;
uint8_t xpdisk_buf[XPFE_BLKSIZE];

void *
xpdisk_open(const char *fname)
{
	int fd;
	off_t size;
	void *buf;
	struct stat sb;

	fd = open(fname, O_RDWR, 0666);
	if (fd < 0)
		err(EXIT_FAILURE, "can not open disk image %s", fname);
	if (fstat(fd, &sb) != 0)
		err(EXIT_FAILURE, "can not stat disk image %s", fname);

	size = sb.st_size;
	if (size % XPFE_BLKSIZE != 0)
		warnx("%s: size is not multiple of XPFE_BLKSIZE (%d)",
		    fname, XPFE_BLKSIZE);
	if (size == 0 || (size / XPFE_BLKSIZE > XPFE_DISKMAX_BLK))
		err(EXIT_FAILURE, "%s: invalid size (%lld)", fname, size);

	if (v_flag)
		printf("%s: Disk image: %s (%lld bytes)\n",
		    getprogname(), fname, size);

	xpdisk_fd = fd;
	xpdisk_size = sb.st_size;
}

void
xpdisk_close(void)
{
	volatile uint32_t *xpd_flag = &(xpfe_if->xpd_flag);
	uint32_t flag;

	flag = *xpd_flag;
	*xpd_flag = flag & 0xffffff00;

	close(xpdisk_fd);
}

#ifdef DEBUG
void
xpdisk_debug_dump(uint8_t *buf)
{
	int fd;

	fd = open("./xpdisk_debug.dump", O_RDWR | O_CREAT, 0666);
	if (fd < 0)
		err(EXIT_FAILURE, "can not open dump file");

	write(fd, buf, XPFE_BLKSIZE);

	close(fd);
}
#endif

void
xpdisk_transfer(struct xpfe_if_t *xpfe_if)
{
	volatile uint32_t *xpd_lba = &(xpfe_if->xpd_lba);
	volatile uint32_t *xpd_da  = &(xpfe_if->xpd_dir_addr);
	int dir, xpaddr;
	off_t pos;

	dir = (*xpd_da & 0xff000000) >> 24;
	/* XP stores its address in little endian (use 16bit for now) */
	xpaddr = (int)le16toh(*xpd_da & 0x0000ffff);

	pos = lseek(xpdisk_fd, (off_t)(*xpd_lba) * XPFE_BLKSIZE, SEEK_SET);
	if (pos == -1) {
		warnx("%s: seek error, LBA = 0x08x", __func__, *xpd_lba);
		return;
	}

	if (dir == 0) {	/* XP wants to read */
		read(xpdisk_fd, xpdisk_buf, XPFE_BLKSIZE);
		memcpy((void *)(xpshm + xpaddr), xpdisk_buf, XPFE_BLKSIZE);
	} else {	/* XP wants to write */
		memcpy(xpdisk_buf, (void *)(xpshm + xpaddr), XPFE_BLKSIZE);
		write(xpdisk_fd, xpdisk_buf, XPFE_BLKSIZE);
	}
}

void
xpdisk_io(void)
{
	volatile uint32_t  *xpd_flag = &(xpfe_if->xpd_flag);

	/* disk I/O */
	if (*xpd_flag & 0xff000000) {
		xpdisk_transfer(xpfe_if);
		*xpd_flag &= 0x00ffffff;
		*xpd_flag |= 0x00ff0000;
	}
}

void
xpdisk_register(void)
{
	volatile uint32_t *xpd_flag = &(xpfe_if->xpd_flag);
	volatile uint32_t *xpd_blknum = &(xpfe_if->xpd_blknum);
	uint32_t flag;

	flag = *xpd_flag;
	*xpd_flag = flag | 0x00000001;

	*xpd_blknum = (uint32_t)(xpdisk_size / XPFE_BLKSIZE);
}
