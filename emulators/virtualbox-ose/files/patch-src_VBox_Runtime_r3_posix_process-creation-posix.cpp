--- src/VBox/Runtime/r3/posix/process-creation-posix.cpp.orig	2019-11-21 17:02:57 UTC
+++ src/VBox/Runtime/r3/posix/process-creation-posix.cpp
@@ -33,7 +33,7 @@
 #ifdef RT_OS_LINUX
 # define IPRT_WITH_DYNAMIC_CRYPT_R
 #endif
-#if (defined(RT_OS_LINUX) || defined(RT_OS_OS2)) && !defined(_GNU_SOURCE)
+#if (defined(RT_OS_FREEBSD) || defined(RT_OS_LINUX) || defined(RT_OS_OS2)) && !defined(_GNU_SOURCE)
 # define _GNU_SOURCE
 #endif
 
@@ -64,7 +64,7 @@
 # include <shadow.h>
 #endif
 
-#if defined(RT_OS_LINUX) || defined(RT_OS_OS2)
+#if defined(RT_OS_FREEBSD) || defined(RT_OS_LINUX) || defined(RT_OS_OS2)
 /* While Solaris has posix_spawn() of course we don't want to use it as
  * we need to have the child in a different process contract, no matter
  * whether it is started detached or not. */
@@ -97,6 +97,10 @@
 # include <iprt/asm.h>
 #endif
 
+#ifdef RT_OS_FREEBSD
+# include <sys/param.h>
+#endif
+
 #ifdef RT_OS_SOLARIS
 # include <limits.h>
 # include <sys/ctfs.h>
@@ -372,7 +376,7 @@ static int rtCheckCredentials(const char *pszUser, con
     if (pPwd->pw_passwd && *pPwd->pw_passwd)
 # endif
     {
-# if defined(RT_OS_LINUX) || defined(RT_OS_OS2)
+# if (defined(RT_OS_FREEBSD) && __FreeBSD_version >= 1200000) || defined(RT_OS_LINUX) || defined(RT_OS_OS2)
 #  ifdef IPRT_WITH_DYNAMIC_CRYPT_R
         size_t const       cbCryptData = RT_MAX(sizeof(struct crypt_data) * 2, _256K);
 #  else
