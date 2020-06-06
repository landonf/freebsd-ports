--- src/VBox/Main/webservice/vboxweb.cpp.orig	2020-06-05 23:57:28 UTC
+++ src/VBox/Main/webservice/vboxweb.cpp
@@ -944,7 +944,7 @@ static void doQueuesLoop()
                 if (rv == 0)
                     continue; // timeout, not necessary to bother gsoap
                 // r < 0, errno
-                if (soap_socket_errno(soap.master) == SOAP_EINTR)
+                if (soap_socket_errno == SOAP_EINTR)
                     rv = 0; // re-check if we should terminate
                 break;
             }
