--- monitor.h.orig	Tue Jun 11 18:42:49 2002
+++ monitor.h	Sun Jun 30 07:13:09 2002
@@ -50,6 +51,10 @@
 	MONITOR_REQ_RSACHALLENGE, MONITOR_ANS_RSACHALLENGE,
 	MONITOR_REQ_RSARESPONSE, MONITOR_ANS_RSARESPONSE,
 	MONITOR_REQ_PAM_START,
+	MONITOR_REQ_PAM_INIT_CTX, MONITOR_ANS_PAM_INIT_CTX,
+	MONITOR_REQ_PAM_QUERY, MONITOR_ANS_PAM_QUERY,
+	MONITOR_REQ_PAM_RESPOND, MONITOR_ANS_PAM_RESPOND,
+	MONITOR_REQ_PAM_FREE_CTX, MONITOR_ANS_PAM_FREE_CTX,
 	MONITOR_REQ_TERM
 };
 