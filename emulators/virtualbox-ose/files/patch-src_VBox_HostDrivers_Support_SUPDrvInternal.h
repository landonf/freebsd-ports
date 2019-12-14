--- src/VBox/HostDrivers/Support/SUPDrvInternal.h.orig	2019-11-21 17:01:59 UTC
+++ src/VBox/HostDrivers/Support/SUPDrvInternal.h
@@ -211,7 +211,7 @@
 # define SUPDRV_WITHOUT_MSR_PROBER
 #endif
 
-#if 1
+#if 0
 /** @def SUPDRV_USE_TSC_DELTA_THREAD
  * Use a dedicated kernel thread to service TSC-delta measurement requests.
  * @todo Test on servers with many CPUs and sockets. */
