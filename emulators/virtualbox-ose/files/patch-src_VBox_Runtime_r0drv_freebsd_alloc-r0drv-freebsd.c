--- src/VBox/Runtime/r0drv/freebsd/alloc-r0drv-freebsd.c.orig	2019-11-21 17:02:50 UTC
+++ src/VBox/Runtime/r0drv/freebsd/alloc-r0drv-freebsd.c
@@ -79,6 +79,7 @@ MALLOC_DEFINE(M_IPRTCONT, "iprtcont", "IPRT - contiguo
 
 DECLHIDDEN(int) rtR0MemAllocEx(size_t cb, uint32_t fFlags, PRTMEMHDR *ppHdr)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
     size_t      cbAllocated = cb;
     PRTMEMHDR   pHdr        = NULL;
 
@@ -101,8 +102,10 @@ DECLHIDDEN(int) rtR0MemAllocEx(size_t cb, uint32_t fFl
         cbAllocated = RT_ALIGN_Z(cb + sizeof(*pHdr), PAGE_SIZE);
 
         pVmObject = vm_object_allocate(OBJT_DEFAULT, cbAllocated >> PAGE_SHIFT);
-        if (!pVmObject)
+        if (!pVmObject) {
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_EXEC_MEMORY;
+        }
 
         /* Addr contains a start address vm_map_find will start searching for suitable space at. */
 #if __FreeBSD_version >= 1000055
@@ -139,6 +142,8 @@ DECLHIDDEN(int) rtR0MemAllocEx(size_t cb, uint32_t fFl
                                  fFlags & RTMEMHDR_FLAG_ZEROED ? M_NOWAIT | M_ZERO : M_NOWAIT);
     }
 
+    IPRT_FREEBSD_RESTORE_EFL_AC();
+
     if (RT_UNLIKELY(!pHdr))
         return VERR_NO_MEMORY;
 
@@ -154,6 +159,8 @@ DECLHIDDEN(int) rtR0MemAllocEx(size_t cb, uint32_t fFl
 
 DECLHIDDEN(void) rtR0MemFree(PRTMEMHDR pHdr)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
+
     pHdr->u32Magic += 1;
 
 #ifdef RT_ARCH_AMD64
@@ -166,11 +173,14 @@ DECLHIDDEN(void) rtR0MemFree(PRTMEMHDR pHdr)
     else
 #endif
         free(pHdr, M_IPRTHEAP);
+
+    IPRT_FREEBSD_RESTORE_EFL_AC();
 }
 
 
 RTR0DECL(void *) RTMemContAlloc(PRTCCPHYS pPhys, size_t cb)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
     void *pv;
 
     /*
@@ -195,6 +205,7 @@ RTR0DECL(void *) RTMemContAlloc(PRTCCPHYS pPhys, size_
         *pPhys = vtophys(pv);
         Assert(!(*pPhys & PAGE_OFFSET_MASK));
     }
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return pv;
 }
 
@@ -204,7 +215,9 @@ RTR0DECL(void) RTMemContFree(void *pv, size_t cb)
     if (pv)
     {
         AssertMsg(!((uintptr_t)pv & PAGE_OFFSET_MASK), ("pv=%p\n", pv));
+        IPRT_FREEBSD_SAVE_EFL_AC();
         contigfree(pv, cb, M_IPRTCONT);
+        IPRT_FREEBSD_RESTORE_EFL_AC();
     }
 }
 
