--- src/VBox/HostDrivers/VBoxNetFlt/freebsd/VBoxNetFlt-freebsd.c.orig	2019-05-13 13:19:28 UTC
+++ src/VBox/HostDrivers/VBoxNetFlt/freebsd/VBoxNetFlt-freebsd.c
@@ -52,11 +52,14 @@
 #include <net/if_dl.h>
 #include <net/if_types.h>
 #include <net/ethernet.h>
+#include <net/if_vlan_var.h>
 
 #include <netgraph/ng_message.h>
 #include <netgraph/netgraph.h>
 #include <netgraph/ng_parse.h>
 
+#include <netgraph/ng_ether.h>
+
 #define LOG_GROUP LOG_GROUP_NET_FLT_DRV
 #include <VBox/version.h>
 #include <VBox/err.h>
@@ -73,6 +76,9 @@
 
 #define VBOXNETFLT_OS_SPECFIC 1
 #include "../VBoxNetFltInternal.h"
+#include "freebsd/the-freebsd-kernel.h"
+                                                                                                                        
+MALLOC_DEFINE(M_VBOXNETFLT, "vboxnetflt", "VBoxNetFlt netgraph node");
 
 static int vboxnetflt_modevent(struct module *, int, void *);
 static ng_constructor_t    ng_vboxnetflt_constructor;
@@ -93,6 +99,9 @@ static int        ng_vboxnetflt_mod_event(module_t mod
 /** Output netgraph hook name */
 #define NG_VBOXNETFLT_HOOK_OUT      "output"
 
+/** Maximum time to wait for ng_ether replies */
+#define NG_VBOXNETFLT_RPLY_TIMEOUT  (10*hz)
+
 /** mbuf tag identifier */
 #define MTAG_VBOX                   0x56424f58
 /** mbuf packet tag */
@@ -115,6 +124,18 @@ static int        ng_vboxnetflt_mod_event(module_t mod
 #endif /* !defined(__FreeBSD_version) || __FreeBSD_version < 800500 */
 
 /*
+ * Context used to track synchronous netgraph request state.
+ */
+struct vboxnetflt_sreq {
+    uint32_t         typecookie;    /**< Matched type cookie. */
+    uint32_t         cmd;           /**< Matched command. */
+    uint32_t         token;         /**< Matched token. */
+    struct ng_mesg  *resp;          /**< On completion, a copy of the reply */
+
+    STAILQ_ENTRY(vboxnetflt_sreq) sr_link;
+};
+
+/*
  * Netgraph command list, we don't support any
  * additional commands.
  */
@@ -303,24 +324,225 @@ static int ng_vboxnetflt_newhook(node_p node, hook_p h
 }
 
 /**
- * Netgraph message processing for node specific messages.
- * We don't accept any special messages so this is not used.
+ * Enqueue and return a reference to a new synchronous response handler, or
+ * NULL if allocation fails.
  */
+static struct vboxnetflt_sreq *
+vboxnetflt_sreq_enqueue(PVBOXNETFLTINS pThis, uint32_t cookie, uint32_t cmd,
+    uint32_t token)
+{
+    struct vboxnetflt_sreq *sreq;
+
+    mtx_assert(&pThis->u.s.sreq_lck, MA_OWNED);
+
+    sreq = malloc(sizeof(*sreq), M_VBOXNETFLT, M_NOWAIT|M_ZERO);
+    if (sreq == NULL)
+        return (NULL);
+
+    sreq->typecookie = cookie;
+    sreq->cmd = cmd;
+    sreq->token = token;
+
+    STAILQ_INSERT_TAIL(&pThis->u.s.sreq_list, sreq, sr_link);
+
+    return (sreq);
+}
+
+/**
+ * Dequeue and return the first synchronous responder handler capable of
+ * handling the given message, or NULL if no handler is found.
+ *
+ * The caller assumes ownership of any returned handler.
+ */
+static struct vboxnetflt_sreq *
+vboxnetflt_sreq_dequeue(PVBOXNETFLTINS pThis, struct ng_mesg *msg)
+{
+    struct vboxnetflt_sreq *sreq, *sr_next;
+    int error;
+
+    mtx_assert(&pThis->u.s.sreq_lck, MA_OWNED);
+
+    if (!(msg->header.flags & NGF_RESP))
+        return (EINVAL);
+
+    STAILQ_FOREACH_SAFE(sreq, &pThis->u.s.sreq_list, sr_link, sr_next)
+    {
+        if (sreq->typecookie != msg->header.typecookie)
+            continue;
+
+        if (sreq->cmd != msg->header.cmd)
+            continue;
+
+        if (sreq->token != msg->header.token)
+            continue;
+
+        STAILQ_REMOVE(&pThis->u.s.sreq_list, sreq, vboxnetflt_sreq,
+            sr_link);
+
+        return (sreq);
+    }
+
+    return (NULL);
+}
+
+/**
+ * Wait for completion of a previously enqueued response handler. On
+ * return, the handler will no longer be enqueued, and the caller is
+ * responsible for deallocating it via vboxnetflt_sreq_free().
+ */
+static int
+vboxnetflt_sreq_wait_enqueued(PVBOXNETFLTINS pThis,
+    struct vboxnetflt_sreq *sreq, int timo)
+{
+    int error;
+
+    mtx_assert(&pThis->u.s.sreq_lck, MA_OWNED);
+
+    while (sreq->resp == NULL) {
+        error = mtx_sleep(sreq, &pThis->u.s.sreq_lck, 0, "vboxnetflt_sreq",
+            timo);
+
+        if (error)
+            break;
+    }
+
+    /* If our request was completed, we don't care if mtx_sleep()
+     * timed out or otherwise failed */
+    if (sreq->resp != NULL)
+        return (0);
+
+    /*
+     * Otherwise, we stopped waiting before a response has been
+     * received, and we're responsible for removing this handler from
+     * the queue.
+     *
+     * If the expected message is delivered later, it will be dropped as
+     * spurious if it fails to match against another registered handler.
+     */
+    STAILQ_REMOVE(&pThis->u.s.sreq_list, sreq, vboxnetflt_sreq,
+        sr_link);
+
+    return (ETIMEDOUT);
+}
+
+/**
+ * Free the given synchronous request handler and all associated
+ * state.
+ *
+ * The handler must be dequeued and owned by the caller.
+ */
+static void
+vboxnetflt_sreq_free(struct vboxnetflt_sreq *sreq)
+{
+    if (sreq->resp != NULL)
+        NG_FREE_MSG(sreq->resp);
+
+    free(sreq, M_VBOXNETFLT);
+}
+
+
+
+/**
+ * Send a simple control message with no additional bytes, blocking until
+ * a response is received or a timeout is reached.
+ *
+ * On success, the caller assumes ownership of the response message, and
+ * is responsible for deallocating it via NG_FREE_MSG().
+ */
+int
+vboxnetflt_sreq_simple_rpc(PVBOXNETFLTINS pThis, node_p node, ng_ID_t peer,
+    uint32_t cookie, uint32_t cmd, struct ng_mesg **resp)
+{
+    struct ng_mesg *msg;
+    struct vboxnetflt_sreq *sr;
+    int error;
+
+    NG_MKMESSAGE(msg, cookie, cmd, 0, M_NOWAIT);
+    if (msg == NULL)
+        return (ENOMEM);
+
+    NG_SEND_MSG_ID(error, node, msg, peer, 0);
+    if (error) {
+        LogRel(("VBoxNetFlt: error sending netgraph message (cookie=%u, "
+            "cmd=%u) to [%x]\n", cookie, cmd, peer));
+        return (error);
+    }
+
+    mtx_lock(&pThis->u.s.sreq_lck);
+
+    /* Enqueue our response handler and wait for completion */
+    if ((sr = vboxnetflt_sreq_enqueue(pThis, cookie, cmd, 0)) == NULL) {
+        mtx_unlock(&pThis->u.s.sreq_lck);
+        return (ENOMEM);
+    }
+
+    error = vboxnetflt_sreq_wait_enqueued(pThis, sr,
+        NG_VBOXNETFLT_RPLY_TIMEOUT);
+
+    mtx_unlock(&pThis->u.s.sreq_lck);
+
+    if (error)
+        goto cleanup;
+
+    /* Transfer ownership of the message to the caller */
+    *resp = sr->resp;
+    sr->resp = NULL;
+
+    /* fallthrough to common cleanup */
+
+cleanup:
+    vboxnetflt_sreq_free(sr);
+    return (error);
+}
+
+/**
+ * Netgraph control message processing
+ */
 static int ng_vboxnetflt_rcvmsg(node_p node, item_p item, hook_p lasthook)
 {
     PVBOXNETFLTINS pThis = NG_NODE_PRIVATE(node);
     struct ng_mesg *msg;
-    int error = 0;
+    struct vboxnetflt_sreq *sreq;
+    int error;
 
     NGI_GET_MSG(item, msg);
-    if (msg->header.typecookie != NGM_VBOXNETFLT_COOKIE)
-        return (EINVAL);
 
-    switch (msg->header.cmd)
+    /* We don't accept any control messages of our own; drop anything that is
+     * not a message response. */
+    if (!(msg->header.flags & NGF_RESP))
     {
-        default:
-            error = EINVAL;
+        error = EINVAL;
+        goto failed;
     }
+
+    /* Deqeueue the first matching response handler, if any */
+    mtx_lock(&pThis->u.s.sreq_lck);
+    if ((sreq = vboxnetflt_sreq_dequeue(pThis, msg)) == NULL)
+    {
+        mtx_unlock(&pThis->u.s.sreq_lck);
+
+        LogRel(("VBoxNetFlt: dropping spurious netgraph reply (typecookie=%u, "
+            "cmd=%u, token=%u)\n", msg->header.typecookie, msg->header.cmd,
+            msg->header.token));
+
+        error = EINVAL;
+        goto failed;
+    }
+
+    /* Populate response data, transfering ownership of the message
+     * to the response handler. */
+    KASSERT(sreq->resp == NULL, ("request already completed"));
+    sreq->resp = msg;
+    msg = NULL;
+
+    /* Wake up any waiting threads */
+    mtx_unlock(&pThis->u.s.sreq_lck);
+    wakeup(sreq);
+
+    return (0);
+
+failed:
+    NG_FREE_MSG(msg);
     return (error);
 }
 
@@ -370,7 +590,11 @@ static int ng_vboxnetflt_rcvdata(hook_p hook, item_p i
         mtx_lock_spin(&pThis->u.s.inq.ifq_mtx);
         _IF_ENQUEUE(&pThis->u.s.inq, m);
         mtx_unlock_spin(&pThis->u.s.inq.ifq_mtx);
+#if __FreeBSD_version >= 1100100
+        taskqueue_enqueue(taskqueue_fast, &pThis->u.s.tskin);
+#else
         taskqueue_enqueue_fast(taskqueue_fast, &pThis->u.s.tskin);
+#endif
     }
     /*
      * Handle mbufs on the outgoing hook, frames going to the interface
@@ -388,7 +612,11 @@ static int ng_vboxnetflt_rcvdata(hook_p hook, item_p i
         mtx_lock_spin(&pThis->u.s.outq.ifq_mtx);
         _IF_ENQUEUE(&pThis->u.s.outq, m);
         mtx_unlock_spin(&pThis->u.s.outq.ifq_mtx);
+#if __FreeBSD_version >= 1100100
+        taskqueue_enqueue(taskqueue_fast, &pThis->u.s.tskout);
+#else
         taskqueue_enqueue_fast(taskqueue_fast, &pThis->u.s.tskout);
+#endif
     }
     else
     {
@@ -428,6 +656,8 @@ static void vboxNetFltFreeBSDinput(void *arg, int pend
     struct ifnet *ifp = pThis->u.s.ifp;
     unsigned int cSegs = 0;
     bool fDropIt = false, fActive;
+    bool is_vl_tagged = false;
+    uint16_t vl_tag;
     PINTNETSG pSG;
 
     VBOXCURVNET_SET(ifp->if_vnet);
@@ -440,6 +670,19 @@ static void vboxNetFltFreeBSDinput(void *arg, int pend
         if (m == NULL)
             break;
 
+        /* Prepend a VLAN header for consumption by the virtual switch */
+        if (m->m_flags & M_VLANTAG) {
+            vl_tag = m->m_pkthdr.ether_vtag;
+            is_vl_tagged = true;
+
+            m = ether_vlanencap(m, m->m_pkthdr.ether_vtag);
+            if (m == NULL) {
+                printf("vboxflt: unable to prepend VLAN header\n");
+                break;
+            }
+            m->m_flags &= ~M_VLANTAG;
+        }
+
         for (m0 = m; m0 != NULL; m0 = m0->m_next)
             if (m0->m_len > 0)
                 cSegs++;
@@ -454,6 +697,27 @@ static void vboxNetFltFreeBSDinput(void *arg, int pend
         vboxNetFltFreeBSDMBufToSG(pThis, m, pSG, cSegs, 0);
         fDropIt = pThis->pSwitchPort->pfnRecv(pThis->pSwitchPort, NULL /* pvIf */, pSG, INTNETTRUNKDIR_WIRE);
         RTMemTmpFree(pSG);
+
+        /* Restore the VLAN flags before re-injecting the packet */
+        if (is_vl_tagged && !fDropIt) {
+            struct ether_vlan_header *vl_hdr;
+
+            /* This shouldn't fail, as the header was just prepended */
+            if (m->m_len < sizeof(*vl_hdr) && (m = m_pullup(m, sizeof(*vl_hdr))) == NULL) {
+                printf("vboxflt: unable to pullup VLAN header\n");
+                m_freem(m);
+                break;
+            }
+
+            /* Copy the MAC dhost/shost over the 802.1q field */
+            vl_hdr = mtod(m, struct ether_vlan_header *);
+            bcopy((char *)vl_hdr, (char *)vl_hdr + ETHER_VLAN_ENCAP_LEN, ETHER_HDR_LEN - ETHER_TYPE_LEN);
+            m_adj(m, ETHER_VLAN_ENCAP_LEN);
+
+            m->m_pkthdr.ether_vtag = vl_tag;
+            m->m_flags |= M_VLANTAG;
+        }
+
         if (fDropIt)
             m_freem(m);
         else
@@ -513,6 +777,7 @@ static void vboxNetFltFreeBSDoutput(void *arg, int pen
  */
 int vboxNetFltPortOsXmit(PVBOXNETFLTINS pThis, void *pvIfData, PINTNETSG pSG, uint32_t fDst)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
     NOREF(pvIfData);
 
     void (*input_f)(struct ifnet *, struct mbuf *);
@@ -529,10 +794,16 @@ int vboxNetFltPortOsXmit(PVBOXNETFLTINS pThis, void *p
     {
         m = vboxNetFltFreeBSDSGMBufFromSG(pThis, pSG);
         if (m == NULL)
+        {
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_MEMORY;
+        }
         m = m_pullup(m, ETHER_HDR_LEN);
         if (m == NULL)
+        {
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_MEMORY;
+        }
 
         m->m_flags |= M_PKTHDR;
         ether_output_frame(ifp, m);
@@ -542,10 +813,16 @@ int vboxNetFltPortOsXmit(PVBOXNETFLTINS pThis, void *p
     {
         m = vboxNetFltFreeBSDSGMBufFromSG(pThis, pSG);
         if (m == NULL)
+        {
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_MEMORY;
+        }
         m = m_pullup(m, ETHER_HDR_LEN);
         if (m == NULL)
+        {
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_MEMORY;
+        }
         /*
          * Delivering packets to the host will be captured by the
          * input hook. Tag the packet with a mbuf tag so that we
@@ -556,6 +833,7 @@ int vboxNetFltPortOsXmit(PVBOXNETFLTINS pThis, void *p
         if (mtag == NULL)
         {
             m_freem(m);
+            IPRT_FREEBSD_RESTORE_EFL_AC();
             return VERR_NO_MEMORY;
         }
 
@@ -566,6 +844,7 @@ int vboxNetFltPortOsXmit(PVBOXNETFLTINS pThis, void *p
         ifp->if_input(ifp, m);
     }
     VBOXCURVNET_RESTORE();
+    IPRT_FREEBSD_RESTORE_EFL_AC();
     return VINF_SUCCESS;
 }
 
@@ -576,9 +855,43 @@ static bool vboxNetFltFreeBsdIsPromiscuous(PVBOXNETFLT
     return (pThis->u.s.flags & IFF_PROMISC) ? true : false;
 }
 
+static bool vboxNetFltFreeBSDNodeName(const char *ifname, char *output, size_t len)
+{
+    size_t i;
+
+    /* rewrite all characters in netgraph's reserved set '[]:.' to '_' */
+    for (i = 0; i < len; i++)
+    {
+        char c = ifname[i];
+
+        switch (c)
+        {
+            case '[':
+            case ']':
+            case ':':
+            case '.':
+                output[i] = '_';
+                break;
+            default:
+                output[i] = c;
+                break;
+        }
+
+        if (c == '\0')
+            break;
+    }
+
+    if (ifname[i] != '\0')
+        return false;
+
+    return true;
+}
+
 int vboxNetFltOsInitInstance(PVBOXNETFLTINS pThis, void *pvContext)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
     char nam[NG_NODESIZ];
+    char nam_if[IFNAMSIZ];
     struct ifnet *ifp;
     node_p node;
 
@@ -586,11 +899,24 @@ int vboxNetFltOsInitInstance(PVBOXNETFLTINS pThis, voi
     NOREF(pvContext);
     ifp = ifunit(pThis->szName);
     if (ifp == NULL)
+    {
+        IPRT_FREEBSD_RESTORE_EFL_AC();
         return VERR_INTNET_FLT_IF_NOT_FOUND;
+    }
 
+    /* Rewrite netgraph-reserved characters in the interface name to produce
+     * our node's name suffix */
+    if (!vboxNetFltFreeBSDNodeName(pThis->szName, nam_if, sizeof(nam_if)))
+    {
+        IPRT_FREEBSD_RESTORE_EFL_AC();
+        return VERR_INTNET_FLT_IF_NOT_FOUND;
+    }
+
     /* Create a new netgraph node for this instance */
-    if (ng_make_node_common(&ng_vboxnetflt_typestruct, &node) != 0)
+    if (ng_make_node_common(&ng_vboxnetflt_typestruct, &node) != 0) {
+        IPRT_FREEBSD_RESTORE_EFL_AC();
         return VERR_INTERNAL_ERROR;
+    }
 
     RTSpinlockAcquire(pThis->hSpinlock);
 
@@ -599,6 +925,10 @@ int vboxNetFltOsInitInstance(PVBOXNETFLTINS pThis, voi
     bcopy(IF_LLADDR(ifp), &pThis->u.s.MacAddr, ETHER_ADDR_LEN);
     ASMAtomicUoWriteBool(&pThis->fDisconnectedFromHost, false);
 
+    /* Initialize synchronous request state */
+    mtx_init(&pThis->u.s.sreq_lck, "vboxnetflt ng_recv", NULL, MTX_DEF|MTX_NEW);
+    STAILQ_INIT(&pThis->u.s.sreq_list);
+
     /* Initialize deferred input queue */
     bzero(&pThis->u.s.inq, sizeof(struct ifqueue));
     mtx_init(&pThis->u.s.inq.ifq_mtx, "vboxnetflt inq", NULL, MTX_SPIN);
@@ -614,7 +944,7 @@ int vboxNetFltOsInitInstance(PVBOXNETFLTINS pThis, voi
     NG_NODE_SET_PRIVATE(node, pThis);
 
     /* Attempt to name it vboxnetflt_<ifname> */
-    snprintf(nam, NG_NODESIZ, "vboxnetflt_%s", pThis->szName);
+    snprintf(nam, NG_NODESIZ, "vboxnetflt_%s", nam_if);
     ng_name_node(node, nam);
 
     /* Report MAC address, promiscuous mode and GSO capabilities. */
@@ -630,12 +960,14 @@ int vboxNetFltOsInitInstance(PVBOXNETFLTINS pThis, voi
         vboxNetFltRelease(pThis, true /*fBusy*/);
     }
     VBOXCURVNET_RESTORE();
+    IPRT_FREEBSD_RESTORE_EFL_AC();
 
     return VINF_SUCCESS;
 }
 
 bool vboxNetFltOsMaybeRediscovered(PVBOXNETFLTINS pThis)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
     struct ifnet *ifp, *ifp0;
 
     ifp = ASMAtomicUoReadPtrT(&pThis->u.s.ifp, struct ifnet *);
@@ -652,6 +984,7 @@ bool vboxNetFltOsMaybeRediscovered(PVBOXNETFLTINS pThi
         pThis->u.s.node = NULL;
     }
     VBOXCURVNET_RESTORE();
+    IPRT_FREEBSD_RESTORE_EFL_AC();
 
     if (ifp0 != NULL)
     {
@@ -664,6 +997,7 @@ bool vboxNetFltOsMaybeRediscovered(PVBOXNETFLTINS pThi
 
 void vboxNetFltOsDeleteInstance(PVBOXNETFLTINS pThis)
 {
+    IPRT_FREEBSD_SAVE_EFL_AC();
 
     taskqueue_drain(taskqueue_fast, &pThis->u.s.tskin);
     taskqueue_drain(taskqueue_fast, &pThis->u.s.tskout);
@@ -676,106 +1010,347 @@ void vboxNetFltOsDeleteInstance(PVBOXNETFLTINS pThis)
         ng_rmnode_self(pThis->u.s.node);
     VBOXCURVNET_RESTORE();
     pThis->u.s.node = NULL;
+
+    /* Must be done *after* the node has been destroyed, as any incoming
+     * netgraph replies will need to acquire this lock. */
+    mtx_destroy(&pThis->u.s.sreq_lck);
+
+    /* Not possible unless a state management bug in VBoxNetFlt triggers
+     * instance deletion while also blocked in another thread on an active
+     * call into our instance */
+    KASSERT(STAILQ_EMPTY(&pThis->u.s.sreq_list),
+        ("incomplete synchronous netgraph requests"));
+
+    IPRT_FREEBSD_RESTORE_EFL_AC();
 }
 
 int vboxNetFltOsPreInitInstance(PVBOXNETFLTINS pThis)
 {
-
     pThis->u.s.ifp = NULL;
     pThis->u.s.flags = 0;
     pThis->u.s.node = NULL;
     return VINF_SUCCESS;
 }
 
-void vboxNetFltPortOsSetActive(PVBOXNETFLTINS pThis, bool fActive)
+/**
+ * Fetch the list of netgraph nodes.
+ *
+ * On success, the caller assumes ownership of the list of node names, and
+ * is responsible for deallocating it via vboxNetFltFreeBSDFreeNodeList().
+ */
+int vboxNetFltFreeBSDListNodes(PVBOXNETFLTINS pThis, node_p node,
+    struct namelist **names)
 {
-    struct ifnet *ifp;
-    struct ifreq ifreq;
+    struct ng_mesg *msg;
+    struct namelist *nl;
+    size_t len, ni_len, ni_count;
     int error;
-    node_p node;
+
+    /* Fetch the node list */
+    error = vboxnetflt_sreq_simple_rpc(pThis, node, NG_NODE_ID(node),
+        NGM_GENERIC_COOKIE, NGM_LISTNODES, &msg);
+    if (error)
+    {
+        LogRel(("VBoxNetFlt: NGM_LISTNODES failed (%d)\n", error));
+        return error;
+    }
+
+    /* Must be at least large enough for the fixed entry count. */
+    nl = (struct namelist *)msg->data;
+    if (msg->header.arglen < sizeof(*nl)) {
+        error = ENXIO;
+        goto cleanup;
+    }
+
+    /* Must be large enough for the header + (numnames * sizeof(nodeinfo)) */
+    ni_count = nl->numnames;
+    ni_len = msg->header.arglen - sizeof(*nl);
+
+    if (SIZE_MAX / sizeof(nl->nodeinfo[0]) < ni_count) {
+        /* Would overflow */
+        error = ENXIO;
+        goto cleanup;
+    }
+
+    if (ni_len < (sizeof(nl->nodeinfo[0]) * ni_count)) {
+        error = ENXIO;
+        goto cleanup;
+    }
+
+    /* Copy out the result */
+    nl = malloc(msg->header.arglen, M_VBOXNETFLT, M_NOWAIT);
+    if (nl == NULL) {
+
+        error = ENOMEM;
+        goto cleanup;
+    }
+    memcpy(nl, msg->data, msg->header.arglen);
+
+    *names = nl;
+    error = 0;
+
+    /* fallthrough */
+
+cleanup:
+    NG_FREE_MSG(msg);
+    return (error);
+}
+
+int vboxNetFltFreeBSDFreeNodeList(struct namelist *names)
+{
+    free(names, M_VBOXNETFLT);
+}
+
+/**
+ * Fetch the interface name from an ng_ether node.
+ */
+int vboxNetFltFreeBSDEtherGetIfName(PVBOXNETFLTINS pThis, node_p node,
+    ng_ID_t ether_node, char *buf, size_t buflen)
+{
     struct ng_mesg *msg;
+    char *ifname;
+    size_t namelen;
+    int error;
+
+    /* Fetch the interface name */
+    error = vboxnetflt_sreq_simple_rpc(pThis, node, ether_node,
+        NGM_ETHER_COOKIE, NGM_ETHER_GET_IFNAME, &msg);
+    if (error)
+    {
+        /* May fail for innocuous reasons; e.g. if the node disappeared while
+         * iterating the node list, etc. */
+        Log(("VBoxNetFlt: NGM_ETHER_GET_IFNAME failed (%d)\n", error));
+        return (error);
+    }
+
+    ifname = (const char *)msg->data;
+    namelen = strnlen(ifname, msg->header.arglen);
+
+    /* Must be NUL-terminated */
+    if (namelen == msg->header.arglen) {
+        NG_FREE_MSG(msg);
+        return (ENXIO);
+    }
+
+    /* Copy out the result */
+    if (buflen < namelen+1) {
+        NG_FREE_MSG(msg);
+        return (ENOMEM);
+    }
+
+    strlcpy(buf, ifname, buflen);
+    NG_FREE_MSG(msg);
+
+    return (0);
+}
+
+/**
+ * Find the ng_ether node correspnding to our target interface.
+ */
+static int vboxNetFltFreeBSDFindEtherNode(PVBOXNETFLTINS pThis, node_p node,
+    ng_ID_t *id)
+{
+    struct namelist *nl;
+    ng_ID_t ether_node;
+    int error;
+
+    /* Fetch the full node list */
+    if ((error = vboxNetFltFreeBSDListNodes(pThis, node, &nl))) {
+        LogRel(("VBoxNetFlt: failed to fetch node list (%d)\n", error));
+        return (error);
+    }
+
+    /* Look for a matching NG_ETHER node */
+    ether_node = 0;
+    for (uint32_t i = 0; i < nl->numnames; i++) {
+        struct nodeinfo *ni;
+        char ifname[IF_NAMESIZE];
+
+        ni = &nl->nodeinfo[i];
+
+        /* We only care about ng_ether nodes */
+        if (strcmp(ni->type, NG_ETHER_NODE_TYPE) != 0)
+            continue;
+
+        /* Fetch the interface name */
+        error = vboxNetFltFreeBSDEtherGetIfName(pThis, node, ni->id, ifname,
+            sizeof(ifname));
+        if (error) {
+            Log(("VBoxNetFlt: failed to fetch interface name (%d)\n",
+                error));
+            continue;
+        }
+
+        /* Skip non-matching interfaces */
+        if (strcmp(ifname, pThis->szName) != 0)
+            continue;
+
+        *id = ether_node;
+        vboxNetFltFreeBSDFreeNodeList(nl);
+        return (0);
+    }
+
+    vboxNetFltFreeBSDFreeNodeList(nl);
+    return (error);
+}
+
+/**
+ * Send an NGM_CONNECT message.
+ */
+static int vboxNetFltFreeBSDConnectHook(PVBOXNETFLTINS pThis, node_p node,
+    ng_ID_t peer, const char *ourhook, const char *peerhook)
+{
+    struct ng_mesg *msg;
     struct ngm_connect *con;
+    int error;
+
+    if (strlen(ourhook) > NG_HOOKSIZ || strlen(peerhook) > NG_HOOKSIZ)
+        return (EINVAL);
+
+    NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_CONNECT,
+        sizeof(struct ngm_connect), M_NOWAIT);
+    if (msg == NULL)
+        return (ENOMEM);
+
+    con = (struct ngm_connect *)msg->data;
+    snprintf(con->path, NG_PATHSIZ, "[%x]:", NG_NODE_ID(node));
+    strlcpy(con->ourhook, ourhook, NG_HOOKSIZ);
+    strlcpy(con->peerhook, peerhook, NG_HOOKSIZ);
+
+    NG_SEND_MSG_ID(error, node, msg, peer, 0);
+    return (error);
+}
+
+/**
+ * Send an NGM_RMHOOK message.
+ */
+static int vboxNetFltFreeBSDRmHook(PVBOXNETFLTINS pThis, node_p node,
+    const char *ourhook)
+{
+    struct ng_mesg *msg;
     struct ngm_rmhook *rm;
-    char path[NG_PATHSIZ];
+    int error;
 
-    Log(("%s: fActive:%d\n", __func__, fActive));
+    if (strlen(ourhook) > NG_HOOKSIZ)
+        return (EINVAL);
 
-    ifp = ASMAtomicUoReadPtrT(&pThis->u.s.ifp, struct ifnet *);
-    VBOXCURVNET_SET(ifp->if_vnet);
-    node = ASMAtomicUoReadPtrT(&pThis->u.s.node, node_p);
+    NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_RMHOOK,
+        sizeof(struct ngm_rmhook), M_NOWAIT);
+    if (msg == NULL)
+        return (ENOMEM);
 
-    memset(&ifreq, 0, sizeof(struct ifreq));
-    /* Activate interface */
-    if (fActive)
-    {
-        pThis->u.s.flags = ifp->if_flags;
-        ifpromisc(ifp, 1);
+    rm = (struct ngm_rmhook *)msg->data;
+    strlcpy(rm->ourhook, ourhook, NG_HOOKSIZ);
 
-        /* ng_ether nodes are named after the interface name */
-        snprintf(path, sizeof(path), "%s:", ifp->if_xname);
+    NG_SEND_MSG_PATH(error, node, msg, ".:", 0);
+    return (error);
+}
 
-        /*
-         * Send a netgraph connect message to the ng_ether node
-         * assigned to the bridged interface. Connecting
-         * the hooks 'lower' (ng_ether) to out 'input'.
-         */
-        NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_CONNECT,
-            sizeof(struct ngm_connect), M_NOWAIT);
-        if (msg == NULL)
-            return;
-        con = (struct ngm_connect *)msg->data;
-        snprintf(con->path, NG_PATHSIZ, "vboxnetflt_%s:", ifp->if_xname);
-        strlcpy(con->ourhook, "lower", NG_HOOKSIZ);
-        strlcpy(con->peerhook, "input", NG_HOOKSIZ);
-        NG_SEND_MSG_PATH(error, node, msg, path, 0);
+/**
+ * Called by vboxNetFltPortOsSetActive() to activate the filter driver instance.
+ */
+static int vboxNetFltFreeBSDActivate(PVBOXNETFLTINS pThis, struct ifnet *ifp,
+    node_p node)
+{
+    ng_ID_t ether_node;
+    int error;
 
-        /*
-         * Do the same for the hooks 'upper' (ng_ether) and our
-         * 'output' hook.
-         */
-        NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_CONNECT,
-            sizeof(struct ngm_connect), M_NOWAIT);
-        if (msg == NULL)
-            return;
-        con = (struct ngm_connect *)msg->data;
-        snprintf(con->path, NG_PATHSIZ, "vboxnetflt_%s:",
-            ifp->if_xname);
-        strlcpy(con->ourhook, "upper", sizeof(con->ourhook));
-        strlcpy(con->peerhook, "output", sizeof(con->peerhook));
-        NG_SEND_MSG_PATH(error, node, msg, path, 0);
+    pThis->u.s.flags = ifp->if_flags;
+    ifpromisc(ifp, 1);
+
+    /* Locate the interface's corresponding NG_ETHER netgraph node */
+    if ((error = vboxNetFltFreeBSDFindEtherNode(pThis, node, &ether_node))) {
+        LogRel(("VBoxNetFlt: Failed to locate netgraph node for %s: %d\n",
+            ifp->if_xname, error));
+        return (error);
     }
-    else
-    {
-        /* De-activate interface */
-        pThis->u.s.flags = 0;
-        ifpromisc(ifp, 0);
 
-        /* Disconnect msgs are addressed to ourself */
-        snprintf(path, sizeof(path), "vboxnetflt_%s:", ifp->if_xname);
+    /*
+     * Send a netgraph connect message to the ng_ether node
+     * assigned to the bridged interface. Connecting
+     * the hooks 'lower' (ng_ether) to out 'input'.
+     */
+    error = vboxNetFltFreeBSDConnectHook(pThis, node, ether_node,
+        NG_ETHER_HOOK_LOWER, NG_VBOXNETFLT_HOOK_IN);
+    if (error) {
+        LogRel(("VBoxNetFlt: Failed to connect netgraph input hook for %s:"
+            " %d\n", ifp->if_xname, error));
+        return (error);
+    }
 
-        /*
-         * Send a netgraph message to disconnect our 'input' hook
-         */
-        NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_RMHOOK,
-            sizeof(struct ngm_rmhook), M_NOWAIT);
-        if (msg == NULL)
-            return;
-        rm = (struct ngm_rmhook *)msg->data;
-        strlcpy(rm->ourhook, "input", NG_HOOKSIZ);
-        NG_SEND_MSG_PATH(error, node, msg, path, 0);
+    /*
+     * Do the same for the hooks 'upper' (ng_ether) and our
+     * 'output' hook.
+     */
+    error = vboxNetFltFreeBSDConnectHook(pThis, node, ether_node,
+        NG_ETHER_HOOK_UPPER, NG_VBOXNETFLT_HOOK_OUT);
+    if (error) {
+        LogRel(("VBoxNetFlt: Failed to connect netgraph output hook for %s:"
+            " %d\n", ifp->if_xname, error));
+        return (error);
+    }
 
-        /*
-         * Send a netgraph message to disconnect our 'output' hook
-         */
-        NG_MKMESSAGE(msg, NGM_GENERIC_COOKIE, NGM_RMHOOK,
-            sizeof(struct ngm_rmhook), M_NOWAIT);
-        if (msg == NULL)
-            return;
-        rm = (struct ngm_rmhook *)msg->data;
-        strlcpy(rm->ourhook, "output", NG_HOOKSIZ);
-        NG_SEND_MSG_PATH(error, node, msg, path, 0);
+    return (0);
+}
+
+/**
+ * Called by vboxNetFltPortOsSetActive() to deactivate the filter driver
+ * instance.
+ */
+static int vboxNetFltFreeBSDDeactivate(PVBOXNETFLTINS pThis, struct ifnet *ifp,
+    node_p node)
+{
+    int ng_error;
+    int error = 0;
+
+    pThis->u.s.flags = 0;
+    ifpromisc(ifp, 0);
+
+    /*
+     * Send a netgraph message to disconnect our input and output hooks; on
+     * failure, we still attempt to disconnect any remaining hooks.
+     */
+    ng_error = vboxNetFltFreeBSDRmHook(pThis, node, NG_VBOXNETFLT_HOOK_IN);
+    if (ng_error) {
+        LogRel(("VBoxNetFlt: Failed to disconnect netgraph input hook for %s:"
+            " %d\n", ifp->if_xname, error));
+        error = ng_error;
     }
+
+    ng_error = vboxNetFltFreeBSDRmHook(pThis, node, NG_VBOXNETFLT_HOOK_OUT);
+    if (ng_error) {
+        LogRel(("VBoxNetFlt: Failed to disconnect netgraph output hook for %s:"
+            " %d\n", ifp->if_xname, error));
+        error = ng_error;
+    }
+
+    return (error);
+}
+
+void vboxNetFltPortOsSetActive(PVBOXNETFLTINS pThis, bool fActive)
+{
+    IPRT_FREEBSD_SAVE_EFL_AC();
+    struct ifnet *ifp;
+    int error;
+    node_p node;
+
+    Log(("%s: fActive:%d\n", __func__, fActive));
+
+    ifp = ASMAtomicUoReadPtrT(&pThis->u.s.ifp, struct ifnet *);
+    VBOXCURVNET_SET(ifp->if_vnet);
+    node = ASMAtomicUoReadPtrT(&pThis->u.s.node, node_p);
+
+    if (fActive)
+        error = vboxNetFltFreeBSDActivate(pThis, ifp, node);
+    else
+        error = vboxNetFltFreeBSDDeactivate(pThis, ifp, node);
+
+    if (error) 
+        LogRel(("VBoxNetFlt: Failed to %s %s filter driver: %d\n",
+            (fActive ? "activate" : "deactivate"), ifp->if_xname, error));
+
     VBOXCURVNET_RESTORE();
+    IPRT_FREEBSD_RESTORE_EFL_AC();
 }
 
 int vboxNetFltOsDisconnectIt(PVBOXNETFLTINS pThis)
