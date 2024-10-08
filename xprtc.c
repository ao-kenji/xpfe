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
#include <stdio.h>	/* printf() */
#include <stdlib.h>
#include <string.h>	/* memcpy() */
#include <time.h>

#include "xpfe.h"

void
xprtc_sync(void)
{
	volatile uint8_t *xprtc = xpfe_if->rtc;
	time_t t;
	struct tm *now;

	time(&t);
	now = gmtime(&t);

	*xprtc++ = (uint8_t)((now->tm_year + 1900) % 256);
	*xprtc++ = (uint8_t)((now->tm_year + 1900) / 256);
	*xprtc++ = (uint8_t) (now->tm_mon);
	*xprtc++ = (uint8_t) (now->tm_mday);
	*xprtc++ = (uint8_t) (now->tm_hour);
	*xprtc++ = (uint8_t) (now->tm_min);
	*xprtc++ = (uint8_t) (now->tm_sec);
}
