/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.6-dev */

#ifndef PB_FLOW_DEFINITION_PB_H_INCLUDED
#define PB_FLOW_DEFINITION_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _InterfaceMessage_MessageType { 
    InterfaceMessage_MessageType_kVersionInfo = 0, 
    InterfaceMessage_MessageType_kConstantFlow = 1, 
    InterfaceMessage_MessageType_kManualFlow = 2, 
    InterfaceMessage_MessageType_kDynamicFlow = 3, 
    InterfaceMessage_MessageType_kDynamicFlowInterval = 4, 
    InterfaceMessage_MessageType_kInformationRequest = 5, 
    InterfaceMessage_MessageType_kRunDynamicFlowRequest = 6, 
    InterfaceMessage_MessageType_kAck = 7, 
    InterfaceMessage_MessageType_kNack = 8 
} InterfaceMessage_MessageType;

typedef enum _SimulatorMessage_MessageType { 
    SimulatorMessage_MessageType_kVersionInfo = 0, 
    SimulatorMessage_MessageType_kFlow = 1, 
    SimulatorMessage_MessageType_kError = 2, 
    SimulatorMessage_MessageType_kHeartbeat = 3, 
    SimulatorMessage_MessageType_kFlowInterval = 4 
} SimulatorMessage_MessageType;

typedef enum _ManualFlow_FanDirection { 
    ManualFlow_FanDirection_kClockwise = 0, 
    ManualFlow_FanDirection_kCounterClockwise = 1 
} ManualFlow_FanDirection;

typedef enum _InformationRequest_DataType { 
    InformationRequest_DataType_kDynamicFlow = 0 
} InformationRequest_DataType;

/* Struct definitions */
typedef struct _ConstantFlow { 
    float flow; 
    uint32_t duration; 
    uint32_t trigger1_delay; 
    uint32_t trigger2_delay; 
} ConstantFlow;

typedef struct _DynamicFlow { 
    uint32_t duration; 
    uint32_t count; 
    uint32_t interval; 
    uint32_t trigger1_delay; 
    uint32_t trigger2_delay; 
} DynamicFlow;

typedef struct _DynamicFlowInterval { 
    uint32_t interval; 
    float flow; 
    uint32_t final; 
} DynamicFlowInterval;

typedef struct _FlowInfo { 
    float flow; 
    uint32_t timestamp; 
} FlowInfo;

typedef struct _InformationRequest { 
    InformationRequest_DataType data_type; 
} InformationRequest;

typedef struct _ManualFlow { 
    float flow; 
    uint32_t driver; 
    uint32_t motor_state; 
    ManualFlow_FanDirection fan_direction; 
} ManualFlow;

typedef struct _VersionInfo { 
    uint32_t major; 
    uint32_t minor; 
    uint32_t patch; 
} VersionInfo;

typedef struct _InterfaceMessage { 
    InterfaceMessage_MessageType message_type; 
    pb_size_t which_message;
    union {
        VersionInfo version_info;
        ConstantFlow constant_flow;
        DynamicFlow dynamic_flow;
        ManualFlow manual_flow;
        DynamicFlowInterval dynamic_flow_interval;
        InformationRequest information_request;
    } message; 
} InterfaceMessage;

typedef struct _SimulatorMessage { 
    SimulatorMessage_MessageType message_type; 
    pb_size_t which_message;
    union {
        VersionInfo version_info;
        FlowInfo flow_info;
        pb_callback_t error_message;
    } message; 
} SimulatorMessage;


/* Helper constants for enums */
#define _InterfaceMessage_MessageType_MIN InterfaceMessage_MessageType_kVersionInfo
#define _InterfaceMessage_MessageType_MAX InterfaceMessage_MessageType_kNack
#define _InterfaceMessage_MessageType_ARRAYSIZE ((InterfaceMessage_MessageType)(InterfaceMessage_MessageType_kNack+1))

#define _SimulatorMessage_MessageType_MIN SimulatorMessage_MessageType_kVersionInfo
#define _SimulatorMessage_MessageType_MAX SimulatorMessage_MessageType_kFlowInterval
#define _SimulatorMessage_MessageType_ARRAYSIZE ((SimulatorMessage_MessageType)(SimulatorMessage_MessageType_kFlowInterval+1))

#define _ManualFlow_FanDirection_MIN ManualFlow_FanDirection_kClockwise
#define _ManualFlow_FanDirection_MAX ManualFlow_FanDirection_kCounterClockwise
#define _ManualFlow_FanDirection_ARRAYSIZE ((ManualFlow_FanDirection)(ManualFlow_FanDirection_kCounterClockwise+1))

#define _InformationRequest_DataType_MIN InformationRequest_DataType_kDynamicFlow
#define _InformationRequest_DataType_MAX InformationRequest_DataType_kDynamicFlow
#define _InformationRequest_DataType_ARRAYSIZE ((InformationRequest_DataType)(InformationRequest_DataType_kDynamicFlow+1))


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define InterfaceMessage_init_default            {_InterfaceMessage_MessageType_MIN, 0, {VersionInfo_init_default}}
#define SimulatorMessage_init_default            {_SimulatorMessage_MessageType_MIN, 0, {VersionInfo_init_default}}
#define FlowInfo_init_default                    {0, 0}
#define ConstantFlow_init_default                {0, 0, 0, 0}
#define ManualFlow_init_default                  {0, 0, 0, _ManualFlow_FanDirection_MIN}
#define DynamicFlow_init_default                 {0, 0, 0, 0, 0}
#define DynamicFlowInterval_init_default         {0, 0, 0}
#define InformationRequest_init_default          {_InformationRequest_DataType_MIN}
#define VersionInfo_init_default                 {0, 0, 0}
#define InterfaceMessage_init_zero               {_InterfaceMessage_MessageType_MIN, 0, {VersionInfo_init_zero}}
#define SimulatorMessage_init_zero               {_SimulatorMessage_MessageType_MIN, 0, {VersionInfo_init_zero}}
#define FlowInfo_init_zero                       {0, 0}
#define ConstantFlow_init_zero                   {0, 0, 0, 0}
#define ManualFlow_init_zero                     {0, 0, 0, _ManualFlow_FanDirection_MIN}
#define DynamicFlow_init_zero                    {0, 0, 0, 0, 0}
#define DynamicFlowInterval_init_zero            {0, 0, 0}
#define InformationRequest_init_zero             {_InformationRequest_DataType_MIN}
#define VersionInfo_init_zero                    {0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define ConstantFlow_flow_tag                    1
#define ConstantFlow_duration_tag                2
#define ConstantFlow_trigger1_delay_tag          3
#define ConstantFlow_trigger2_delay_tag          4
#define DynamicFlow_duration_tag                 1
#define DynamicFlow_count_tag                    2
#define DynamicFlow_interval_tag                 3
#define DynamicFlow_trigger1_delay_tag           4
#define DynamicFlow_trigger2_delay_tag           5
#define DynamicFlowInterval_interval_tag         1
#define DynamicFlowInterval_flow_tag             2
#define DynamicFlowInterval_final_tag            3
#define FlowInfo_flow_tag                        1
#define FlowInfo_timestamp_tag                   2
#define InformationRequest_data_type_tag         1
#define ManualFlow_flow_tag                      1
#define ManualFlow_driver_tag                    2
#define ManualFlow_motor_state_tag               3
#define ManualFlow_fan_direction_tag             4
#define VersionInfo_major_tag                    1
#define VersionInfo_minor_tag                    2
#define VersionInfo_patch_tag                    3
#define InterfaceMessage_message_type_tag        1
#define InterfaceMessage_version_info_tag        2
#define InterfaceMessage_constant_flow_tag       3
#define InterfaceMessage_dynamic_flow_tag        4
#define InterfaceMessage_manual_flow_tag         5
#define InterfaceMessage_dynamic_flow_interval_tag 6
#define InterfaceMessage_information_request_tag 7
#define SimulatorMessage_message_type_tag        1
#define SimulatorMessage_version_info_tag        2
#define SimulatorMessage_flow_info_tag           3
#define SimulatorMessage_error_message_tag       4

/* Struct field encoding specification for nanopb */
#define InterfaceMessage_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    message_type,      1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,version_info,message.version_info),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,constant_flow,message.constant_flow),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,dynamic_flow,message.dynamic_flow),   4) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,manual_flow,message.manual_flow),   5) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,dynamic_flow_interval,message.dynamic_flow_interval),   6) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,information_request,message.information_request),   7)
#define InterfaceMessage_CALLBACK NULL
#define InterfaceMessage_DEFAULT NULL
#define InterfaceMessage_message_version_info_MSGTYPE VersionInfo
#define InterfaceMessage_message_constant_flow_MSGTYPE ConstantFlow
#define InterfaceMessage_message_dynamic_flow_MSGTYPE DynamicFlow
#define InterfaceMessage_message_manual_flow_MSGTYPE ManualFlow
#define InterfaceMessage_message_dynamic_flow_interval_MSGTYPE DynamicFlowInterval
#define InterfaceMessage_message_information_request_MSGTYPE InformationRequest

#define SimulatorMessage_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    message_type,      1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,version_info,message.version_info),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,flow_info,message.flow_info),   3) \
X(a, CALLBACK, ONEOF,    STRING,   (message,error_message,message.error_message),   4)
#define SimulatorMessage_CALLBACK pb_default_field_callback
#define SimulatorMessage_DEFAULT NULL
#define SimulatorMessage_message_version_info_MSGTYPE VersionInfo
#define SimulatorMessage_message_flow_info_MSGTYPE FlowInfo

#define FlowInfo_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    flow,              1) \
X(a, STATIC,   SINGULAR, UINT32,   timestamp,         2)
#define FlowInfo_CALLBACK NULL
#define FlowInfo_DEFAULT NULL

#define ConstantFlow_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    flow,              1) \
X(a, STATIC,   SINGULAR, UINT32,   duration,          2) \
X(a, STATIC,   SINGULAR, UINT32,   trigger1_delay,    3) \
X(a, STATIC,   SINGULAR, UINT32,   trigger2_delay,    4)
#define ConstantFlow_CALLBACK NULL
#define ConstantFlow_DEFAULT NULL

#define ManualFlow_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    flow,              1) \
X(a, STATIC,   SINGULAR, UINT32,   driver,            2) \
X(a, STATIC,   SINGULAR, UINT32,   motor_state,       3) \
X(a, STATIC,   SINGULAR, UENUM,    fan_direction,     4)
#define ManualFlow_CALLBACK NULL
#define ManualFlow_DEFAULT NULL

#define DynamicFlow_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   duration,          1) \
X(a, STATIC,   SINGULAR, UINT32,   count,             2) \
X(a, STATIC,   SINGULAR, UINT32,   interval,          3) \
X(a, STATIC,   SINGULAR, UINT32,   trigger1_delay,    4) \
X(a, STATIC,   SINGULAR, UINT32,   trigger2_delay,    5)
#define DynamicFlow_CALLBACK NULL
#define DynamicFlow_DEFAULT NULL

#define DynamicFlowInterval_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   interval,          1) \
X(a, STATIC,   SINGULAR, FLOAT,    flow,              2) \
X(a, STATIC,   SINGULAR, UINT32,   final,             3)
#define DynamicFlowInterval_CALLBACK NULL
#define DynamicFlowInterval_DEFAULT NULL

#define InformationRequest_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    data_type,         1)
#define InformationRequest_CALLBACK NULL
#define InformationRequest_DEFAULT NULL

#define VersionInfo_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   major,             1) \
X(a, STATIC,   SINGULAR, UINT32,   minor,             2) \
X(a, STATIC,   SINGULAR, UINT32,   patch,             3)
#define VersionInfo_CALLBACK NULL
#define VersionInfo_DEFAULT NULL

extern const pb_msgdesc_t InterfaceMessage_msg;
extern const pb_msgdesc_t SimulatorMessage_msg;
extern const pb_msgdesc_t FlowInfo_msg;
extern const pb_msgdesc_t ConstantFlow_msg;
extern const pb_msgdesc_t ManualFlow_msg;
extern const pb_msgdesc_t DynamicFlow_msg;
extern const pb_msgdesc_t DynamicFlowInterval_msg;
extern const pb_msgdesc_t InformationRequest_msg;
extern const pb_msgdesc_t VersionInfo_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define InterfaceMessage_fields &InterfaceMessage_msg
#define SimulatorMessage_fields &SimulatorMessage_msg
#define FlowInfo_fields &FlowInfo_msg
#define ConstantFlow_fields &ConstantFlow_msg
#define ManualFlow_fields &ManualFlow_msg
#define DynamicFlow_fields &DynamicFlow_msg
#define DynamicFlowInterval_fields &DynamicFlowInterval_msg
#define InformationRequest_fields &InformationRequest_msg
#define VersionInfo_fields &VersionInfo_msg

/* Maximum encoded size of messages (where known) */
/* SimulatorMessage_size depends on runtime parameters */
#define ConstantFlow_size                        23
#define DynamicFlowInterval_size                 17
#define DynamicFlow_size                         30
#define FlowInfo_size                            11
#define InformationRequest_size                  2
#define InterfaceMessage_size                    34
#define ManualFlow_size                          19
#define VersionInfo_size                         18

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
