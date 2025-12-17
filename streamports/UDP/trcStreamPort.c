/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
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

#if (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER == 0)
#error This StreamPort requires that TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER is enabled!
#endif

/* udp includes - for lwIP in this case */
#include <lwip/sockets.h>
#include <lwip/errno.h>

int sock = { -1 };
struct sockaddr_in address_out[TRC_CFG_CORE_COUNT];

static TraceStreamPortBuffer_t* pxStreamPortUDP TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static int32_t prvSocketInitialize();

static int32_t prvSocketInitialize()
{
	int flags = 0;
	int i;
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

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		address_out[i].sin_family = AF_INET;
		address_out[i].sin_port = htons(TRC_CFG_STREAM_PORT_UDP_PORT + i);
		address_out[i].sin_addr.s_addr = inet_addr(TRC_CFG_STREAM_PORT_UDP_ADDRESS);
	}

	return 0;
}

int32_t prvTraceUdpWrite( void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten )
{
	if (prvSocketInitialize() != 0)	/* This ensures the socket is OK */
		return -1;
	
	if (piBytesWritten == (void*)0)
	{
		return -1;
	}
	
	*piBytesWritten = sendto( sock, pvData, uiSize, 0, (struct sockaddr *)&address_out[uiChannel], sizeof(address_out[uiChannel]) );
	
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

int32_t prvTraceUdpRead( void* pvData, uint32_t uiSize, int32_t* piBytesRead )
{
	struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

	if (prvSocketInitialize() != 0)	/* This ensures the socket is OK */
		return -1;
	
	if (piBytesRead == (void*)0)
		return -1;

	*piBytesRead = recvfrom( sock, pvData, uiSize, 0, (struct sockaddr *)&source_addr, &addr_len );
	
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

/************** MODIFY THE ABOVE PART TO USE YOUR UDP STACK ****************/

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortUDP = pxBuffer;

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	if (sock >= 0)
	{
		close(sock);
	}

	return TRC_SUCCESS;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
