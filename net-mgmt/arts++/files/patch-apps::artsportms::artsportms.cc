--- ./apps/artsportms/artsportms.cc.orig	Mon Oct  2 13:05:12 2000
+++ ./apps/artsportms/artsportms.cc	Wed Sep 25 16:38:09 2002
@@ -51,13 +51,21 @@
 #include <errno.h>
 }
 
-#include <fstream.h>
-#include <string>
-
 #include "ArtsPortMatrix.hh"
 #include "ArtsIfIndexSelectionSet.hh"
 #include "ArtsTimeIntervalSelectionSet.hh"
 #include "ArtsPackageVersion.hh"
+
+#ifdef HAVE_FSTREAM
+# include <fstream>
+# include <iterator>
+  using namespace std;
+#else
+# include <fstream.h>
+#endif
+
+#include <string>
+
 
 #ifndef NDEBUG
   #include "ArtsDebug.hh"
