$L
/* $A.c -- generated PIC C-code file for INTERCAL program $A.i */

/* This code is explicitly *not* GPLed.  Use, abuse, and redistribute freely */

$M
$E

$K

#define ICKABSTAINED(d) abstain##d
#define ICKSTASH(a,b,c,d) c##stash[b]=c[b]
#define ICKRETRIEVE(a,b,c,d,e) a[b]=a##stash[b]
#define ICKIGNORE(a,b,c) ignore##c##b

ICK_INT16 skipto=0;
$O

$C

$D
$P
int main(void)
{
  pickinit();
      /* degenerated code */
 ick_restart: 
 top:
    switch(skipto)
    {
    case 0:
      $G
	}
    
#ifdef YUK
    if(yukloop) goto ick_restart; 
#endif
    lose(E633, $J, (char *)0);

    $H
    
    return 0;
}
$Q

/* Generated code for $A.i ends here */
