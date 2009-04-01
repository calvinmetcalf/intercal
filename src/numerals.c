/*
 * numeral.c -- internationalization support for INTERCAL input.
 *
 * After release 0.5, I wrote:
 *
 * 2. (ESR) Input format internationalization -- allow WRITE IN input digits in
 *   major languages such as Nahuatl, Tagalog, Sanskrit, and Basque.
 *
 * The twisted loons in the alt.folklore.computers crowd loved this
 * idea, and I actually got sent digit lists for Nahuatl, Tagalog,
 * Sanskrit, and Basque -- also, Kwakiutl, Georgian, Ojibwe. Albanian,
 * and Volap\"uk.  I've left out Albanian (didn't want to keep track
 * of the dipthong diacritical) and Ojibwe (no zero digit).  So:
 * Nahuatl, Tagalog, Sanskrit, Basque, Georgian, Kwakiutl, and
 * Volap\"uk are now supported in addition to English.
 *
 * As of release 0.18, Volap\"uk digits can be entered in either ick_traditional
 * Tex format, Latin-1, or UTF-8.  Latin is also now a supported language,
 * to fulfill the requirement for Ancient Roman localization.
 *
LICENSE TERMS
    Copyright (C) 1996 Eric S. Raymond

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

typedef struct
{
    /*@observer@*/ const char *name;
    const int	value;
}
numeral;

const numeral ick_numerals[] =
{
/* English */
    { "OH",		0 },
    { "ZERO",		0 },
    { "ONE",		1 },
    { "TWO",		2 },
    { "THREE",		3 },
    { "FOUR",		4 },
    { "FIVE",		5 },
    { "SIX",		6 },
    { "SEVEN",		7 },
    { "EIGHT",		8 },
    { "NINE",		9 },
    { "NINER",		9 },	/* For all you junior birdmen */
/* Sanskrit */
    { "SUTYA",		0 },	/* Retroflex s, pronounced halfway to sh */
    { "SHUTYA",		0 },
    { "EKA",		1 },
    { "DVI",		2 },
    { "TRI",		3 },
    { "CHATUR",		4 },
    { "PANCHAN",	5 },
    { "SHASH",		6 },
    { "SAPTAM",		7 },
    { "ASHTAN",		8 },
    { "NAVAN",		9 },
/* Basque */
    { "ZEROA",		0 },
    { "BAT",		1 },
    { "BI",		2 },
    { "HIRO",		3 },
    { "LAU",		4 },
    { "BORTZ",		5 },
    { "SEI",		6 },
    { "ZAZPI",		7 },
    { "ZORTZI",		8 },
    { "BEDERATZI",	9 },
/* Tagalog */
    { "WALA",		0 },
    { "ISA",		1 },
    { "DALAWA",		2 },
    { "TATLO",		3 },
    { "APAT",		4 },
    { "LIMA",		5 },
    { "ANIM",		6 },
    { "PITO",		7 },
    { "WALO",		8 },
    { "SIYAM",		9 },
/* Classical Nahuatl */
    { "AHTLE",		0 },	/* Actually `nothing'; no separate zero word is known */
    { "CE",		1 },
    { "OME",		2 },
    { "IEI",		3 },
    { "NAUI",		4 },
    { "NACUILI",	5 },
    { "CHIQUACE",	6 },
    { "CHICOME",	7 },
    { "CHICUE",		8 },
    { "CHICUNAUI",	9 },
/* Georgian */
    { "NULI",		0 },
    { "ERTI",		1 },
    { "ORI",		2 },
    { "SAMI",		3 },
    { "OTXI",		4 },
    { "XUTI",		5 },
    { "EKSVI",		6 },
    { "SHVIDI",		7 },
    { "RVA",		8 },
    { "CXRA",		9 },
/* Kwakiutl (technically, Kwak'wala) */
    { "KE'YOS",		0 },	/* Actually `nothing'; no separate zero word is known */
    { "'NEM",		1 },
    { "MAL'H",		2 },
    { "YUDEXW",		3 },
    { "MU",		4 },
    { "SEK'A",		5 },
    { "Q'ETL'A",	6 },
    { "ETLEBU",		7 },
    { "MALHGWENALH",	8 },
    { "'NA'NE'MA",	9 },
/* Volap\"uk */
    { "NOS",		0 },
    { "BAL",		1 },
    { "TEL",		2 },
    { "KIL",		3 },
    { "FOL",		4 },
    { "LUL",		5 },
    { "M\\\"AL",	6 },
    { "M\xC4L",		6 },	/* Latin-1 support */
    { "M\xA3\xA4L",	6 },	/* UTF-8 support */
    { "VEL",		7 },
    { "J\\\"OL",	8 },
    { "J\xD6L",		8 },	/* Latin-1 support */
    { "J\xA3\x96L",	8 },	/* UTF-8 support */
    { "Z\\\"UL",	9 },
    { "Z\xDCL",		9 },	/* Latin-1 support */
    { "Z\xA3\x9CL",	9 },	/* UTF-8 support */
/* Latin localization */
    { "NIL",		0 },
    { "NIHIL",		0 },
    { "UNUS",		1 },
    { "UNA",		1 },
    { "UNUM",		1 },
    { "DUO",		2 },
    { "DUAE",		2 },
    { "TRES",		3 },
    { "QUATTUOR",	4 },
    { "QUATUOR",	4 },
    { "QUINQUE",	5 },
    { "SEX",		6 },
    { "SEPTEM",		7 },
    { "OCTO",		8 },
    { "NOVEM",		9 },
};

/* numeral.c ends here */
