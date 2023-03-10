/*
 * Note: this file originally auto-generated by mib2c
 * using mib2c.scalar.conf
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "Test.h"
#include <time.h>
/** Initializes the Test module */
void
init_Test(void)
{
    const oid GetTime_oid[] = { 1,3,6,1,4,1,9527,1,1 };

  DEBUGMSGTL(("Test", "Initializing\n"));

    netsnmp_register_scalar(
        netsnmp_create_handler_registration("GetTime", handle_GetTime,
                               GetTime_oid, OID_LENGTH(GetTime_oid),
                               HANDLER_CAN_RONLY
        ));
}

int
handle_GetTime(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
		{
			time_t t;
			time(&t);
			char timeStr[100];
			snprintf(timeStr, 100, ctime(&t));
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                     /* XXX: a pointer to the scalar's data */
									 timeStr,
                                     /* XXX: the length of the data in bytes */
									 strlen(timeStr));
		}
            
        break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_GetTime\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}