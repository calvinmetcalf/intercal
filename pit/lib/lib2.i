(2010)  PLEASE ABSTAIN FROM (2004)
(2000)  PLEASE STASH .2
        DO .2 <- #1
        DO (2001) NEXT
(2001)  DO FORGET #1
        DO .1 <- '?.1$.2'~'#0$#65535'
        DO (2002) NEXT
        DO .2 <- !2$#0'~'#32767$#1'
        DO (2001) NEXT
(2003)  PLEASE RESUME "?!1~.2'$#1"~#3
(2002)  DO (2003) NEXT
        PLEASE RETRIEVE .2
(2004)	PLEASE RESUME #2
	DO REINSTATE (2004)
	PLEASE RESUME '?"!1~.1'~#1"$#2'~#6
(2020)  PLEASE STASH .2 + .3
	DO (1021) NEXT
(2030)	DO STASH .1 + .5
	DO .3 <- #0
	DO .5 <- '?"!2~.2'~#1"$#1'~#3
	PLEASE DO (2031) NEXT
	DO .4 <- #1
	PLEASE DO (2033) NEXT
(2033)	DO FORGET #1
	DO .5 <- '?".2~#32768"$#2'~#3
	DO (2032) NEXT
	DO .2 <- !2$#0'~'#32767$#1'
	PLEASE DO .4 <- !4$#0'~'#32767$#1'
	DO (2033) NEXT
(2032)	DO (1001) NEXT
(2036)	DO FORGET #1
        DO .5 <- '?.1$.2'~'#0$#65535'
        DO .5 <- '?"'&"!2~.5'~'"?'?.5~.5'$#32768"~"#0$#65535"'"$
                 ".5~.5"'~#1"$#2'~#3
	DO (2034) NEXT
	DO .5 <- .3
	DO (1010) NEXT
	PLEASE DO .1 <- .3
        DO .3 <- 'V.4$.5'~'#0$#65535'
	DO (2035) NEXT
(2034)	PLEASE DO (1001) NEXT
(2035)	DO FORGET #1
	DO .5 <- "?'.4~#1'$#2"~#3
	DO (2031) NEXT
	DO .2 <- .2~#65534
	DO .4 <- .4~#65534
	PLEASE DO (2036) NEXT
(2031)	DO (1001) NEXT
	PLEASE DO .4 <- .1
	DO RETRIEVE .1 + .5
	PLEASE RESUME #2
