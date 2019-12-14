--- src/VBox/HostDrivers/VBoxNetFlt/VBoxNetFltInternal.h	2019-12-10 10:52:37.000000000 -0700
+++ src/VBox/HostDrivers/VBoxNetFlt/VBoxNetFltInternal.h	2019-12-13 17:34:41.330051000 -0700
@@ -243,6 +243,10 @@
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
