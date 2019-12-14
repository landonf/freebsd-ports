--- src/VBox/Main/src-server/VirtualBoxImpl.cpp.orig	2019-11-21 17:02:20 UTC
+++ src/VBox/Main/src-server/VirtualBoxImpl.cpp
@@ -2655,7 +2655,7 @@ int VirtualBox::i_encryptSettingBytes(const uint8_t *a
     if (aCiphertextSize < 32)
         return VERR_INVALID_PARAMETER;
 
-    AssertCompile(sizeof(m->SettingsCipherKey) >= 32);
+    // AssertCompile(sizeof(m->SettingsCipherKey) >= 32);
 
     /* store the first 8 bytes of the cipherkey for verification */
     for (i = 0, j = 0; i < 8; i++, j++)
