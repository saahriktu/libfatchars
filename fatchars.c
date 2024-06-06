/*
 * libfatchars v0.6 - lowlevel UTF-8 library
 * 2017-2024 (c) Artem Kurashov
 * under GNU GPLv3
 */

#include <stdio.h>

int sizeoffatc(int headbyte)
{
    if (headbyte >= 0 && headbyte <= 0x7f)
	return 1;
    else if (headbyte >= 0xc0 && headbyte <= 0xdf)
	return 2;
    else if (headbyte >= 0xe0 && headbyte <= 0xef)
	return 3;
    else if (headbyte >= 0xf0 && headbyte <= 0xf7)
	return 4;
    else if (headbyte >= 0xf8 && headbyte <= 0xff)
	return -1;		// error
    return -1;			// error
}

int rsizeoffatc(int fatcode)
{
    if (fatcode > 0xffff)
	return 4;
    if (fatcode > 0x7ff)
	return 3;
    if (fatcode > 0x7f)
	return 2;
    return 1;
}

int ismodifierfatc(int fatcode)
{
    if (fatcode > 0x02ff && fatcode < 0x0370)
	return 1;
    if (fatcode > 0x1aaf && fatcode < 0x1b00)
	return 1;
    if (fatcode > 0x1dbf && fatcode < 0x1e00)
	return 1;
    if (fatcode > 0x20cf && fatcode < 0x2100)
	return 1;
    if (fatcode > 0xfe1f && fatcode < 0xfe30)
	return 1;
    return 0;
}

int decodefatc(int fcdt0, int fcdt1, int fcdt2, int fcdt3, int fcmaxk)
{
    if (fcmaxk == 1)
	return (fcdt0 & 0x3f) << 0x6 | (fcdt1 & 0x7f);
    if (fcmaxk == 2)
	return (fcdt0 & 0xe0) << 0xc | (fcdt1 & 0x7f) << 0x6 | (fcdt2 &
								  0x7f);
    return ((fcdt0 & 0xf0) << 0x12) | (((fcdt1 & 0x7f) << 0xc) +
	((fcdt2 & 0x7f) << 0x6)) | (fcdt3 & 0x7f);
}

int fgetfatc(FILE * fatstream)
{
    int cdbuf, cdt[4];
    int cdindx = 0, maxk;
    for (;;) {
	cdbuf = fgetc(fatstream);
	if (cdindx == 0) {
	    if (cdbuf == -1)
		return EOF;
	    if (cdbuf < 0x80)
		return cdbuf;	// ASCII
	    else
		maxk = sizeoffatc(cdbuf) - 1;
	    if (maxk == -2)
		return -2;
	}
	cdt[cdindx] = cdbuf;
	cdindx++;
	if (cdindx > maxk)
	    break;
    }
    return decodefatc(cdt[0], cdt[1], cdt[2], cdt[3], maxk);
}

int nextfatc(FILE * fatstream)
{
    int cdbuf;
    cdbuf = fgetc(fatstream);
    if (cdbuf == EOF)
	return EOF;
    return fseek(fatstream, sizeoffatc(cdbuf), SEEK_CUR);
}

int nextvisfatc(FILE * fatstream)
{
    int cdbuf;
    int ssts;
    do {
	cdbuf = fgetc(fatstream);
	if (cdbuf == EOF)
	    return EOF;
	ssts = fseek(fatstream, sizeoffatc(cdbuf), SEEK_CUR);
	if (ssts == EOF)
	    return EOF;
    } while (ismodifierfatc(cdbuf));
    return ssts;
}

int fputfatc(int fatcode, FILE * fatstream)
{
    int csts;
    if (fatcode < 0x80) {
	return fputc((fatcode & 0x7f) | 0x00, fatstream);
    }
    if (fatcode < 0x0800) {
	csts = fputc((fatcode >> 0x6 & 0x1f) | 0xc0, fatstream);
	if (csts == EOF)
	    return EOF;
	return fputc((fatcode & 0x3f) | 0x80, fatstream);
    }
    if (fatcode < 0x010000) {
	csts = fputc((fatcode >> 0xc & 0x0f) | 0xe0, fatstream);
	if (csts == EOF)
	    return EOF;
	csts = fputc((fatcode >> 0x6 & 0x3f) | 0x80, fatstream);
	if (csts == EOF)
	    return EOF;
	return fputc((fatcode & 0x3f) | 0x80, fatstream);
    }
    if (fatcode < 0x110000) {
	csts = fputc((fatcode >> 0x12 & 0x07) | 0xf0, fatstream);
	if (csts == EOF)
	    return EOF;
	csts = fputc((fatcode >> 0xc & 0x3f) | 0x80, fatstream);
	if (csts == EOF)
	    return EOF;
	csts = fputc((fatcode >> 6 & 0x3f) | 0x80, fatstream);
	return fputc((fatcode & 0x3f) | 0x80, fatstream);
    }
    return -2;
}
