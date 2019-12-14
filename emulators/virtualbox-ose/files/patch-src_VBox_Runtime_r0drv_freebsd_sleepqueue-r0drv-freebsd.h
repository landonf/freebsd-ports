--- src/VBox/Runtime/r0drv/freebsd/sleepqueue-r0drv-freebsd.h.orig	2019-11-21 17:02:50 UTC
+++ src/VBox/Runtime/r0drv/freebsd/sleepqueue-r0drv-freebsd.h
@@ -84,6 +84,8 @@ DECLINLINE(uint32_t) rtR0SemBsdWaitUpdateTimeout(PRTR0
     uint64_t cTicks = ASMMultU64ByU32DivByU32(uTimeout, hz, UINT32_C(1000000000));
     if (cTicks >= INT_MAX)
         return RTSEMWAIT_FLAGS_INDEFINITE;
+    else if (cTicks == 0 && uTimeout > 0)
+        pWait->iTimeout     = 1;
     else
         pWait->iTimeout     = (int)cTicks;
 #endif
