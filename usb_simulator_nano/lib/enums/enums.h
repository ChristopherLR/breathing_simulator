#pragma once

typedef enum {
  waiting_for_connection,
  sending_heartbeat,
  sending_flow,
} system_state;

typedef enum {
  invalid,
  none,
  attempt_connect,
  start_constant_flow,
  start_manual_flow,
  start_dynamic_flow,
  end_flow,
} system_event ;

typedef enum {
  ok,
  err,
} result;

typedef enum {
  sending_constant_flow,
  sending_dynamic_flow,
  sending_manual_flow,
  waiting_for_flow_end,
  idle,
} fan_state;