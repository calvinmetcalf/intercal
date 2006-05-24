/* feh.h -- compilation functions used by perpetrate.c and ick.y */

extern node *newnode(void);
extern node *cons(int type, node *car, node *cdr);
extern unsigned int intern(int type, int index);
extern void checklabel(int label);
extern void treset(void);
extern tuple *newtuple(void);
extern void typecast(node *np);
extern void codecheck(void);
extern void optimize(node *np);
extern char *nameof(int value, assoc table[]);
extern void emit(tuple *tn, FILE *fp);
extern void emittextlines(FILE *fp);
extern int  comefromsearch(tuple *tn, unsigned int index); /* AIS */
extern void explexpr(node* np, FILE* fp); /* AIS */
extern void optimizef(); /* AIS */

extern char *enablers[MAXTYPES];
extern assoc vartypes[];

/* feh.h ends here */
