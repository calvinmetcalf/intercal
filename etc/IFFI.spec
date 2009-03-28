%fingerprint-spec 1.4
%fprint:IFFI
%url:http://example.com
%desc:INTERCAL-like Foreign Function Interface
%safe:false
%begin-instrs
#I	name	desc
A	create	CREATE a new INTERCAL instruction
C	come_from	In markmode COME FROM the top of stack
D	create_data	Push information about a CREATEd instruction argument
F	forget	FORGET NEXT stack entries equal to top of stack
G	var_get	Get the value of an INTERCAL scalar variable
L	label	Use top of stack as a line label for this point
M	marker	Marks points where the code can be entered from outside
N	next	Try to NEXT to the line labelled with the top of stack
R	resume	RESUME to the top-of-stackth NEXT stack entry
S	var_set	Set the value of an INTERCAL scalar variable
V	arg_set	Assign to a CREATEd instruction argument
X	next_from	In markmode NEXT FROM the top of stack
Y	yield	Marks the end of initialisation
%end
