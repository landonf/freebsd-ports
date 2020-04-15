--- sesman/chansrv/chansrv_xfs.h.orig	2020-04-15 13:44:29.979479000 -0600
+++ sesman/chansrv/chansrv_xfs.h	2020-04-15 13:46:40.579115000 -0600
@@ -23,6 +23,10 @@
 /* Skip this include if there's no FUSE */
 #ifdef XRDP_FUSE
 
+#ifndef FUSE_USE_VERSION
+#define FUSE_USE_VERSION 26
+#endif
+
 #include <stddef.h>
 #include <fuse_lowlevel.h>
 #include <time.h>
