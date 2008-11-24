" Vim syntax file
" Language:	INTERCAL
" Maintainer:	Stefan O'Rear <stefanor@cox.net>
" Last Change:	2008 Nov 29
" Remark:	Use the Power of Vim to turn your INTERCAL programs into angry
"               fruit salad.
"
" How to use this:
" * Put levdes.vim (this file) under ~/.vim/syntax (or similar directory for
"   your system - usually C:\Program Files\Vim\vimfiles\syntax on Windows).
" * In your .vimrc, add this line: 
"     au BufRead,BufNewFile *.i set syntax=intercal
" Thereafter, any .i files you edit in (g)vim will use syntax highlighting.

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

if !exists("main_syntax")
  let main_syntax = 'ick'
endif


syn match ickIdentifier /DO\|PLEASE/

syn region ickComment start=/\(PLEASE\s*DO\s*\|PLEASE\s*\|DO\s*\|\)N\s*['O]\s*T/ end=/\(([ 	0-9]*[0-9])\s*\)\?\(DO\|PLEASE\)/me=s-1

syn match ickKeyword       /F\s*O\s*R\s*G\s*E\s*T/
syn match ickKeyword       /G\s*I\s*V\s*E\s*U\s*P/
syn match ickKeyword       /I\s*G\s*N\s*O\s*R\s*E/
syn match ickKeyword       /N\s*E\s*X\s*T/
syn match ickKeyword       /R\s*E\s*I\s*N\s*S\s*T\s*A\s*T\s*E/
syn match ickKeyword       /R\s*E\s*M\s*E\s*M\s*B\s*E\s*R/
syn match ickKeyword       /R\s*E\s*S\s*U\s*M\s*E/
syn match ickKeyword       /R\s*E\s*T\s*R\s*I\s*E\s*V\s*E/
syn match ickKeyword       /S\s*T\s*A\s*S\s*H/
syn match ickKeyword       /W\s*R\s*I\s*T\s*E\s*I\s*N/
syn match ickKeywordAIS    /A\s*G\s*A\s*I\s*N/
syn match ickKeywordAIS    /G\s*O\s*A\s*H\s*E\s*A\s*D/
syn match ickKeywordAIS    /G\s*O\s*B\s*A\s*C\s*K/
syn match ickKeywordAIS    /M\s*A\s*Y\s*B\s*E/
syn match ickKeywordAIS    /O\s*N\s*C\s*E/
syn match ickKeywordAIS    /P\s*I\s*N/
syn match ickKeywordAIS    /T\s*R\s*Y\s*A\s*G\s*A\s*I\s*N/
syn match ickKeywordAISGrn /G\s*O\s*I\s*N\s*G\s*A\s*H\s*E\s*A\s*D/
syn match ickKeywordAISGrn /G\s*O\s*I\s*N\s*G\s*B\s*A\s*C\s*K/
syn match ickKeywordAISGrn /N\s*E\s*X\s*T\s*I\s*N\s*G\s*F\s*R\s*O\s*M/
syn match ickKeywordAISGrn /P\s*I\s*N\s*N\s*I\s*N\s*G/
syn match ickKeywordAISGrn /T\s*R\s*Y\s*I\s*N\s*G\s*A\s*G\s*A\s*I\s*N/
syn match ickKeywordCLC    /C\s*R\s*E\s*A\s*T\s*E/
syn match ickKeywordCLC    /N\s*E\s*X\s*T\s*F\s*R\s*O\s*M/
syn match ickKeywordCLCExp /W\s*H\s*I\s*L\s*E/
syn match ickKeywordCLCGrn /C\s*O\s*M\s*M\s*E\s*N\s*T/
syn match ickKeywordCLCGrn /C\s*O\s*M\s*M\s*E\s*N\s*T\s*I\s*N\s*G/
syn match ickKeywordCLCGrn /C\s*O\s*M\s*M\s*E\s*N\s*T\s*S/
syn match ickKeywordCLCGrn /C\s*R\s*E\s*A\s*T\s*I\s*N\s*G/
syn match ickKeywordCLCGrn /C\s*R\s*E\s*A\s*T\s*I\s*O\s*N/
syn match ickKeywordCLCGrn /L\s*O\s*O\s*P\s*I\s*N\s*G/
syn match ickKeywordCLCGrn /W\s*H\s*I\s*L\s*I\s*N\s*G/
syn match ickKeywordESR    /C\s*O\s*M\s*E\s*F\s*R\s*O\s*M/
syn match ickKeywordESRGrn /C\s*O\s*M\s*I\s*N\s*G\s*F\s*R\s*O\s*M/
syn match ickKeywordExp    /B\s*Y/
syn match ickKeywordExp    /S\s*U\s*B/
syn match ickKeywordGrn    /R\s*E\s*M\s*E\s*M\s*B\s*E\s*R\s*I\s*N\s*G/
syn match ickKeywordGrn    /R\s*E\s*I\s*N\s*S\s*T\s*A\s*T\s*I\s*N\s*G/
syn match ickKeyword       /A\s*B\s*S\s*T\s*A\s*I\s*N\s*F\s*R\s*O\s*M/
syn match ickKeywordGrn    /A\s*B\s*S\s*T\s*A\s*I\s*N\s*I\s*N\s*G/
syn match ickKeywordGrn    /C\s*A\s*L\s*C\s*U\s*L\s*A\s*T\s*I\s*N\s*G/
syn match ickKeywordGrn    /F\s*O\s*R\s*G\s*E\s*T\s*T\s*I\s*N\s*G/
syn match ickKeywordGrn    /I\s*G\s*N\s*O\s*R\s*I\s*N\s*G/
syn match ickKeywordGrn    /R\s*E\s*A\s*D\s*I\s*N\s*G\s*O\s*U\s*T/
syn match ickKeyword       /R\s*E\s*A\s*D\s\+O\s*U\s*T/
syn match ickKeywordGrn    /N\s*E\s*X\s*T\s*I\s*N\s*G/
syn match ickKeywordGrn    /R\s*E\s*S\s*U\s*M\s*I\s*N\s*G/
syn match ickKeywordGrn    /R\s*E\s*T\s*R\s*I\s*E\s*V\s*I\s*N\s*G/
syn match ickKeywordGrn    /S\s*T\s*A\s*S\s*H\s*I\s*N\s*G/
syn match ickKeywordGrn    /W\s*R\s*I\s*T\s*I\s*N\s*G\s*I\s*N/

syn match ickKeywordExp    /<\s*-/

syn match ickVariable /[!.:]\s*\(\s\|\d\)*\d/ contains=ickUnaryVar
syn match ickArrayVar /[,;][0-9 	]*[0-9]/
syn match ickConstant /#[0-9 	]*[0-9]/
syn match ickUnaryVar /[&|?^]\|[2-5]\?@/ contained

syn match ickLabel    /([0-9 	]*[0-9][9-9 	]*)/

hi link ickLabel      Tag
hi link ickUnaryVar   Normal
hi link ickVariable   Identifier
hi link ickArrayVar   Special
hi link ickConstant   Constant
hi link ickIdentifier PreProc
hi link ickComment    Comment

hi link ickKeywordGrn Type
hi link ickKeywordExp Operator
hi link ickKeyword    Keyword

hi link ickKeywordAISGrn ickKeywordGrn
hi link ickKeywordAISExp ickKeywordExp
hi link ickKeywordAIS    ickKeyword

hi link ickKeywordCLCGrn ickKeywordGrn
hi link ickKeywordCLCExp ickKeywordExp
hi link ickKeywordCLC    ickKeyword

hi link ickKeywordESRGrn ickKeywordGrn
hi link ickKeywordESRExp ickKeywordExp
hi link ickKeywordESR    ickKeyword

syn sync minlines=45

let b:current_syntax="intercal"
