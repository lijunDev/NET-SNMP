#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>
#include "Test.h"
const char *app_name = "snmpd";

static int keep_running;

RETSIGTYPE
stop_server(int a) {
    keep_running = 0;
}

static void
version(void)
{
    fprintf(stderr, "NET-SNMP version: %s\n", netsnmp_get_version());
    exit(0);
}

int
main (int argc, char **argv) {

    int agentx_subagent=0; /* change this if you want to be a SNMP master agent */
    int background = 0; /* change this if you want to run in the background */
    int syslog = 1; /* change this if you want to use syslog */
    int arg;
    int dont_fork = 0;
    int ret;

    while ((arg = getopt(argc, argv, "vdfL:")) != -1) {
        switch (arg) {
        case 'v':
            version();
            break;
        case 'd':
            netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
                                   NETSNMP_DS_LIB_DUMP_PACKET, 1);
            break;
        case 'f':
            dont_fork = 1;
            break;
        case 'L':
            if (snmp_log_options(optarg, argc, argv) < 0)
                return -1;
        }
    }
    /* print log errors to syslog or stderr */
    snmp_enable_filelog("../../test.log",
                            netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID,
                                                   NETSNMP_DS_LIB_APPEND_LOGFILES));
    if (syslog)
        snmp_enable_calllog();
    else
        snmp_enable_stderrlog();

    /* we're an agentx subagent? */
    if (agentx_subagent) {
        /* make us a agentx client. */
        netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
    }

    /* run in background, if requested */
    if (background && netsnmp_daemonize(1, !syslog))
        exit(1);

    /* Initialize tcpip, if necessary */
    SOCK_STARTUP;

    /* Initialize the agent library */
    init_agent(app_name); // 配置文件名

    /* Initialize our mib code here */
    printf("Before init Test\n");

    // 加载节点信息
    init_Test();

    printf("End init Test");

    /* initialize vacm/usm access control  */
    if (!agentx_subagent) {
        void  init_vacm_vars();
        void  init_usmUser();
    }

    /* Example-demon will be used to read example-demon.conf files. */
    init_snmp(app_name);

    /* If we're going to be a snmp master agent, initial the ports */
    if (!agentx_subagent)
        init_master_agent();  /* open the port to listen on (defaults to udp:161) */

    if(!dont_fork) {
        int quit = ! netsnmp_ds_get_boolean(NETSNMP_DS_APPLICATION_ID,
                                            NETSNMP_DS_AGENT_QUIT_IMMEDIATELY);
        ret = netsnmp_daemonize(quit,
#ifndef NETSNMP_FEATURE_REMOVE_LOGGING_STDIO
                                snmp_stderrlog_status()
#else /* NETSNMP_FEATURE_REMOVE_LOGGING_STDIO */
                                0
#endif /* NETSNMP_FEATURE_REMOVE_LOGGING_STDIO */
            );
        /*
         * xxx-rks: do we care if fork fails? I think we should...
         */
        if(ret != 0) {
            snmp_log(LOG_ERR, "Server Exiting with code 1\n");
            goto out;
        }
    }

    /* In case we recevie a request to stop (kill -TERM or kill -INT) */
    keep_running = 1;
    signal(SIGTERM, stop_server);

    signal(SIGINT, stop_server);

    snmp_log(LOG_INFO,"MyAgent is up and running.\n");

    /* your main loop here... */
    while(keep_running) {
        /* if you use select(), see snmp_select_info() in snmp_api(3) */
        /*     --- OR ---  */
        agent_check_and_process(1); /* 0 == don't block */

    }

    /* at shutdown time */
    snmp_shutdown(app_name);
    SOCK_CLEANUP;
    return 0;
out:
    SOCK_CLEANUP;
    return 1;
}