# xpfe

## NAME
**xpfe** - OMRON LUNA's XP (HD647180X I/O processor) front-end

## SYNOPSIS
**xpfe** [**-v**] [**-c** _config_file_ ] _binfile_ _diskimage_

## DESCRIPTION
The **xpfe** utility is a support program for HD647180X I/O Processor, so-called XP, on OMRON LUNA series workstations.
It is intended to run Z80 binaries, such as FUZIX and CP/M, on XP.
It communicates with XP via shared memory that is accessible by LUNA's main CPU.

**xpfe** provides following functions:
- input from tty on LUNA
- output to tty on LUNA
- read from/write to _diskimage_ on LUNA, per 512 bytes block
- real-time clock (date and time)

**xpfe** loads _binfile_ image, maps it on XP's memory begins at 0x0000, then resets XP to start that image.
To quit **xpfe**, type 'Control-\\'. (This may be changed in the future.)

The options are as follows:

**-c** _config_file_

Use the specified configuration by _config_file_.

**-v**

Set the Verbose mode. (mostly for debugging)

## RUNNING ENVIRONMENT
**xpfe** runs on OpenBSD/luna88k and NetBSD/luna68k, on the real hardware and "nono" emulator.

## BUILD
Just extract and make.
```
% tar zxf xpfe.tar.gz
% cd xpfe
% make
```
The executable binary is "xpfe".

## REFERENCES
- OpenBSD/luna88k https://www.openbsd.org/luna88k.html
- NetBSD/luna68k https://wiki.netbsd.org/ports/luna68k/
- nono - LUNA emulator http://www.pastel-flower.jp/~isaki/nono/
- FUZIX for LUNA XP (work-in-progress) https://github.com/ao-kenji/FUZIX/

## AUTHOR
Kenji Aoyama
