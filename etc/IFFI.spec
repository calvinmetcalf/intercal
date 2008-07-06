%fingerprint-spec 1.1
%fprint:IFFI
%url:http://example.com
%desc:INTERCAL-like Foreign Function Interface
%safe:false
%begin-instrs
#I	name	desc
C	comeFrom	In markmode COME FROM the top of stack
F	forget	FORGET NEXT stack entries equal to top of stack
L	label	Use top of stack as a line label for this point
M	marker	Marks points where the code can be entered from outside
N	next	Try to NEXT to the line labelled with the top of stack
R	resume	RESUME to the top-of-stackth NEXT stack entry
X	nextFrom	In markmode NEXT FROM the top of stack
%end
