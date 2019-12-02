--- bin/clion.sh.orig	2019-11-27 22:59:35 UTC
+++ bin/clion.sh
@@ -100,7 +100,7 @@ if [ -z "$JDK" ]; then
 
   if [ -n "$JDK_PATH" ]; then
     if [ "$OS_TYPE" = "FreeBSD" ] || [ "$OS_TYPE" = "MidnightBSD" ]; then
-      JAVA_LOCATION=$(JAVAVM_DRYRUN=yes java | "$GREP" '^JAVA_HOME' | "$CUT" -c11-)
+      JAVA_LOCATION=$(JAVAVM_DRYRUN=yes JAVA_VERSION=11+ java | "$GREP" '^JAVA_HOME' | "$CUT" -c11-)
       if [ -x "$JAVA_LOCATION/bin/java" ]; then
         JDK="$JAVA_LOCATION"
       fi
