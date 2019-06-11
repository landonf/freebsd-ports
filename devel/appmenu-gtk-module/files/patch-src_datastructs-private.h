--- src/datastructs-private.h.orig	2019-05-31 18:15:23.103515000 -0600
+++ src/datastructs-private.h	2019-05-31 18:15:34.808471000 -0600
@@ -30,8 +30,8 @@
 
 struct _WindowData
 {
-	uint window_id;
-	ulong wayland_window_id;
+	guint window_id;
+	gulong wayland_window_id;
 	GMenu *menu_model;
 	guint menu_model_export_id;
 	GSList *menus;
