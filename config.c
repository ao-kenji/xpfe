/*
 * Copyright (c) 2025 Kenji Aoyama
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>	/* isspace() */
#include <err.h>
#include <errno.h>
#include <limits.h>

#include "xpfe.h"

#define MAX_LINE_LEN 256
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 128

/* extern */
extern struct xpfe_config_t xpfe_config;
extern int v_flag;

/*
 * Trim the string
 * Note: the argument string is not constant; it will be modified.
 */
char *
trim(char *s)
{
	char *e;

	/* skip spaces in head */
	while (isspace((int)*s))
		s++;

	/* return if the string is null */
	if (*s == '\0')
        	return s;

	/* delete spaces in tail */
	e = s + strnlen(s, MAX_LINE_LEN) - 1;
	while (e > s && isspace((int)*e))
        	e--;
	*++e = '\0';

	return s;
}

int
yes_no(const char *s)
{
	if ((strcasecmp(s, "yes") == 0) || (strcmp(s, "1") == 0))
		return 1;
	if ((strcasecmp(s, "no") == 0) || (strcmp(s, "0") == 0))
		return 0;
	return -1;
}

int
read_config(const char *fname)
{
	FILE *fp;
	char line[MAX_LINE_LEN];
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	char *p;
	int val;

	fp = fopen(fname, "r");
	if (fp == NULL) {
		if (v_flag)
			printf("config file not found\n");
		return 1;
	}

	while (fgets(line, sizeof(line), fp)) {

		/*
		  skip comment or empty line
		  Note: comment line should start with '#', no spaces
		 */
		if (line[0] == '#' || trim(line)[0] == '\0')
			continue;

		/* search '=' */
		p = strchr(line, '=');
		if (p == NULL) {
			warnx("invalid format: %s", line);
			continue;
		}

		/* get key and value */
		*p = '\0';
		strlcpy(key, line, MAX_KEY_LEN);
		strlcpy(value, p + 1, MAX_VALUE_LEN);

		/* trim key and value */
		trim(key);
		trim(value);

		/* set key and value */
		if (strcmp(key, "if-addr") == 0) {
			val = (int)strtol(value, &p, 0);
           		if (value[0] == '\0' || *p != '\0')
				warnx("Not a number");
           		if (errno == ERANGE &&
			    (val == LONG_MAX || val == LONG_MIN))
				warnx("Out of range");
			if ((val < 0) || (val > 0x1ffff))
				warnx("Out of range");
			xpfe_config.addr = val;
		} else if (strcmp(key, "use-disk") == 0) {
			val = yes_no(value);
			if (val == -1)
				warnx("Invalid value for use-disk");
			else
				xpfe_config.disk = val;
		} else if (strcmp(key, "use-rtc") == 0) {
			val = yes_no(value);
			if (val == -1)
				warnx("Invalid value for use-rtc");
			else
				xpfe_config.rtc = val;
		}
    	}
	fclose(fp);

	if (v_flag) {
		printf("if-addr: 0x%x\n", xpfe_config.addr);
		printf("disk   : %d\n", xpfe_config.disk);
		printf("rtc    : %d\n", xpfe_config.rtc);
	}

    	return 0;
}
