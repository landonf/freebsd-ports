--- src/VBox/Main/include/HostPower.h.orig	2019-11-21 17:02:14 UTC
+++ src/VBox/Main/include/HostPower.h
@@ -31,7 +31,7 @@
 
 #include <vector>
 
-#ifdef RT_OS_LINUX
+#if defined(RT_OS_LINUX) || defined(RT_OS_FREEBSD)
 # include <VBox/dbus.h>
 #endif
 
@@ -67,7 +67,7 @@ class HostPowerServiceWin : public HostPowerService (p
     RTTHREAD    mThread;
 };
 # endif
-# if defined(RT_OS_LINUX) || defined(DOXYGEN_RUNNING)
+# if defined(RT_OS_LINUX) || defined(RT_OS_FREEBSD) || defined(DOXYGEN_RUNNING)
 /**
  * The Linux hosted Power Service.
  */
