(non0_2)	mov sc[0] = 1
(non0_3)	mov sc[1] = 1
	;; 
	add mpr[0] = mpr[0], mpr[1]
(non0_2)	shl sc[0] = sc[0], iFcode
	add mpr[2] = mpr[2], mpr[3]
(non0_3)	shl sc[1] = sc[1], iFcode
	add mpr[4] = mpr[4], mpr[5]
	add mpr[6] = mpr[6], mpr[7]
	;; 
(non0_2)	add sc[0] = -1, sc[0]
(non0_3)	add sc[1] = -1, sc[1]
	mov ret0 = 2
	;; 
(non0_2)	add component[0] = component[0], sc[0]
(non0_3)	add component[1] = component[1], sc[1]
	;; 
(non0_2)	shr component[0] = component[0], iFcode
(non0_3)	shr component[1] = component[1], iFcode
	add mpr[0] = mpr[0], mpr[2]
	add mpr[4] = mpr[4], mpr[6]
	;; 
(non0_2)	cmp.lt cg32_0, p0 = 32, component[0]
(non0_3)	cmp.lt cg32_1, p0 = 32, component[1]
	;; 
(cg32_0)	mov component[0] = 32
(cg32_1)	mov component[1] = 32
	;;
(non0_2)	addl tabaddress[0] = @gprel(mvtab#), gp
(non0_3)	addl tabaddress[1] = @gprel(mvtab#), gp
	;; 
(non0_2)	shladd tabaddress[0] = component[0], 2, tabaddress[0]
(non0_3)	shladd tabaddress[1] = component[1], 2, tabaddress[1]
	;; 
(non0_2)	ld4 sc[0] = [tabaddress[0]]
(non0_3)	ld4 sc[1] = [tabaddress[1]]
	mov component[0] = dx
	mov component[1] = dy
	cmp.ne non0_0, p0 = 0, dx
	cmp.gt neg_0, p0 = 0, dx
	.pred.rel "mutex", p30, p34	//non0_0, neg_0
 
	cmp.ne non0_1, p0 = 0, dy
	cmp.gt neg_1, p0 = 0, dy
	;;
	.pred.rel "mutex", p31, p35	//non0_1, neg_1
	 
(non0_2)	add sc[0] = iFcode, sc[0]
(non0_3)	add sc[1] = iFcode, sc[1]
	;;
(non0_2)	add ret0 = ret0, sc[0]
(neg_0)	sub component[0] = 0, component[0]	//abs
(neg_1)	sub component[1] = 0, component[1]	//abs
	;; 
(non0_3)	add ret0 = ret0, sc[1]
		add iSAD = mpr[0], mpr[4]
	;; 

.explicit
{.mii
	setf.sig fmv = ret0
(non0_0)	mov sc[0] = 1
(non0_1)	mov sc[1] = 1
	;; 
}
{.mfb
	xmpy.l fmv = fmv, fQuant
}
{.mii
(non0_0)	shl sc[0] = sc[0], iFcode
(non0_1)	shl sc[1] = sc[1], iFcode
	;;
}
	
.default
	
(non0_0)	add sc[0] = -1, sc[0]
(non0_1)	add sc[1] = -1, sc[1]
	;; 
(non0_0)	add component[0] = component[0], sc[0]
(non0_1)	add component[1] = component[1], sc[1]
	;; 
(non0_0)	shr component[0] = component[0], iFcode
(non0_1)	shr component[1] = component[1], iFcode
	;; 
(non0_0)	cmp.lt cg32_0, p0 = 32, component[0]
(non0_1)	cmp.lt cg32_1, p0 = 32, component[1]
	;; 
(cg32_0)	mov component[0] = 32
(cg32_1)	mov component[1] = 32
	;;
(non0_0)	addl tabaddress[0] = @gprel(mvtab#), gp
(non0_1)	addl tabaddress[1] = @gprel(mvtab#), gp
	;; 
(non0_0)	shladd tabaddress[0] = component[0], 2, tabaddress[0]
(non0_1)	shladd tabaddress[1] = component[1], 2, tabaddress[1]
	getf.sig ret0 = fmv
	;; 
(non0_0)	ld4 sc[0] = [tabaddress[0]]
(non0_1)	ld4 sc[1] = [tabaddress[1]]
	add mpr[8] = mpr[8], ret0
	;;
