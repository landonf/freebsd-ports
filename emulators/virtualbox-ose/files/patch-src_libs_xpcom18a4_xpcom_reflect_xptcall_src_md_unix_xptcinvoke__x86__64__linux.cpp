--- src/libs/xpcom18a4/xpcom/reflect/xptcall/src/md/unix/xptcinvoke_x86_64_linux.cpp.orig	2019-10-10 12:37:46.000000000 -0600
+++ src/libs/xpcom18a4/xpcom/reflect/xptcall/src/md/unix/xptcinvoke_x86_64_linux.cpp	2020-04-15 13:07:43.001407000 -0600
@@ -143,7 +143,7 @@
     if (nr_stack)
         nr_stack = (nr_stack + 1) & ~1;
 
-#ifndef VBOX_WITH_GCC_SANITIZER
+#ifndef VBOX_WITH_GCC_SANITIZER && !defined(__clang__)
     // Load parameters to stack, if necessary
     PRUint64 *stack = (PRUint64 *) __builtin_alloca(nr_stack * 8);
 #else
@@ -154,7 +154,7 @@
 #endif
     PRUint64 gpregs[GPR_COUNT];
     double fpregs[FPR_COUNT];
-#ifndef VBOX_WITH_GCC_SANITIZER
+#ifndef VBOX_WITH_GCC_SANITIZER && !defined(__clang__)
     invoke_copy_to_stack(stack, paramCount, params, gpregs, fpregs);
 #else
     invoke_copy_to_stack(stack.stack, paramCount, params, gpregs, fpregs);
@@ -216,7 +216,7 @@
     methodAddress += 8 * methodIndex;
     methodAddress = *((PRUint64 *)methodAddress);
     
-#ifndef VBOX_WITH_GCC_SANITIZER
+#ifndef VBOX_WITH_GCC_SANITIZER && !defined(__clang__)
     typedef PRUint32 (*Method)(PRUint64, PRUint64, PRUint64, PRUint64, PRUint64, PRUint64);
     PRUint32 result = ((Method)methodAddress)(a0, a1, a2, a3, a4, a5);
 #else
