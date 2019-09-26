diff --git a/subprojects/appmenu-gtk-module/src/blacklist.c b/subprojects/appmenu-gtk-module/src/blacklist.c
index 60f920191a77a60a3614385c3301b8a8f516c9c8..3bbc94aaf03d129487d35b1edc13c95644f6259d 100644
--- src/blacklist.c
+++ src/blacklist.c
@@ -58,10 +58,7 @@ static bool is_string_in_array(const char *string, GVariant *array)
 	while (g_variant_iter_loop(&iter, "&s", &element))
 	{
 		if (g_strcmp0(element, string) == 0)
-		{
-			g_clear_pointer(&element, g_free);
 			return true;
-		}
 	}
 
 	return false;
