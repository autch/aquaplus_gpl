	;; 
(fb)	mov iMinSAD = mpr[8]
(fb)	mov currX = backupX
(fb)	mov currY = backupY
	mov ret0 = 2 
(non0_0)	add sc[0] = iFcode, sc[0]
(non0_1)	add sc[1] = iFcode, sc[1]
	;;
(non0_0)	add ret0 = ret0, sc[0]
	;; 
(non0_1)	add ret0 = ret0, sc[1]
	;; 

	setf.sig fmv = ret0
	;; 
	xmpy.l fmv = fmv, fQuant
	;; 
