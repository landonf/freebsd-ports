--- src/blacklist.c.orig	2019-05-01 22:08:39 UTC
+++ src/blacklist.c
@@ -58,10 +58,7 @@ static bool is_string_in_array(const char *string, GVa
 	while (g_variant_iter_loop(&iter, "&s", &element))
 	{
 		if (g_strcmp0(element, string) == 0)
-		{
-			g_clear_pointer(&element, g_free);
 			return true;
-		}
 	}
 
 	return false;
