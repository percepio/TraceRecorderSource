/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
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

#include "trcRecorder.h"

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)  
#if (TRC_USE_TRACEALYZER_RECORDER == 1)


#pragma comment(lib,"ws2_32.lib") //Winsock Library

SOCKET server_socket = (UINT_PTR)NULL, trace_socket = (UINT_PTR)NULL;
struct sockaddr_in server, client;

int initServerSocketIfNeeded(void);
int initWinsockIfNeeded(void);

int initWinsockIfNeeded(void)
{
	WSADATA wsa;
	
	if (server_socket)
		return 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}

	return 0;
}


int initServerSocketIfNeeded(void)
{
	if (initWinsockIfNeeded() < 0)
	{
		return -1;
	}

	if (server_socket)
		return 0;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		closesocket(server_socket);
		WSACleanup();
		server_socket = (UINT_PTR)NULL;
		return -1;
	}

	if (listen(server_socket, 3) < 0)
	{
		closesocket(server_socket);
		WSACleanup();
		server_socket = (UINT_PTR)NULL;
		return -1;
	}

	return 0;
}

int initTraceSocketIfNeeded(void)
{
	int c;

	if (!server_socket)
		return -1;

	if (trace_socket)
		return 0;

	c = sizeof(struct sockaddr_in);
	trace_socket = accept(server_socket, (struct sockaddr *)&client, &c);
	if (trace_socket == INVALID_SOCKET)
	{
		trace_socket = (UINT_PTR)NULL;
		
		closesocket(server_socket);
		WSACleanup();
		server_socket = (UINT_PTR)NULL;
		
		return -1;
	}

	return 0;
}

int32_t writeToSocket(void* data, uint32_t size, int32_t *ptrBytesWritten)
{
	int ret;

	if (!trace_socket)
	{
		if (ptrBytesWritten != NULL)
		{
			*ptrBytesWritten = 0;
		}
		return -1;
	}
	ret = send(trace_socket, data, size, 0);
	if (ret <= 0)
	{
		if (ptrBytesWritten != NULL)
		{
			*ptrBytesWritten = 0;
		}

		closesocket(trace_socket);
		trace_socket = (UINT_PTR)NULL;
		return ret;
	}

	if (ptrBytesWritten != NULL)
	{
		*ptrBytesWritten = ret;
	}
	
	return 0;
}

int32_t readFromSocket(void* data, uint32_t bufsize, int32_t *ptrBytesRead)
{
	unsigned long bytesAvailable = 0;

	if (initServerSocketIfNeeded() < 0)
		return -1;
	
	if (initTraceSocketIfNeeded() < 0)
		return -1;

	if (ioctlsocket(trace_socket, FIONREAD, &bytesAvailable) != NO_ERROR)
	{
		closesocket(trace_socket);
		trace_socket = (UINT_PTR)NULL;
		return -1;
	}

	if (bytesAvailable > 0)
	{
		*ptrBytesRead = recv(trace_socket, data, bufsize, 0);
		if (*ptrBytesRead == SOCKET_ERROR)
		{
			closesocket(trace_socket);
			trace_socket = (UINT_PTR)NULL;
			return -1;
		}
	}

	return 0;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
