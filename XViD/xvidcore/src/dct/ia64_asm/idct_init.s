
	addreg3 = r20
	addreg4 = r21
	addreg5 = r22
	addreg6 = r23
	
	one = f30
	alloc   r16 = ar.pfs, 1, 71, 0, 0
	addl	addreg1 = @gprel(.data_c0#), gp
	addl	addreg2 = @gprel(.data_c2#), gp
	;;
	add	addreg3 = 32, addreg1
	add	addreg4 = 32, addreg2
	add	addreg5 = 64, addreg1
	add	addreg6 = 64, addreg2
	;; 
	ldfp8	c0, c1 = [addreg1]
	ldfp8	c2, c3 = [addreg2]
	;; 
	ldfp8	c4, c5 = [addreg3], 16
	ldfp8	c6, c7 = [addreg4], 16
	add	addreg1 = 96, addreg1
	add	addreg2 = 96, addreg2
	;;
	ldfp8	c8, c9 = [addreg5], 16
	ldfp8	c10, c11 = [addreg6], 16
	;; 
	ldfp8	c12, c13 = [addreg1]
	ldfp8	c14, c15 = [addreg2]
	;;
	mov	addreg1 = in0
	fpack	one = f1, f1
	add	addreg2 = 2, in0
	;; 
