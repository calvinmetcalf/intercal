/* feh.h -- compilation functions used by perpetrate.c and ick.y */

/*@partial@*/ extern node *newnode(void);
/*@partial@*/ extern node *cons(int type, /*@null@*/ /*@keep@*/ node *car, /*@null@*/ /*@keep@*/ node *cdr);
extern unsigned long intern(int type, unsigned long index);
extern void checklabel(int label);
extern void treset(void);
/*@out@*/ /*@dependent@*/ extern tuple *newtuple(void);
extern void tupleswap(int,int); /* AIS */
extern void ppinit(int); /* AIS */
extern void typecast(node *np);
extern void codecheck(void);
extern void optimize(node *np);
/*@observer@*/ /*@null@*/ extern const char *nameof(int value, const assoc table[]);
extern void emit(tuple *tn, FILE *fp);
extern void emittextlines(FILE *fp);
extern void emitslatproto(FILE *fp); /* AIS: emit prototypes for /-functions */
extern void emitslat(FILE* fp); /* AIS: emit bodies of /-functions */
extern int  comefromsearch(tuple *tn, unsigned int index); /* AIS */
extern void explexpr(node* np, FILE* fp); /* AIS */
extern void prexpr(node *np, FILE* fp, int freenode); /* AIS: destaticed */
extern void checknodeactbits(node *np); /* AIS */
extern void optimizef(void); /* AIS */
extern void nodefree(/*@keep@*/ /*@null@*/ node* np); /* AIS */
extern unsigned long varextern(unsigned long intern, int vartype); /* AIS */
extern node *nodecopy(const node*); /* AIS */
extern bool nodessame(/*@observer@*/ const node*, /*@observer@*/ const node*); /* AIS */

extern node *optdebugnode; /* AIS */
extern const char **enablers; /* AIS: so that there can be an element before the
			         ick_first element of the ick_array (UNKNOWN is element
			         -1, just to cause a bit more confusion) */
extern const assoc vartypes[];

extern bool useprintflow; /* AIS */

/* feh.h ends here */
