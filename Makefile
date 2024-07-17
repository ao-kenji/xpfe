#
# Makefile for xpfe
#

PROG = xpfe
SRCS = xpfe.c xpload.c xptty.c
BINDIR = $(HOME)/bin
NOMAN = 1

.include <bsd.prog.mk>
