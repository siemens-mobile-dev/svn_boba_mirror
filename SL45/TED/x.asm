	PUBLIC	_bl_us
_bl_us	PROC	FAR
; Locals:
; 
; Statics:
; 
; CSEs:
;     pos = R12
; 
; main.c    469	 if (!pos) return(0);
; main.c    470	 while(ustk[--pos]>1) if (!pos) return(0); //Ищем конец предыдущей строки
_90:
	CMP	R12,#00h
	JMPR	cc_Z,_92
	SUB	R12,#01h
	EXTP	#PAG _ustk,#01h
	MOVB	RL1,[R12+#POF _ustk]
	CMPB	RL1,#01h
	JMPR	cc_UGT,_90
; main.c    471	 if (!pos) return(0);
; main.c    472	 //Теперь pos указывает на eol
; main.c    473	 //Ищем теперь сл. eol
; main.c    474	 while(ustk[--pos]>1) if (!pos) return(0);
_91:
	CMP	R12,#00h
	JMPR	cc_NZ,_93
_92:
	MOV	R4,#00h
	RETS
_93:
	SUB	R12,#01h
	EXTP	#PAG _ustk,#01h
	MOVB	RL1,[R12+#POF _ustk]
	CMPB	RL1,#01h
	JMPR	cc_UGT,_91
; main.c    475	 pos++;
	ADD	R12,#01h
; main.c    476	 return(pos);
	MOV	R4,R12
; main.c    477	}
_94:
	RETS
_bl_us	ENDP
