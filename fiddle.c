/*
 * fiddle.c -- functions that implement the five INTERCAL operators
 *
 * We link these to the compiler, too, in order to do constant folding
 */

#define BIT(n, q)	(((q) & (1 << (n))) != 0)

unsigned int mingle(r, s)
unsigned int r, s;
{
    return (
	(BIT(15, r) << 31) |
	(BIT(15, s) << 30) |
	(BIT(14, r) << 29) |
	(BIT(14, s) << 28) |
	(BIT(13, r) << 27) |
	(BIT(13, s) << 26) |
	(BIT(12, r) << 25) |
	(BIT(12, s) << 24) |
	(BIT(11, r) << 23) |
	(BIT(11, s) << 22) |
	(BIT(10, r) << 21) |
	(BIT(10, s) << 20) |
	(BIT( 9, r) << 19) |
	(BIT( 9, s) << 18) |
	(BIT( 8, r) << 17) |
	(BIT( 8, s) << 16) |
	(BIT( 7, r) << 15) |
	(BIT( 7, s) << 14) |
	(BIT( 6, r) << 13) |
	(BIT( 6, s) << 12) |
	(BIT( 5, r) << 11) |
	(BIT( 5, s) << 10) |
	(BIT( 4, r) <<  9) |
	(BIT( 4, s) <<  8) |
	(BIT( 3, r) <<  7) |
	(BIT( 3, s) <<  6) |
	(BIT( 2, r) <<  5) |
	(BIT( 2, s) <<  4) |
	(BIT( 1, r) <<  3) |
	(BIT( 1, s) <<  2) |
	(BIT( 0, r) <<  1) |
	(BIT( 0, s) <<  0));
}

unsigned int select(r, s)
unsigned int r, s;
{
    int	i, n = 0, result = 0;

    for (i = 0; i < 32; i++)
	if (BIT(i, s))
	    result |= ((1 << n++) & r);
    return(result);
}

unsigned int and16(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x8000;

    return(m & n);
}

unsigned int or16(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x8000;

    return(m | n);
}

unsigned int xor16(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x8000;

    return(m ^ n);
}

unsigned int and32(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x80000000;

    return(m & n);
}

unsigned int or32(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x80000000;

    return(m | n);
}

unsigned int xor32(n)
unsigned int n;
{
    unsigned int m = (n >> 1);

    if (n & 1)
	m |= 0x80000000;

    return(m ^ n);
}

/* fiddle.c */
