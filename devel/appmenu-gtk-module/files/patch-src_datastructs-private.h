--- src/datastructs-private.h.orig	2019-05-01 22:08:39 UTC
+++ src/datastructs-private.h
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
