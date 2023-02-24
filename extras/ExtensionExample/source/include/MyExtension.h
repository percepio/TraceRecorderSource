#ifndef TRC_MY_EXTENSION_H
#define TRC_MY_EXTENSION_H

#include <trcRecorder.h>

/* This define enables or disables the extension. This should be in a config */
#define MY_EXTENSION_ENABLED 1

/* This check can enable/disable the entire extension */
#if (MY_EXTENSION_ENABLED == 1)

/* extern declaration for xMyExtension declared in MyExtension.c */
extern TraceExtensionHandle_t xMyExtension;

/* This information is used to set up the extension and is also used to indicate how to find the XML configuration file. Proper XML name for this example is "MyExtension-v1.2.3.xml" */
#define MY_EXTENSION_NAME "MyExtension"
#define MY_EXTENSION_VERSION_MAJOR 1
#define MY_EXTENSION_VERSION_MINOR 2
#define MY_EXTENSION_VERSION_PATCH 3
#define MY_EXTENSION_EVENT_COUNT 4

/* Macro that enables the extension and initializes xMyExtension */
#define xMyExtensionEnable() (xTraceExtensionCreate(MY_EXTENSION_NAME, MY_EXTENSION_VERSION_MAJOR, MY_EXTENSION_VERSION_MINOR, MY_EXTENSION_VERSION_PATCH, MY_EXTENSION_EVENT_COUNT, &xMyExtension))

/* Local event IDs up to (MY_EXTENSION_EVENT_COUNT - 1). You get the global event ID by calling xTraceExtensionGetEventId(...) and passing the extension handle and the local event ID as parameters */
#define MY_EVENT_A 0
#define MY_EVENT_B 1
#define MY_EVENT_C 2
#define MY_EVENT_D 3

/* Trace point examples */

/* First undefine any previous definitions */
#undef MY_TRACE_POINT_A
/* This stores a trace event without any parameters. Retrieve the Extension event ID by calling xTraceExtensionGetEventId() using xMyExtension and the event's local ID */
#define MY_TRACE_POINT_A() prvTraceStoreEvent_None(xTraceExtensionGetEventId(xMyExtension, MY_EVENT_A))

#undef MY_TRACE_POINT_B
/* This stores a trace event with a numeric parameter. Retrieve the Extension event ID by calling xTraceExtensionGetEventId() using xMyExtension and the event's local ID */
#define MY_TRACE_POINT_B() prvTraceStoreEvent_Param(xTraceExtensionGetEventId(xMyExtension, MY_EVENT_B), 1)

#undef MY_TRACE_POINT_C
/* This stores a trace event with a handle (here we use xMyExtension just as an example). Retrieve the Extension event ID by calling xTraceExtensionGetEventId() using xMyExtension and the event's local ID */
#define MY_TRACE_POINT_C() prvTraceStoreEvent_Handle(xTraceExtensionGetEventId(xMyExtension, MY_EVENT_C), xMyExtension)

#undef MY_TRACE_POINT_D
/* This stores a trace event with a handle and a numeric parameter (here we use xMyExtension just as an example). Retrieve the Extension event ID by calling xTraceExtensionGetEventId() using xMyExtension and the event's local ID */
#define MY_TRACE_POINT_D() prvTraceStoreEvent_HandleParam(xTraceExtensionGetEventId(xMyExtension, MY_EVENT_D), xMyExtension, 1)

#else

/* Extension disabled */
#define xMyExtensionEnable() 

#endif

#endif
