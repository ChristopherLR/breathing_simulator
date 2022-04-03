#pragma once

typedef enum {
  kWaitingForConnection,
  kSendingHeartbeat,
  kSendingFlow,
} SystemState;

typedef enum {
  kInvalid,
  kNone,
  kAttemptConnect,
  kStartConstantFlow,
  kStartManualFlow,
  kStartDynamicFlow,
  kReceiveDynamicFlowInterval,
  kEndFlow,
  kConfirmDynamicFlow,
  kRunDynamicFlow,
} SystemEvent;

typedef enum {
  kOk,
  kErr,
} Result;

typedef enum {
  kStillProcessing,
  kAck,
  kNak,
} AckResponse;

typedef enum {
  kSendingConstantFlow,
  kSendingDynamicFlow,
  kSendingManualFlow,
  kWaitingForFlowEnd,
  kReceivingDynamicFlow,
  kConfirmingFlow,
  kIdle,
} FanState;

typedef enum {
  kClockwise,
  kCounterClockwise,
} FanDirection;
