--- include/iprt/string.h.orig	2019-11-21 16:51:44 UTC
+++ include/iprt/string.h
@@ -46,6 +46,11 @@
 #elif defined(RT_OS_FREEBSD) && defined(_KERNEL)
   RT_C_DECLS_BEGIN
 # include <sys/libkern.h>
+  /*
+   * Kludge for the FreeBSD kernel:
+   *  sys/libkern.h includes sys/param.h via sys/systm.h since r335879.
+   */
+# undef PVM
   RT_C_DECLS_END
 
 #elif defined(RT_OS_NETBSD) && defined(_KERNEL)
