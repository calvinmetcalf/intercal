/* feh.h -- compilation functions used by perpetrate.c and ick.y */

extern node *newnode(void);
extern node *cons(int type, node *car, node *cdr);
extern unsigned int intern(int type, unsigned int index);
extern void checklabel(int label);
extern void treset(void);
extern tuple *newtuple(void);
extern void typecast(node *np);
extern void codecheck(void);
extern void optimize(node *np);
extern char *nameof(int value, assoc table[]);
extern void emit(tuple *tn, FILE *fp);
extern void emittextlines(FILE *fp);
extern void emitslatproto(FILE *fp); /* AIS: emit prototypes for /-functions */
extern void emitslat(FILE* fp); /* AIS: emit bodies of /-functions */
extern int  comefromsearch(tuple *tn, unsigned int index); /* AIS */
extern void explexpr(node* np, FILE* fp); /* AIS */
extern void optimizef(); /* AIS */
extern void nodefree(node* np); /* AIS */
extern unsigned long varextern(unsigned long intern, int vartype); /* AIS */

extern char **enablers; /* AIS: so that there can be an element before the
			   first element of the array (UNKNOWN is element
			   -1, just to cause a bit more confusion) */
extern assoc vartypes[];

/* feh.h ends here */
