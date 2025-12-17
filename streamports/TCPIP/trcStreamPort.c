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

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER == 0)
#error This StreamPort requires that TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER is enabled!
#endif

/* TCP/IP includes - for lwIP in this case */
#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/errno.h>

int listener_socket =  -1 ;
int data_sockets[TRC_CFG_CORE_COUNT];
struct sockaddr_in address, remote;
int remoteSize = sizeof(remote);

static TraceStreamPortBuffer_t* pxStreamPortTCPIP TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static int32_t prvSocketSend(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten);
static int32_t prvSocketReceive(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesRead);
static int32_t prvSocketInitializeListener(void);
static int32_t prvSocketAccept(void);
static void prvCloseAllSockets(void);

static int32_t prvSocketSend( void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten )
{
  if (data_sockets[uiChannel] < 0)
    return -1;
  
  if (piBytesWritten == (void*)0)
	return -1;
  
  *piBytesWritten = send( data_sockets[uiChannel], pvData, uiSize, 0 );
  
  if (*piBytesWritten < 0)
  {
    *piBytesWritten = 0;
		
    /* EWOULDBLOCK may be expected when buffers are full */
    if (errno != EWOULDBLOCK)
    {
      close(data_sockets[uiChannel]);
      data_sockets[uiChannel] = -1;
      return -1;
    }
  }
  
  return 0;
}

static int32_t prvSocketReceive( void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesRead )
{
  if (data_sockets[uiChannel] < 0)
    return -1;
  
  if (piBytesRead == (void*)0)
	  return -1;

  *piBytesRead = recv( data_sockets[uiChannel], pvData, uiSize, 0 );
  
  if (*piBytesRead < 0)
  {
	  *piBytesRead = 0;
	  
		/* EWOULDBLOCK may be expected when there is no data to receive */
	  if (errno != EWOULDBLOCK)
	  {
		  close(data_sockets[uiChannel]);
		  data_sockets[uiChannel] = -1;
		  return -1;
	  }
  }

  return 0;
}

static int32_t prvSocketInitializeListener(void)
{
  if (listener_socket >= 0)
  {
	  return 0;
  }
  
  listener_socket = socket(AF_INET, SOCK_STREAM, 0);
  
  if (listener_socket < 0)
  {
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(TRC_CFG_STREAM_PORT_TCPIP_PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(listener_socket, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    close(listener_socket);
    listener_socket = -1;
    return -1;
  }

  if (listen(listener_socket, (TRC_CFG_CORE_COUNT) + 1) < 0)
  {
    close(listener_socket);
    listener_socket = -1;
    return -1;
  }

  return 0;
}

static int32_t prvSocketAccept(void)
{
  int i;
  int flags;

  if (listener_socket < 0)
      return -1;

  for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
  {
      if (data_sockets[i] >= 0)
      {
          continue;
      }

      data_sockets[i] = accept( listener_socket, (struct sockaddr *)&remote, (socklen_t*)&remoteSize );

      if( data_sockets[i] < 0 )
      {
          prvCloseAllSockets();
          return -1;
      }

      flags = fcntl( data_sockets[i], F_GETFL, 0 );
      fcntl( data_sockets[i], F_SETFL, flags | O_NONBLOCK );
  }

  return 0;
}

static void prvCloseAllSockets()
{
  int i;
  for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
  {
    if (data_sockets[i] > 0)
    {
      close(data_sockets[i]);
      data_sockets[i] = -1;
    }
  }
    
  if (listener_socket > 0)
  {
    close(listener_socket);
    listener_socket = -1;
  }
}
/************** MODIFY THE ABOVE PART TO USE YOUR TPC/IP STACK ****************/

int32_t prvTraceTcpWrite(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t *piBytesWritten)
{
  if (listener_socket < 0)
  {
    prvSocketInitializeListener();
    prvSocketAccept();
  }

  return prvSocketSend(pvData, uiSize, uiChannel, piBytesWritten);
}

int32_t prvTraceTcpRead(void* pvData, uint32_t uiSize, int32_t *piBytesRead)
{
  if (listener_socket < 0)
  {
    prvSocketInitializeListener();
    prvSocketAccept();
  }
      
    return prvSocketReceive(pvData, uiSize, 0, piBytesRead);
}

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
  int i;

	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortTCPIP = pxBuffer;

  for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
  {
    data_sockets[i] = -1;
  }

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	prvCloseAllSockets();

	return TRC_SUCCESS;
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
