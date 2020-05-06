--- server/faker-glx.cpp	2020-05-06 00:58:34.001364000 -0600
+++ server/faker-glx.cpp	2020-05-06 00:58:53.649383000 -0600
@@ -1671,6 +1671,8 @@
 		CHECK_FAKED(glViewport)
 		CHECK_FAKED(glDrawBuffer)
 		CHECK_FAKED(glPopAttrib)
+		CHECK_FAKED(glGetString)
+		CHECK_FAKED(glGetStringi)
 	}
 	if(!retval)
 	{
