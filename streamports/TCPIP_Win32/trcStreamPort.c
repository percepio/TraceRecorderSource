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
 * different TCP/IP stack, or to define your own stream port.
 */

#include<stdio.h>
#include<winsock2.h>

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#pragma comment(lib,"ws2_32.lib") //Winsock Library

static TraceStreamPortBuffer_t* pxStreamPortTCPIP TRC_CFG_RECORDER_DATA_ATTRIBUTE;
static SOCKET server_socket[TRC_CFG_CORE_COUNT] = { (UINT_PTR)0 };
static SOCKET trace_socket[TRC_CFG_CORE_COUNT] = { (UINT_PTR)0 };
struct sockaddr_in server, client;

static int prvInitWinsockIfNeeded(void);
static int prvInitServerSocketIfNeeded(uint32_t uiChannel);
static int prvInitTraceSocketIfNeeded(uint32_t uiChannel);
static void prvCloseAllSockets(void);

static int prvInitWinsockIfNeeded(void)
{
	WSADATA wsa;
	
	if (server_socket[0])
		return 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	return 0;
}

static int prvInitServerSocketIfNeeded(uint32_t uiChannel)
{
	if (prvInitWinsockIfNeeded() < 0)
	{
		return -1;
	}

	if (server_socket[uiChannel])
		return 0;

	if ((server_socket[uiChannel] = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(TRC_CFG_STREAM_PORT_TCPIP_PORT + uiChannel);

	if (bind(server_socket[uiChannel], (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		closesocket(server_socket[uiChannel]);
		WSACleanup();
		server_socket[uiChannel] = (UINT_PTR)0;
		return -1;
	}

	if (listen(server_socket[uiChannel], 3) < 0)
	{
		closesocket(server_socket[uiChannel]);
		WSACleanup();
		server_socket[uiChannel] = (UINT_PTR)0;
		return -1;
	}

	return 0;
}

static int prvInitTraceSocketIfNeeded(uint32_t uiChannel)
{
	int c;

	if (!server_socket[uiChannel])
		return -1;

	if (trace_socket[uiChannel])
		return 0;

	c = sizeof(struct sockaddr_in);
	trace_socket[uiChannel] = accept(server_socket[uiChannel], (struct sockaddr *)&client, &c);
	if (trace_socket[uiChannel] == INVALID_SOCKET)
	{
		trace_socket[uiChannel] = (UINT_PTR)0;
		
		closesocket(server_socket[uiChannel]);
		WSACleanup();
		server_socket[uiChannel] = (UINT_PTR)0;
		
		return -1;
	}

	return 0;
}

int32_t prvTraceWriteToSocket(void* data, uint32_t size, uint32_t uiChannel, int32_t *ptrBytesWritten)
{
	int ret;

	if (prvInitServerSocketIfNeeded(uiChannel) < 0)
	{
		return -1;
	}

	if (prvInitTraceSocketIfNeeded(uiChannel) < 0)
	{
		return -1;
	}

	if (!trace_socket[uiChannel])
	{
		if (ptrBytesWritten != 0)
		{
			*ptrBytesWritten = 0;
		}
		return -1;
	}
	ret = send(trace_socket[uiChannel], data, size, 0);
	if (ret <= 0)
	{
		if (ptrBytesWritten != 0)
		{
			*ptrBytesWritten = 0;
		}

		closesocket(trace_socket[uiChannel]);
		trace_socket[uiChannel] = (UINT_PTR)0;
		return ret;
	}

	if (ptrBytesWritten != 0)
	{
		*ptrBytesWritten = ret;
	}
	
	return 0;
}

int32_t prvTraceReadFromSocket(void* data, uint32_t bufsize, int32_t *ptrBytesRead)
{
	unsigned long bytesAvailable = 0;

	if (prvInitServerSocketIfNeeded() < 0)
		return -1;
	
	if (prvInitTraceSocketIfNeeded() < 0)
		return -1;

	if (ioctlsocket(trace_socket[0], FIONREAD, &bytesAvailable) != NO_ERROR)
	{
		closesocket(trace_socket[0]);
		trace_socket[0] = (UINT_PTR)0;
		return -1;
	}

	if (bytesAvailable > 0)
	{
		*ptrBytesRead = recv(trace_socket[0], data, bufsize, 0);
		if (*ptrBytesRead == SOCKET_ERROR)
		{
			closesocket(trace_socket[0]);
			trace_socket[0] = (UINT_PTR)0;
			return -1;
		}
	}

	return 0;
}

static void prvCloseAllSockets(void)
{
	int i;

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		if (trace_socket[i] != 0)
		{
			closesocket(trace_socket[i]);
			trace_socket[i] = 0;
		}
	
		if (server_socket[i] != 0)
		{
			closesocket(server_socket[i]);
			server_socket[i] = 0;
		}
	}
}

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortTCPIP = (TraceStreamPortTCPIP_t*)pxBuffer;

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	prvCloseAllSockets();
	
	return TRC_SUCCESS;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
