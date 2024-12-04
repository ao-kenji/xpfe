# xpfe

## NAME
**xpfe** - OMRON LUNA's XP (HD647180X I/O processor) front-end

## SYNOPSIS
**xpfe** _binfile_ _diskimage_

## DESCRIPTION
The **xpfe** utility is a support program for HD647180X I/O Processor, so-called XP, on OMRON LUNA series workstations.
It provides following functions to XP via the shared memory which LUNA's main CPU can access.
- input from tty on LUNA
- output to tty on LUNA
- read from/write to the _diskimage_ file on LUNA per 512 bytes block
- real-time clock (date and time)

The **xpfe** loads _binfile_, maps it on XP's memory begins at 0x0000, and resets XP.


## ENVIRONMENT
The **xpfe** runs on OpenBSD/luna88k and NetBSD/luna68k.

## BUILD
Just extract and make.
```
% tar zxf xpfe.tar.gz
% cd xpfe
% make
```
The executable binary is "xpfe".

## DISCLAIMER
ABSOLUTELY NO WARRANTY

## AUTHOR
Kenji Aoyama
