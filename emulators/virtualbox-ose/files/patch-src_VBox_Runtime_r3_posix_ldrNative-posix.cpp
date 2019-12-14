--- src/VBox/Runtime/r3/posix/ldrNative-posix.cpp.orig	2019-11-21 17:02:57 UTC
+++ src/VBox/Runtime/r3/posix/ldrNative-posix.cpp
@@ -148,7 +148,7 @@ DECLHIDDEN(int) rtldrNativeLoadSystem(const char *pszF
      */
     if (!pszExt)
     {
-#if !defined(RT_OS_DARWIN) && !defined(RT_OS_OS2) && !defined(RT_OS_WINDOWS)
+#if !defined(RT_OS_DARWIN) && !defined(RT_OS_FREEBSD) && !defined(RT_OS_OS2) && !defined(RT_OS_WINDOWS)
         if (    (fFlags & RTLDRLOAD_FLAGS_SO_VER_BEGIN_MASK) >> RTLDRLOAD_FLAGS_SO_VER_BEGIN_SHIFT
              == (fFlags & RTLDRLOAD_FLAGS_SO_VER_END_MASK)   >> RTLDRLOAD_FLAGS_SO_VER_END_SHIFT)
 #endif
@@ -168,7 +168,7 @@ DECLHIDDEN(int) rtldrNativeLoadSystem(const char *pszF
 
     int rc = RTLdrLoadEx(pszTmp, phLdrMod, fFlags2, NULL);
 
-#if !defined(RT_OS_DARWIN) && !defined(RT_OS_OS2) && !defined(RT_OS_WINDOWS)
+#if !defined(RT_OS_DARWIN) && !defined(RT_OS_FREEBSD) && !defined(RT_OS_OS2) && !defined(RT_OS_WINDOWS)
     /*
      * If no version was given after the .so and do .so.MAJOR search according
      * to the range in the fFlags.
