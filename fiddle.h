/* fiddle.h -- functions implementing intercal's operators */

extern unsigned int mingle(register unsigned int r, register unsigned int s);
extern unsigned int iselect(register unsigned int r, register unsigned int s);
extern unsigned int and16(unsigned int n), or16(unsigned int n),
                    xor16(unsigned int n), fin16(unsigned int n);
extern unsigned int whirl16(unsigned int p, unsigned int n);
extern unsigned int and32(unsigned int n), or32(unsigned int n),
                    xor32(unsigned int n), fin32(unsigned int n);
extern unsigned int whirl32(unsigned int p, unsigned int n);

/* fiddle.h ends here */
