PROG    START   0
MAIN    JSUB    OLIN 	 
        J	END_ 	 
OLIN    STL	OLLA 	 
        LDT	#1   
        STT     OUTI 	 
OLCD    LDA     OUTI 	 
        COMP    #9  	 
        JGT     OLBK 	 
OLST    JSUB	ILIN 	 
        LDA	LF   	 
        JSUB	WCHR 	 
        LDA	CR   	 
        JSUB	WCHR 	 
OLUP    LDA     OUTI 	 
        ADD	#1   	 
        STA	OUTI 	 
        J	OLCD
ILIN    STL	ILLA 	 
        LDT	#0  
        STT	IN_I 	 
ILCD    LDA	IN_I 	 
        COMP	#9   	 
        JGT	ILBK 	 
INST    LDA	IN_I 	 
        J	APEN
APST    STL	APLA 	 
        JSUB	WINT 	 
        LDA	SP   	 
        JSUB	WCHR
        LDL	APLA 	 
        RSUB         	 
APEN    COMP	#0  
        JEQ	IF01 	 
        J	EI01 	 
IF01    LDA	OUTI 	 
        JSUB	APST 	 
        J	ILUP 	 
EI01    MUL	OUTI 	 
        JSUB	APST 	 
ILUP    LDA	IN_I 	 
        ADD	#1   	 
        STA	IN_I
        J	ILCD
OLBK    LDL	OLLA 	 
        RSUB
ILBK    LDL	ILLA 	 
        RSUB
WINT    STL	WILA 	 
        STA	RAWA 	 
        COMP	#10  	 
        JLT	IF02 	 
        J	EI02
IF02    ADD	ITOA 	 
        RMO	A,T  	 
        LDA	SP
        JSUB	WCHR 	 
        RMO	T,A
        JSUB	WCHR 	 
        J	ENWI 	 
EI02    DIV	#10  	 
        ADD	ITOA 	 
        JSUB	WCHR 	 
        SUB	ITOA 	 
        MUL	#10  	 
        RMO	A,T  	 
        LDA	RAWA 	 
        SUBR	T,A  	 
        ADD	ITOA 	 
        JSUB	WCHR 	 
ENWI    LDA	RAWA 	 
        LDL	WILA 	 
        RSUB         	 
WCHR    STL	WCLA 	 
WDLP    TD	DEV1 	 
        JEQ	WDLP
        WD	DEV1
        LDL	WCLA 	 
        RSUB         	 
OUTI    RESW    1    	 
IN_I    RESW    1    	 
RAWA    RESW    3    	 
OLLA    RESW    3    	 
ILLA    RESW    3    	 
WILA    RESW    3    	 
WCLA    RESW    3    	 
APLA    RESW    3    	 
DEV1    BYTE    X'F2'
LF      WORD    10
CR      WORD    13
SP      WORD    32
ITOA    WORD    48
END_	J	END_