--- src/VBox/Frontends/VBoxManage/VBoxManageHelp.cpp.orig	2019-11-21 17:01:16 UTC
+++ src/VBox/Frontends/VBoxManage/VBoxManageHelp.cpp
@@ -641,7 +641,7 @@ void printUsage(USAGECATEGORY enmCommand, uint64_t fSu
                      "                                             file <file>|\n"
                      "                                             <devicename>]\n"
                      "                            [--uarttype<1-N> 16450|16550A|16750]\n"
-#if defined(RT_OS_LINUX) || defined(RT_OS_WINDOWS)
+#if defined(RT_OS_FREEBSD) || defined(RT_OS_LINUX) || defined(RT_OS_WINDOWS)
                      "                            [--lpt<1-N> off|<I/O base> <IRQ>]\n"
                      "                            [--lptmode<1-N> <devicename>]\n"
 #endif
@@ -656,7 +656,7 @@ void printUsage(USAGECATEGORY enmCommand, uint64_t fSu
             RTStrmPrintf(pStrm, "|dsound");
 #endif
         }
-        if (fLinux || fSolaris)
+        if (fLinux || fFreeBSD || fSolaris)
         {
             RTStrmPrintf(pStrm, ""
 #ifdef VBOX_WITH_AUDIO_OSS
@@ -669,22 +669,6 @@ void printUsage(USAGECATEGORY enmCommand, uint64_t fSu
                                 "|pulse"
 #endif
                         );
-        }
-        if (fFreeBSD)
-        {
-#ifdef VBOX_WITH_AUDIO_OSS
-            /* Get the line break sorted when dumping all option variants. */
-            if (fDumpOpts)
-            {
-                RTStrmPrintf(pStrm, "|\n"
-                     "                                     oss");
-            }
-            else
-                RTStrmPrintf(pStrm, "|oss");
-#endif
-#ifdef VBOX_WITH_AUDIO_PULSE
-            RTStrmPrintf(pStrm, "|pulse");
-#endif
         }
         if (fDarwin)
         {
