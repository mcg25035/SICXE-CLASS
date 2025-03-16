PROG    START   0
MAIN                 . main started
        JSUB    OLIN .     [outter_loop]
        J	    END  . end program 
.
OLIN    STL	    OLLA . outter loop initialization
        LDT	    #1   
        STT     OUTI .     let outterLoopIndex = 1
OLCD    LDA     OUTI . outter loop condition
        COMP    #9  .     outterLoopIndex <= 9
        JGT     OLBK . 
OLST                 . outter loop statements
        JSUB	ILIN .     [inner loop]
        LDA	    LF   .
        JSUB	WCHR .     print("\n")
        LDA	    CR   .
        JSUB	WCHR .     print("\r")  
OLUP    LDA     OUTI . outter updation
        ADD	    #1   .     outterLoopIndex += 1
        STA	    OUTI .
        J	    OLCD
.
ILIN    STL	    ILLA . inner loop initialization
        LDT	    #0  
        STT	    IN_I .     let innerLoopIndex = 0
ILCD    LDA	    IN_I . inner loop condition
        COMP	#9   .     innerLoopIndex <= 9
        JGT	    ILBK .
INST                 . inner loop statements
        LDA	    IN_I .      let a = innerLoopIndex
        J	    APEN
APST    STL	    APLA .      let printA = ()=>{
        JSUB	WINT .          writeInt(a)
        LDA	    SP   .          print(" ")
        JSUB	WCHR
        LDL	    APLA .
        RSUB         .   
APEN                 .      }
        COMP	#0  
        JEQ	    IF01 .      if (a == 0) {
        J	    EI01 .          
IF01    LDA	    OUTI .          a = outterLoopIndex
        JSUB	APST .          printA()
        J	    ILUP .          continue
        
EI01                 .      }
        MUL	    OUTI .      a *= outterLoopIndex
        JSUB	APST .      printA()
ILUP    LDA	    IN_I . inner updation
        ADD	    #1   .      innerLoopIndex += 1
        STA	    IN_I
        J	    ILCD
.
OLBK    LDL	    OLLA . outer loop break(exit)
        RSUB
ILBK    LDL	    ILLA . inner loop break(exit)
        RSUB


WINT    STL	    WILA . function writeInt() {
        STA	    RAWA .     let rawA = a
        COMP	#10  .     if (a < 10) {
        JLT	    IF02 .           
        J	    EI02
IF02    ADD	    ITOA .         a = itoa(a)
        RMO	    A,T  .         let tmp = a;
        LDA	    SP
        JSUB	WCHR .         print(" ")
        RMO	    T,A
        JSUB	WCHR .         print(a)     
        J	    ENWI .         return
EI02                 .     }     
        DIV	    #10  .     a //= 10
        ADD	    ITOA .     a = itoa(a) // tens digit
        JSUB	WCHR .     print(a) // print tens digit
        SUB	    ITOA .     a = atoi(a)
        MUL	    #10  .     a *= 10
        RMO	    A,T  .     let tmp = a // raw number with 0 at unit digit
        LDA	    RAWA .     let a = rawa
        SUBR	T,A  .     a -= tmp
        ADD	    ITOA .     a = itoa(a) // unit digit      
        JSUB	WCHR .     print(a) // print unit digit
ENWI                 . writeInt return program
        LDA	    RAWA . restore a
        LDL	    WILA . restore last executing address before writeInt
        RSUB         . exit writeInt
.                      }

WCHR    STL	    WCLA . function writeChar(){
WDLP    TD	    DEV1 .     print(a)
        JEQ	    WDLP
        WD	    DEV1
        LDL	    WCLA .     restore last executing address before print char
        RSUB         .     exit writeChar
.                      }

.
.
OUTI    RESW    1    . outer loop index
IN_I    RESW    1    . inner loop index
RAWA    RESW    3    . rawA for writeInt()
OLLA    RESW    3    . last executing address for outter loop
ILLA    RESW    3    . last executing address for inner loop
WILA    RESW    3    . last executing address for writeInt
WCLA    RESW    3    . last executing address for writeChar
APLA    RESW    3    . last executing address for anonymous function printA
DEV1    BYTE    X'F2'
LF      WORD    10
CR      WORD    13
SP      WORD    32
ITOA    WORD    48
.
.
END	J	END