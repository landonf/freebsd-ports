--- src/VBox/Runtime/r0drv/freebsd/semfastmutex-r0drv-freebsd.c.orig	2019-11-21 17:02:50 UTC
+++ src/VBox/Runtime/r0drv/freebsd/semfastmutex-r0drv-freebsd.c
@@ -87,6 +87,7 @@ RTDECL(int)  RTSemFastMutexCreate(PRTSEMFASTMUTEX phFa
 {
     AssertCompile(sizeof(RTSEMFASTMUTEXINTERNAL) > sizeof(void *));
     AssertPtrReturn(phFastMtx, VERR_INVALID_POINTER);
+    IPRT_FREEBSD_SAVE_EFL_AC();
 
     PRTSEMFASTMUTEXINTERNAL pThis = (PRTSEMFASTMUTEXINTERNAL)RTMemAllocZ(sizeof(*pThis));
     if (pThis)
@@ -95,8 +96,10 @@ RTDECL(int)  RTSemFastMutexCreate(PRTSEMFASTMUTEX phFa
         sx_init_flags(&pThis->SxLock, "IPRT Fast Mutex Semaphore", SX_DUPOK);
 
         *phFastMtx = pThis;
+        IPRT_FREEBSD_RESTORE_EFL_AC();
         return VINF_SUCCESS;
     }
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return VERR_NO_MEMORY;
 }
 
@@ -108,11 +111,13 @@ RTDECL(int)  RTSemFastMutexDestroy(RTSEMFASTMUTEX hFas
         return VINF_SUCCESS;
     AssertPtrReturn(pThis, VERR_INVALID_HANDLE);
     AssertMsgReturn(pThis->u32Magic == RTSEMFASTMUTEX_MAGIC, ("%p: u32Magic=%RX32\n", pThis, pThis->u32Magic), VERR_INVALID_HANDLE);
+    IPRT_FREEBSD_SAVE_EFL_AC();
 
     ASMAtomicWriteU32(&pThis->u32Magic, RTSEMFASTMUTEX_MAGIC_DEAD);
     sx_destroy(&pThis->SxLock);
     RTMemFree(pThis);
 
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return VINF_SUCCESS;
 }
 
@@ -122,8 +127,11 @@ RTDECL(int)  RTSemFastMutexRequest(RTSEMFASTMUTEX hFas
     PRTSEMFASTMUTEXINTERNAL pThis = hFastMtx;
     AssertPtrReturn(pThis, VERR_INVALID_HANDLE);
     AssertMsgReturn(pThis->u32Magic == RTSEMFASTMUTEX_MAGIC, ("%p: u32Magic=%RX32\n", pThis, pThis->u32Magic), VERR_INVALID_HANDLE);
+    IPRT_FREEBSD_SAVE_EFL_AC();
 
     sx_xlock(&pThis->SxLock);
+
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return VINF_SUCCESS;
 }
 
@@ -133,8 +141,11 @@ RTDECL(int)  RTSemFastMutexRelease(RTSEMFASTMUTEX hFas
     PRTSEMFASTMUTEXINTERNAL pThis = hFastMtx;
     AssertPtrReturn(pThis, VERR_INVALID_HANDLE);
     AssertMsgReturn(pThis->u32Magic == RTSEMFASTMUTEX_MAGIC, ("%p: u32Magic=%RX32\n", pThis, pThis->u32Magic), VERR_INVALID_HANDLE);
+    IPRT_FREEBSD_SAVE_EFL_AC();
 
     sx_xunlock(&pThis->SxLock);
+
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return VINF_SUCCESS;
 }
 
