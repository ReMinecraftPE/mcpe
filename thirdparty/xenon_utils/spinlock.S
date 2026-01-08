// needed until proper version in libxenon

// thanks cOz for the following :)
.globl spin_lock
spin_lock:
        li      %r8, 0
1:
        mfmsr   %r7
		mtmsrd  %r8, 1
        lwarx   %r4, 0, %r3
        cmplwi  cr7, %r4, 0
        bne     cr7, 2f
		mr      %r4, %r3
2:
        stwcx.  %r4, 0, %r3
        mtmsrd  %r7, 1
        bne     cr7, 2f
		bne     2f
        lwsync
        blr
2:
        db16cyc
        b 1b
 
.globl spin_unlock
spin_unlock:
        li      %r4, 0
        sync    1  // lwsync
        stw     %r4, 0(%r3)
        li      %r4, -1
        mtspr   310, %r4 // reset the hypervisor decrementer to fix a lock issue
        blr