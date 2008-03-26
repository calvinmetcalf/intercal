/* fiddle.h -- functions implementing intercal's operators */

/*@-exportlocal@*/ /* these are used, just in generated code */
extern unsigned int ick_mingle(register unsigned int r, register unsigned int s);
extern unsigned int ick_iselect(register unsigned int r, register unsigned int s);
extern unsigned int ick_and16(unsigned int n), ick_or16(unsigned int n),
                    ick_xor16(unsigned int n), ick_fin16(unsigned int n);
extern unsigned int ick_whirl16(unsigned int p, unsigned int n);
extern unsigned int ick_and32(unsigned int n), ick_or32(unsigned int n),
                    ick_xor32(unsigned int n), ick_fin32(unsigned int n);
extern unsigned int ick_whirl32(unsigned int p, unsigned int n);
/* AIS: Reversed operators */
extern unsigned int ick_rev_and16(unsigned int n), ick_rev_or16(unsigned int n),
                    ick_rev_xor16(unsigned int n), ick_rev_fin16(unsigned int n);
extern unsigned int ick_rev_whirl16(unsigned int p, unsigned int n);
extern unsigned int ick_rev_and32(unsigned int n), ick_rev_or32(unsigned int n),
                    ick_rev_xor32(unsigned int n), ick_rev_fin32(unsigned int n);
extern unsigned int ick_rev_whirl32(unsigned int p, unsigned int n);
/* AIS: Optimizer helper operators */
extern unsigned int ick_xselx(unsigned int x);
extern unsigned int ick_setbitcount(unsigned int x);
extern unsigned int ick_smudgeleft(unsigned int x);
extern unsigned int ick_smudgeright(unsigned int x);
/*@=exportlocal@*/
/* fiddle.h ends here */
