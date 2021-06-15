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

#include "trcRecorder.h"

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)  
#if (TRC_USE_TRACEALYZER_RECORDER == 1)
	
/* TCP/IP includes - for lwIP in this case */
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/errno.h"

#define TRC_TCPIP_PORT 12000

int sock = -1, new_sd = -1;
int flags = 0;
int remoteSize;
struct sockaddr_in address, remote;

int32_t trcSocketSend( void* data, int32_t size, int32_t* bytesWritten )
{
  if (new_sd < 0)
    return -1;
  
  if (bytesWritten == NULL)
	return -1;
  
  *bytesWritten = send( new_sd, data, size, 0 );
  if (*bytesWritten < 0)
  {
    /* EWOULDBLOCK may be expected when buffers are full */
    if ((errno != 0) && (errno != EWOULDBLOCK))
	{
		closesocket(new_sd);
		new_sd = -1;
		return -1;
	}
    else
        *bytesWritten = 0;
  }
  
  return 0;
}

int32_t trcSocketReceive( void* data, int32_t size, int32_t* bytesRead )
{
  if (new_sd < 0)
    return -1;

  *bytesRead = recv( new_sd, data, size, 0 );
  /* EWOULDBLOCK may be expected when there is no data to receive */
  if (errno != 0 && errno != EWOULDBLOCK)
  {
    closesocket(new_sd);
    new_sd = -1;
    return -1;
  }

  return 0;
}

int32_t trcSocketInitializeListener()
{
  if (sock >= 0)
	return 0;
  
  sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
  
  if (sock < 0)
    return -1;

  address.sin_family = AF_INET;
  address.sin_port = htons( TRC_TCPIP_PORT );
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    closesocket(sock);
    sock = -1;
    return -1;
  }

  if (lwip_listen(sock, 5) < 0)
  {
    closesocket(sock);
    sock = -1;
    return -1;
  }

  return 0;
}

int32_t trcSocketAccept()
{
  if (sock < 0)
      return -1;
  
  if (new_sd >= 0)
      return 0;
  
  remoteSize = sizeof( remote );
  new_sd = accept( sock, (struct sockaddr *)&remote, (socklen_t*)&remoteSize );

  if( new_sd < 0 )
  {
   	closesocket(new_sd);
    new_sd = -1;
   	closesocket(sock);
    sock = -1;
    return -1;
  }

  flags = fcntl( new_sd, F_GETFL, 0 );
  fcntl( new_sd, F_SETFL, flags | O_NONBLOCK );

  return 0;
}
/************** MODIFY THE ABOVE PART TO USE YOUR TPC/IP STACK ****************/

int32_t trcTcpWrite(void* data, uint32_t size, int32_t *ptrBytesWritten)
{
    return trcSocketSend(data, size, ptrBytesWritten);
}

int32_t trcTcpRead(void* data, uint32_t size, int32_t *ptrBytesRead)
{
    trcSocketInitializeListener();
        
    trcSocketAccept();
      
    return trcSocketReceive(data, size, ptrBytesRead);
}

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/
