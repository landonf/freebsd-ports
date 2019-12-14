--- include/VBox/VBoxGL2D.h.orig	2019-11-21 16:51:31 UTC
+++ include/VBox/VBoxGL2D.h
@@ -111,7 +111,7 @@ typedef GLvoid (APIENTRY *PFNVBOXVHWA_UNIFORM3I)(GLint
 typedef GLvoid (APIENTRY *PFNVBOXVHWA_UNIFORM4I)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
 
 /* GL_ARB_pixel_buffer_object*/
-#ifndef Q_WS_MAC
+#if 0
 /* apears to be defined on mac */
 typedef ptrdiff_t GLsizeiptr;
 #endif
