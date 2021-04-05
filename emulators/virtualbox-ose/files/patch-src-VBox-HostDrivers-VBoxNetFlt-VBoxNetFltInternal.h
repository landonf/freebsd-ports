--- src/VBox/HostDrivers/VBoxNetFlt/VBoxNetFltInternal.h.orig	2019-06-15 19:42:35 UTC
+++ src/VBox/HostDrivers/VBoxNetFlt/VBoxNetFltInternal.h
@@ -240,6 +240,10 @@ typedef struct VBOXNETFLTINS
             hook_p input;
             /** Output hook */
             hook_p output;
+            /** Pending synchronous netgraph requests */
+            STAILQ_HEAD(,vboxnetflt_sreq) sreq_list;
+            /** Synchronous request lock */
+            struct mtx sreq_lck;
             /** Original interface flags */
             unsigned int flags;
             /** Input queue */
