/*
 * Trace Recorder for Tracealyzer v4.10.2
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

/* udp includes - for lwIP in this case */
#include <lwip/sockets.h>
#include <lwip/errno.h>

int sock = -1;
int remoteSize;
struct sockaddr_in address_out;

typedef struct TraceStreamPortUDP
{
#if (TRC_USE_INTERNAL_BUFFER)
	uint8_t buffer[(TRC_ALIGNED_STREAM_PORT_BUFFER_SIZE)];
#else
	TraceUnsignedBaseType_t buffer[1];
#endif
} TraceStreamPortUDP_t;

static TraceStreamPortUDP_t* pxStreamPortUDP TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static int32_t prvSocketSend(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);
static int32_t prvSocketReceive(void* pvData, uint32_t uiSize, int32_t* piBytesRead);
static int32_t prvSocketInitialize(void);

static int32_t prvSocketSend( void* pvData, uint32_t uiSize, int32_t* piBytesWritten )
{
	if (sock < 0)
	{
		return -1;
	}
	
	if (piBytesWritten == (void*)0)
	{
		return -1;
	}
	
	*piBytesWritten = sendto( sock, pvData, uiSize, 0, (struct sockaddr *)&address_out, sizeof(address_out) );
	
	if (*piBytesWritten < 0)
	{
		*piBytesWritten = 0;
		
		/* EWOULDBLOCK may be expected when buffers are full */
		if (errno != EWOULDBLOCK)
		{
			close(sock);
			sock = -1;
			return -1;
		}
	}
	
	return 0;
}

static int32_t prvSocketReceive( void* pvData, uint32_t uiSize, int32_t* piBytesRead )
{
	if (sock < 0)
		return -1;
	
	if (piBytesRead == (void*)0)
		return -1;

	*piBytesRead = recv( sock, pvData, uiSize, 0 );
	
	if (*piBytesRead < 0)
	{
		*piBytesRead = 0;
		
		/* EWOULDBLOCK may be expected when there is no pvData to receive */
		if (errno != EWOULDBLOCK)
		{
		close(sock);
		sock = -1;
		return -1;
		}
	}

	return 0;
}

static int32_t prvSocketInitialize(void)
{
	int flags = 0;
	struct sockaddr_in address_in;

	if (sock >= 0)
	{
		return 0;
	}
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (sock < 0)
	{
		return -1;
	}

	address_in.sin_family = AF_INET;
	address_in.sin_port = htons(TRC_CFG_STREAM_PORT_UDP_PORT);
	address_in.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sock, (struct sockaddr*)&address_in, sizeof(address_in)) != 0)
	{
		close(sock);
		sock = -1;
		return -1;
	}
	
	flags = fcntl( sock, F_GETFL, 0 );
	fcntl( sock, F_SETFL, flags | O_NONBLOCK );

	address_out.sin_family = AF_INET;
	address_out.sin_port = htons(TRC_CFG_STREAM_PORT_UDP_PORT);
	address_out.sin_addr.s_addr = inet_addr(TRC_CFG_STREAM_PORT_UDP_ADDRESS);

	return 0;
}

/************** MODIFY THE ABOVE PART TO USE YOUR UDP STACK ****************/

int32_t prvTraceUdpWrite(void* pvData, uint32_t uiSize, int32_t *piBytesWritten)
{
	prvSocketInitialize();
	
	return prvSocketSend(pvData, uiSize, piBytesWritten);
}

int32_t prvTraceUdpRead(void* pvData, uint32_t uiSize, int32_t *piBytesRead)
{
	prvSocketInitialize();
			
	return prvSocketReceive(pvData, uiSize, piBytesRead);
}

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceStreamPortBuffer_t, TraceStreamPortUDP_t);

	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortUDP = (TraceStreamPortUDP_t*)pxBuffer;

#if (TRC_USE_INTERNAL_BUFFER == 1)
	return xTraceInternalEventBufferInitialize(pxStreamPortUDP->buffer, sizeof(pxStreamPortUDP->buffer));
#else
	return TRC_SUCCESS;
#endif
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	if (sock >= 0)
	{
		close(sock);
	}

	return TRC_SUCCESS;
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
