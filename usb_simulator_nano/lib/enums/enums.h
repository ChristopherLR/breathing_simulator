#pragma once

typedef enum {
  kWaitingForConnection,
  kSendingHeartbeat,
  kSendingDynamicFlow,
  kSendingConstFlow,
} SystemState;

typedef enum {
  kInvalid,
  kNone,
  kAttemptConnect,
  kStartConstantFlow,
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
