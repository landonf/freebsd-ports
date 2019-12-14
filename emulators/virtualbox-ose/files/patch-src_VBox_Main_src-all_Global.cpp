--- src/VBox/Main/src-all/Global.cpp.orig	2019-11-21 17:02:16 UTC
+++ src/VBox/Main/src-all/Global.cpp
@@ -364,14 +364,15 @@ const Global::OSType Global::sOSTypes[] =
         StorageControllerType_IntelAhci, StorageBus_SATA, ChipsetType_PIIX3, AudioControllerType_AC97, AudioCodecType_STAC9700  },
 
     { "BSD",     "BSD",               "FreeBSD",            "FreeBSD (32-bit)",
-      VBOXOSTYPE_FreeBSD,         VBOXOSHINT_NONE,
-      1024,  16,  2 * _1G64, GraphicsControllerType_VBoxVGA, NetworkAdapterType_I82540EM, 0, StorageControllerType_PIIX4, StorageBus_IDE,
-        StorageControllerType_PIIX4, StorageBus_IDE, ChipsetType_PIIX3, AudioControllerType_AC97, AudioCodecType_STAC9700  },
+      VBOXOSTYPE_FreeBSD,         VBOXOSHINT_RTCUTC | VBOXOSHINT_PAE | VBOXOSHINT_X2APIC,
+      1024,  16, 10 * _1G64, GraphicsControllerType_VMSVGA, NetworkAdapterType_I82540EM, 0, StorageControllerType_PIIX4, StorageBus_IDE,
+        StorageControllerType_IntelAhci, StorageBus_SATA, ChipsetType_PIIX3, AudioControllerType_AC97, AudioCodecType_STAC9700  },
 
     { "BSD",     "BSD",               "FreeBSD_64",         "FreeBSD (64-bit)",
-      VBOXOSTYPE_FreeBSD_x64,     VBOXOSHINT_64BIT | VBOXOSHINT_HWVIRTEX | VBOXOSHINT_IOAPIC,
-      1024,  16, 16 * _1G64, GraphicsControllerType_VBoxVGA, NetworkAdapterType_I82540EM, 0, StorageControllerType_PIIX4, StorageBus_IDE,
-        StorageControllerType_PIIX4, StorageBus_IDE, ChipsetType_PIIX3, AudioControllerType_AC97, AudioCodecType_STAC9700  },
+      VBOXOSTYPE_FreeBSD_x64,     VBOXOSHINT_64BIT | VBOXOSHINT_HWVIRTEX | VBOXOSHINT_IOAPIC | VBOXOSHINT_RTCUTC
+                                | VBOXOSHINT_X2APIC,
+      1024,  16, 10 * _1G64, GraphicsControllerType_VMSVGA, NetworkAdapterType_I82540EM, 0, StorageControllerType_PIIX4, StorageBus_IDE,
+        StorageControllerType_IntelAhci, StorageBus_SATA, ChipsetType_PIIX3, AudioControllerType_AC97, AudioCodecType_STAC9700  },
 
     { "BSD",     "BSD",               "OpenBSD",            "OpenBSD (32-bit)",
       VBOXOSTYPE_OpenBSD,         VBOXOSHINT_HWVIRTEX,
