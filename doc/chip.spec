

























          The Intercal Machine Machine Language Specification File
                       (c) 1990 Christian Brunschen,
                         a. k. a. d89cb@efd.lth.se

This file contains the specifications for the Machine / Assembly Language
used by the Intercal Machine Microprocesor, which closely follows the
definition of the Intercal Compiler Language as defined by Woods/Lyon
in 1972. It also contains ideas as to the actual implementation,
hard- or softwarewise, of the Processor Instruction Set in an actual
environment.

It should be noted that the Intercal Machine Microprocessor is the first
actual RISC processor (RISC being, of course, the acronym for
Ridiculous Instruction Set Computer).

Overview of processor capabilities / requirements

Schematic overview of processor I/O busses:

                     +----------------\   /--------------+
	            _|		       \_/             	 |_
	D     +--- |_| D0             		     A04 |_| ---+
        a     |     _|                		         |_	|
	t     |    |_| D1             		     A05 |_|	|
	a     |     _|                		         |_	|
	      |    |_| D2             		     A06 |_|	|
	b     |     _|                		         |_	|
	u     |    |_| D3             		     A07 |_|	|
	s  ---+     _|                		         |_	|
	      |    |_| D4             		     A08 |_|	|
	8     |     _|                		         |_	|    A
	      |    |_| D5             		     A09 |_|	|    d
	b     |     _|                		         |_	|    d
	i     |    |_| D6             		     A10 |_|	|    r
	t     |     _|                		         |_	|    e
	s     +--- |_| D7             		     A11 |_|	|    s
	            _|                		         |_	|    s
                   |_| DACK                          A12 |_|    |
                    _|                                   |_     |
	           |_| GND            		     A13 |_|	|
		    _| _____          		         |_	|    b
	           |_| RESET          		     A14 |_|	+--- u
		    _|     ____       		         |_	|    s
		   |_| MEM/WRLD       		     A15 |_|	|
		    _|   _            		         |_	|    2
		   |_| R/W            		     A16 |_|	|    4
		    _|                		         |_	|
		   |_| CLK            		     A17 |_|	|    b
		    _|	              		         |_	|    i
	           |_| VCC            		     A18 |_|	|    t
	            _|                		         |_	|    s
	           |_| ERROR          		     A19 |_|	|
	            _|                		         |_	|
	      +--- |_| A00            		     A20 |_|	|
	      |     _|                		         |_	|
	      |    |_| A01            		     A21 |_|	|
	      |     _|                		         |_	|
	      |    |_| A02            		     A22 |_|	|
	      |     _|                		         |_	|
	      |    |_| A03            		     A23 |_|	|
	      |      |                                   |	|
	      |      +-----------------------------------+	|
	      |                                                 |
	      |                                                 |
	      +-------------------------------------------------+

The only signals that might require some explanation are probably
  _
R/W :	When this signal is low, The processor is reading data from the databus.
	When it is high, the processor is driving the the databus.
_____
RESET :	When this signal goes low, the processor is reset, and starts reading
	incoming data from the databus, one byte each clock cycle, until the
	line goes high again, at which time the processor will resume execution
	of the program at memory location (Hex) 000000 (24 bits, remeber ?)
    ____
MEM/WRLD :                                                                   _
	Indicates the `target' or `source' (depending on the status of the R/W
	line) of the data bus, sort of like an additional address bit. If this
	line is low, the processor is communicating with the outside world. If
	it is high, the processor is talking to its memory.

DACK :	Data ACKnowledge. Used when the processor communicates with the
	outside world. Two cases:
	a) Processor Talking
	   After the processor put the data on the databus, it will wait
	   until the DACK has been raised, and then dropped again. Then
	   it will put the next byte of data (if any) on the bus, or
	   proceed to the next instruction.
	b) Processor listening
	   The same procedure as above, but in the other direction :
	   After the processor has read one byte of data, it quickly raises &
	   drops the line, and then expects the next data item to be present.

ERROR :	Indicates the fact that an error condition has occurred somewhere
	in the program. The error number is present on the data bus.
	After the error number has ben read, the address of the next statement
	to be executed, had the eror not occurred, can be read in three
	consecutive bytes from the databus.

Processor Memory Management

Since the Intercal Language provides quite a few interesting ways to manipulate
arrays, stash and retrieve data, etc, all of which operations require
manipulations of memory, and it was the intention of this implementation to
provide an instruction set which immediately makes the average Intercal
programmer feel right at home, quite a few very complex instructions have been
provided.

Intercal Machine Language Reference Part

Summary of instructions:

ABG			ABstain from Gerund
ABL			ABstain from Label
ALA			ALlocate Aray
BAA			Begin Array Allocation
BIA			Begin Indexing Array
BIN			BINary operator
CAL			CALculate
DLD			Data LoaD
EAA			End Array Allocation
FRG			FoRGet
IDA			InDex Array
IGN			IGNore
INS			INitiate Statement
NEX			NEXt
OST			Open STorage
PCT			PerCenTage qualifier
POP			POP
PPA			PoP Array pointers
PSA			PuSh Array pointers
PSH			PuSH
RDO			ReaD Out
REG			Reinstate Gerund
REL			Reinstate Label
RMB			ReMemBer
RSM			ReSuMe
RTV			ReTrieVe
STS			STaSh
UNO			UNary Operator
WRI			WRite In

General Format:

 i i i i i p p s  <optional arguments>

where

 i = instruction field
 p = private field
 s = size field

ABstain from Label

ABL Label

  0 1 1 1 1 0 0 0

Sets the abstain bit of instruction at Label

ABstain from Gerund

ABG mnm

  0 1 1 1 1 1 0 0

Sets the abstain bit of all statements whosde main instruction is mnm

Allocate Array


ALA #num
ALA .var
ALA :var
ALA ,
ALA ;
ALA RES


  0 0 0 1 1 p p s

where p p specifies the source,
 0 0 : immediate data <16 bits>
 0 1 : Variable <variable specifier, 16 bits>
 1 0 : Array, as pointed to by the Read Array Pointer
 1 1 : the contents of the RES register

Takes the ALA argument, multiplies it with the current contents of the
Temproary Register, and puts the result in the Temporary Register.

Begin Allocating Array

BAA ,var
BAA ;var


  0 0 0 1 0 0 0 s
  array specifier, 16 bits
  jump offset, 16 bits

Deallocates the memory block currently allocated to array `var', 
opens the Arrray Entry for array `var' for writing, and puts a pointer
to the Array Entry in the Wite Array Data pointer register. If the array
is being ignored, none of these things actually happen -- instead, the
jump offset is added to the program counter.

Begin Indexing Array

BIA R ,var
BIA W ,var
BIA R ;var
BIA W ;var


  0 0 1 0 1 0 t s
  array specifier, 16 bits

where t = type, 0 = Read or 1 = Write

Sets the Read Array Data pointer to the address of the first array element,
and the Read Array Dimension pointer to the first Dimension entry in the
Array Table, thus preparing for further indexing.

BINary operator

BIN ~		Select
BIN c/		Interleave,
BIN $		aka Mingle

  0 1 1 0 0 t 0 0

where t is the operation specifier:
  0 : Select
  1 : Interleave / Mingle

The operation is performed on the OP1 and OP2 registers, and the result is
placed in the RES register.

CALculate

CAL .
CAL :
CAL ,
CAL ;


  0 1 0 1 0 0 0 s

Puts the contents of the RES register in the memory location pointed to by
the Write Array Data pointer

Data LoaD

DLD t #num
DLD t .var
DLD t :var
DLD t ,
DLD t ;
DLD t RES


  0 1 0 0 t p p s

where t = target register, 0 = OP1 or 1 = OP2
      p p = source :
 0 0 : Immediate data <16 bits>
 0 1 : Variable <specifier, 16 bits>
 1 0 : Array item (as indexed by the Read Array Data pointer)
 1 1 : RES register

Loads the target register with the contents of the source

End Allocating Array

EAA


  0 0 1 0 0 0 0 0

Allocates a chunk of memory to the size of

FoRGet

FRG #num
FRG .var
FRG :var
FRG ,
FRG ;
FRG RES


  1 0 0 0 0 p p s

where p p = source :
 0 0 : Immediate <16 bits>
 0 1 : variable <specifier, 16 bits>
 1 0 : array (as indexed by the Read Array Data pointer)
 1 1 : RES register

Pops source number of addresses from the stack, discarding them.

InDex Array
IDA R #num
IDA W #num
IDA R .var
IDA W .var
IDA R :var
IDA W :var
IDA R RES
IDA W RES


  0 0 1 1 t p p s

where t = type, 0 = Read or 1 = Write
      p p = source :
 0 0 : Immediate Data <16 bits>
 0 1 : variable <specifier, 16 bits>
 1 0 : ILLEGAL
 1 1 : RES register

Takes the Array Dimension (i.e, what the Array Dimension Pointer points to),
adds (#num or RES) instances of it to the Array Data Pointer, and adds 2 to the
Array Dimension Pointer.

IGNore

IGN .var
IGN :var
IGN ,var
IGN ;var

  1 0 1 0 1 0 0 s
  variable specifier, 16 bits

Sets the ignore flag of the variable specified

INitiate Statement

INS mnm Label

where `mnm' is one of the following 3-character mnemonics:

ABS             ABStain
REI             REInstate
CAL             CALculate
NEX             NEXt
FRG             FoRGet
RES             RESume
STS             STaSh
RTV             ReTrieVe
IGN             IGNore
RMB             ReMemBer
WRI             WRite In
RDO             ReaD Out


  1 1 1 p p p p p
  2 bytes :
  a o o o o o o o   o o o o o o o o

where p p p p p = major instruction opcode

where a is the abstain bit of the instruction : set if this instruction
is to be abstained from. The remaining 15 bits are the jump offset, which will
be added to the program counter should the instruction be abstained from.
It is presumed that no statement will result in code exceeding 32.767 bytes.

NEXt

NEX Label

  0 1 1 0 1 0 0 0
  Target Address, 24 bits

Pushes the address of the next instruction on the return stack and puts the
Target addres in the Program counter

Open STorage

OST .var Label
OST :var Label
OST ,var Label
OST ;var Label


  0 0 0 0 1 0 0 s
  variable/array specifier, 16 bits
  jump offset, 8 bits


Trys to open the specified variable for writing.
If it being ignored (i.e, its ignore flag is set) the jump offset is
added to the program counter.
If it is an array, the Write Array Data pointer is set to the address
of the first element of the array, and the Write Array Dimension pointer
to the first dimension entry in the Array Table.
If it is a Variable, the Write Array Data pointer is set to the address
of the Variable.

PerCenTage qualifier

PCT #num Label


  0 0 0 0 0 0 0 0
  0 c c c c c c c
  Jump offset, 16 bits

where c c c c c c c is a 7 bit binary number between 0 and 100

Gets a random number in the range 0 .. 99, compares it to the percent chance
and acts on the result:
- if the random number is less than the percent chance, nothing is done
- if it is equal or greater, the jump offset is added to the program counter

POP

POP


  1 1 0 0 0 0 1 0

Gets the top item on the stack and puts it in the RES register -- 32 bits

PoP Array pointers

PPA

  1 1 0 0 1 0 1 0

Retrieves the contents of the Read Array Data pointer from the stack

PuSh Array pointers

PSA


  1 1 0 0 1 0 0 0

Pushes the contents of the Read Array Data pointer to an internal stack

PuSH

PSH


  1 1 0 0 0 0 0 0

Pushes the RES register to the stack -- 32 bits

ReaD Out

RDO #num
RDO .var
RDO :var
RDO ,
RDO ;
RDO RES


  1 0 1 1 1 p p s

where p p = source:
 0 0 : Immediate <16 bits>
 0 1 : Variable <specifier, 16 bits>
 1 0 : Array (as pointed to by the Read Array Data pointer)
 1 1 : RES register

Writes 2 or 4 bytes (16 or 32 bits) on the data bus

REinstate Gerund

REG mnm

  0 1 1 1 0 1 0 0
  operation specifier, 8 bits

In this form, the hole program will be searched to find instructions of the
specified type, and their abstain bits will be reset.

REinstate Label

REL Label

  0 1 1 1 0 0 0 0
  Label, 24 bits

In this form, the instruction at the address of Label will have its abstain
bit set/reset.

ReMemBer

RMB .var
RMB :var
RMB ,var
RMB ;var

  1 0 1 0 0 0 0 s
  variable specifier, 16 bits

Clears the specified variable's ignore flag

ReSuMe

RSM #num
RSM .var
RSM :var
RSM ,
RSM ;
RSM RES

  1 0 0 0 1 p p s

where p p = source :
 0 0 : Immediate <16 bits>
 0 1 : variable <specifier, 16 bits>
 1 0 : array (as indexed by the Read Array Data pointer)
 1 1 : RES register

Pops source number of addresses from the stack.
If there are less than `source' items on the stack, the procesor is halted
and an error signaled
If no error occurs, the last popped address is NOT discarded, but instead
placed in the Program counter.

ReTrieVe

RTV .var
RTV :var
RTV ,var
RTV ;var

  1 0 0 1 1 0 0 s
  Variable/Array Specifier, 16 bits

Retrieves the latest stashed instance of the variable or array specified

STaSh

STS .var
STS :var
STS ,var
STS ;var

  1 0 0 1 0 0 0 s
  Variable/Array specifier, 16 bits

Stashes the variable or array specified

UNary Operator

UNO &		AND
UNO V		OR
UNO V-		XOR
UNO ?		XOR

  0 1 0 1 1 t t s

where t is the operation specifier:
  00 : AND
  01 : OR
  10 : XOR
  11 : Illegal

The specified operation is performed on the RESult register, leaving it there.

WRite In

WRI .
WRI :
WRI ,
WRI ;


  1 0 1 1 0 0 0 s

Reads 2 or 4 bytes (16 or 32 bits) from the data bus and puts them in
the variable pointed to by the Write Array Data pointer

