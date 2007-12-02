/* fiddle.h -- functions implementing intercal's operators */

extern unsigned int mingle(register unsigned int r, register unsigned int s);
extern unsigned int iselect(register unsigned int r, register unsigned int s);
extern unsigned int and16(unsigned int n), or16(unsigned int n),
                    xor16(unsigned int n), fin16(unsigned int n);
extern unsigned int whirl16(unsigned int p, unsigned int n);
extern unsigned int and32(unsigned int n), or32(unsigned int n),
                    xor32(unsigned int n), fin32(unsigned int n);
extern unsigned int whirl32(unsigned int p, unsigned int n);
/* AIS: Reversed operators */
extern unsigned int rev_and16(unsigned int n), rev_or16(unsigned int n),
                    rev_xor16(unsigned int n), rev_fin16(unsigned int n);
extern unsigned int rev_whirl16(unsigned int p, unsigned int n);
extern unsigned int rev_and32(unsigned int n), rev_or32(unsigned int n),
                    rev_xor32(unsigned int n), rev_fin32(unsigned int n);
extern unsigned int rev_whirl32(unsigned int p, unsigned int n);
/* AIS: Optimizer helper operators */
extern unsigned int xselx(unsigned int x);
extern unsigned int setbitcount(unsigned int x);
extern unsigned int smudgeleft(unsigned int x);
extern unsigned int smudgeright(unsigned int x);
/* fiddle.h ends here */
