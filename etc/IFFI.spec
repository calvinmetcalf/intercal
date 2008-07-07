%fingerprint-spec 1.1
%fprint:IFFI
%url:http://example.com
%desc:INTERCAL-like Foreign Function Interface
%safe:false
%begin-instrs
#I	name	desc
A	create	CREATE a new INTERCAL instruction
C	comeFrom	In markmode COME FROM the top of stack
D	createData	Push information about a CREATEd instruction argument
F	forget	FORGET NEXT stack entries equal to top of stack
G	varGet	Get the value of an INTERCAL scalar variable
L	label	Use top of stack as a line label for this point
M	marker	Marks points where the code can be entered from outside
N	next	Try to NEXT to the line labelled with the top of stack
R	resume	RESUME to the top-of-stackth NEXT stack entry
S	varSet	Set the value of an INTERCAL scalar variable
V	argSet	Assign to a CREATEd instruction argument
X	nextFrom	In markmode NEXT FROM the top of stack
Y	yield	Marks the end of initialisation
%end
