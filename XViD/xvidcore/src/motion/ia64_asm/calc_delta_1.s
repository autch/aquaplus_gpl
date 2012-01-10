	;; 
	getf.sig ret0 = fmv
	add mpr[0] = mpr[0], mpr[1]
	add mpr[2] = mpr[2], mpr[3]
	add mpr[4] = mpr[4], mpr[5]
	add mpr[6] = mpr[6], mpr[7]
	;; 
	add mpr[0] = mpr[0], mpr[2]
	add mpr[4] = mpr[4], mpr[6]
	mov component[0] = dx
	mov component[1] = dy
	
	cmp.ne non0_2, p0 = 0, dx
	cmp.gt neg_2, p0 = 0, dx
	
	.pred.rel "mutex", p32, p36	//non0_0, neg_0
 
	cmp.ne non0_3, p0 = 0, dy
	cmp.gt neg_3, p0 = 0, dy
	;;
	.pred.rel "mutex", p33, p37	//non0_1, neg_1

	add iSAD = iSAD, ret0
	add mpr[8] = mpr[0], mpr[4]
(neg_2)	sub component[0] = 0, component[0]	//abs
(neg_3)	sub component[1] = 0, component[1]	//abs
	;; 
