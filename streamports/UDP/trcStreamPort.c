/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming, used by the "stream ports" 
 * for reading and writing data to the interface.
 * Existing ports can easily be modified to fit another setup, e.g., a 
 * different UDP stack, or to define your own stream port.
 */

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)  
	
/* UDP includes - for lwIP in this case */
#include <lwip/udp.h>
#include <lwip/errno.h>
#include <string.h>

int sock = -1, new_sd = -1;
int flags = 0;
int remoteSize;
struct udp_pcb *pxPCB_send = (void*)0;
struct udp_pcb *pxPCB_recv = (void*)0;

typedef struct TraceStreamPortUDP
{
#if (TRC_USE_INTERNAL_BUFFER)
	uint8_t buffer[(TRC_ALIGNED_STREAM_PORT_BUFFER_SIZE)];
#else
	TraceUnsignedBaseType_t buffer[1];
#endif
	uint8_t recv_buffer[32];
	uint32_t recv_len;
} TraceStreamPortUDP_t;

static TraceStreamPortUDP_t* pxStreamPortUDP TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static int32_t prv_udp_send(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);
static int32_t prv_udp_recv(void* pvData, uint32_t uiSize, int32_t* piBytesRead);
static int32_t prv_connect_pcb();
static int32_t prv_bind_pcb();
static void prv_disconnect_pcb();

static void prv_udp_recv_callback(void* arg, struct udp_pcb* pvPCB, struct pbuf* pvBuf, const ip_addr_t* addr, u16_t port);

static void prv_udp_recv_callback(void* arg, struct udp_pcb* pvPCB, struct pbuf* pvBuf, const ip_addr_t* addr, u16_t port)
{
	TraceUnsignedBaseType_t len;
	
	if(pvBuf == (void*)0)
	{
		return;
	}
	
	if (pxStreamPortUDP->recv_len > 0)
	{
		/* We already have received something, so we ignore this */
		pbuf_free(pvBuf);
		return;
	}
	
	len = pvBuf->tot_len;
	if (len > sizeof(pxStreamPortUDP->recv_buffer))
		len = sizeof(pxStreamPortUDP->recv_buffer);
	
	memcpy(pxStreamPortUDP->recv_buffer, pvBuf->payload, len);
	pxStreamPortUDP->recv_len = len;
	
	pbuf_free(pvBuf);
}

static int32_t prv_udp_send( void* pvData, uint32_t uiSize, int32_t* piBytesWritten )
{
	err_t err;
	struct pbuf *pvBuf;
	
  if (pxPCB_send == (void*)0)
    return -1;
  
  if (piBytesWritten == (void*)0)
	return -1;
  
	*piBytesWritten = 0;
  
  pvBuf = pbuf_alloc(PBUF_TRANSPORT, uiSize, PBUF_RAM);
  
  if (pvBuf == (void*)0)
  {
	  return 0;
  }
  
  memcpy(pvBuf->payload, pvData, uiSize);
  
  LOCK_TCPIP_CORE();
  err = udp_send(pxPCB_send, pvBuf);
  UNLOCK_TCPIP_CORE();
  
  pbuf_free(pvBuf);
  
  if (err != ERR_OK)
  {
		prv_disconnect_pcb();
		return -1;
  }
  
  *piBytesWritten = uiSize;
  return 0;
}

static int32_t prv_udp_recv( void* pvData, uint32_t uiSize, int32_t* piBytesRead )
{
	TRC_ASSERT(piBytesRead != (void*)0);
  
	if (pxStreamPortUDP->recv_len > 0)
	{
		memcpy(pvData, pxStreamPortUDP->recv_buffer, pxStreamPortUDP->recv_len);
	}
	*piBytesRead = pxStreamPortUDP->recv_len;
	
	/* Clear this so we can receive new data */
	pxStreamPortUDP->recv_len = 0;
	
	return 0;
}

static int32_t prv_connect_pcb()
{
	ip_addr_t addr;
	err_t err;
	
	if (pxPCB_send != (void*)0)
		return 0;
  
  LOCK_TCPIP_CORE();
  pxPCB_send = udp_new();
  UNLOCK_TCPIP_CORE();
  
  if (pxPCB_send == (void*)0)
    return -1;
  
  	ipaddr_aton(TRC_CFG_STREAM_PORT_UDP_ADDRESS, &addr);

  LOCK_TCPIP_CORE();
  err = udp_connect(pxPCB_send, &addr, (uint16_t)(TRC_CFG_STREAM_PORT_UDP_PORT));
  UNLOCK_TCPIP_CORE();
  
  if (err != ERR_OK)
  {
		udp_remove(pxPCB_send);
		pxPCB_send = (void*)0;
		return -1;
  }

  return 0;
}

static int32_t prv_bind_pcb()
{
	ip_addr_t addr;
	err_t err;
	
	if (pxPCB_recv != (void*)0)
		return 0;

	LOCK_TCPIP_CORE();
	pxPCB_recv = udp_new();
	UNLOCK_TCPIP_CORE();
	
	if (pxPCB_recv == (void*)0)
		return -1;
  
  	ipaddr_aton("0.0.0.0", &addr);
  
	LOCK_TCPIP_CORE();
	err = udp_bind(pxPCB_recv, &addr, (uint16_t)(TRC_CFG_STREAM_PORT_UDP_PORT));
	UNLOCK_TCPIP_CORE();
  
	if (err != ERR_OK)
	{
		udp_remove(pxPCB_recv);
		pxPCB_recv = (void*)0;
		return -1;
	}
  
	LOCK_TCPIP_CORE();
	udp_recv(pxPCB_recv, prv_udp_recv_callback, (void*)0);
	UNLOCK_TCPIP_CORE();

  return 0;
}

static void prv_disconnect_pcb()
{
	if (pxPCB_send != (void*)0)
	{
		LOCK_TCPIP_CORE();
		udp_disconnect(pxPCB_send);
		udp_remove(pxPCB_send);
		pxPCB_send = (void*)0;
		UNLOCK_TCPIP_CORE();
	}
	
	if (pxPCB_recv != (void*)0)
	{
		LOCK_TCPIP_CORE();
		udp_disconnect(pxPCB_recv);
		udp_remove(pxPCB_recv);
		pxPCB_recv = (void*)0;
		UNLOCK_TCPIP_CORE();
	}
}

/************** MODIFY THE ABOVE PART TO USE YOUR UDP STACK ****************/

int32_t prvTraceUdpWrite(void* pvData, uint32_t uiSize, int32_t *piBytesWritten)
{
	prv_connect_pcb();
	
	if (uiSize > 65024) /* We have to limit it to UDP max */
		uiSize = 65024;
	
    return prv_udp_send(pvData, uiSize, piBytesWritten);
}

int32_t prvTraceUdpRead(void* pvData, uint32_t uiSize, int32_t *piBytesRead)
{
    prv_bind_pcb();
	
    return prv_udp_recv(pvData, uiSize, piBytesRead);
}

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceStreamPortBuffer_t, TraceStreamPortUDP_t);

	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortUDP = (TraceStreamPortUDP_t*)pxBuffer;
	
	(void)pxStreamPortUDP;

#if (TRC_USE_INTERNAL_BUFFER == 1)
	return xTraceInternalEventBufferInitialize(pxStreamPortUDP->buffer, sizeof(pxStreamPortUDP->buffer));
#else
	return TRC_SUCCESS;
#endif
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	prv_disconnect_pcb();

	return TRC_SUCCESS;
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
