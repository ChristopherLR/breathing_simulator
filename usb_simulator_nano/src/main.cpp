#include <Arduino.h>
#include <stdint.h>

#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <SFM3000.h>
#include <Fan.h>
#include <enums.h>

#define VERSION "0.2.0"
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define BUFFER_SIZE 128

#define HIGH 1
#define LOW 0

elapsedMillis runtime;

void process_incoming_byte (const byte);
system_event process_input(const char *);
result connect();
void send_heartbeat();
void print_request(result res);
void transition_state();

system_event event = system_event::none;
system_state state = system_state::waiting_for_connection;

DynamicJsonDocument doc(1024);
StaticJsonDocument<256> metadata;

void setup() {
  
  Serial.begin(BAUD);
  while (!Serial); // wait for serial port to connect. Needed for native USB

  initialise_fan();

  metadata["version"] = VERSION;
  metadata["baud"] = BAUD;

}


void loop() {
  if (Serial.available()) process_incoming_byte(Serial.read());

  switch(state){
    case system_state::waiting_for_connection:
      break;
    case system_state::sending_heartbeat: 
      send_heartbeat();
      break;
    case system_state::sending_flow: 
      if(fan_loop()) state = system_state::sending_heartbeat;
      break;
    default: 
      Serial.println("Unknown state");
  }
}

void process_incoming_byte (const uint8_t in){
  static char input_line [BUFFER_SIZE];
  static unsigned int input_pos = 0;

  switch (in){
    case '\n': {
      input_line [input_pos] = 0;  // terminating null byte
      input_pos = 0;  

      Serial.println(input_line);
      event = process_input(input_line);
      transition_state();
      
      // reset buffer for next time
      break;
    }
    case '\r': {
      break;
    }
    default: {
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (BUFFER_SIZE - 1)) input_line [input_pos++] = in;
      break;
    }
  }
}

void transition_state(){
  static result res = result::ok;

  switch (event) {
    case system_event::none:
      break;
    case system_event::invalid: {
      Serial.println("Invalid event");
      break;
    }
    case system_event::attempt_connect: {
      res = connect();
      if (res == result::ok) state = system_state::sending_heartbeat;
      if (res == result::err) Serial.println("Error connecting");
      break;
    }
    case system_event::start_constant_flow: {
      Serial.println("Setting up constant flow");
      float flow = doc["f"]   | 0.0; // Flow
      unsigned int delay  = doc["dl"]  | 0; // Delay
      unsigned int length = doc["d"] | 0; // Duration

      if (flow <= 0.0 || length <= 0) {
        res = result::err;
        Serial.print("Error setting up, flow: ");Serial.print(flow);Serial.print(", delay: ");Serial.println(length);
      }

      if (res == result::ok) { 
        set_const_flow(flow, length, delay);
        print_profile();
        state = system_state::sending_flow;
      }

      break;
    }
    case system_event::start_manual_flow: {
      Serial.println("Starting manual flow");
      unsigned char motor_state = doc["ms"] | 0; // Motor State
      unsigned char driver = doc["dv"] | 0; // Driver
      unsigned char motor = doc["m"] | 0; // Motor (Fan)

      set_manual_flow(motor_state, driver, motor);
      state = system_state::sending_flow;
      break;
    }
    case system_event::start_dynamic_flow: {
      Serial.println("Unimplemented");
      break;
    }
    case system_event::end_flow: {
      Serial.println("Stopping system");

      stop_motor();
      break;
    }
    default:
      Serial.println("Unknown request");
  }
}

system_event process_input(const char * data) {
  DeserializationError err = deserializeJson(doc, data);

  if (err != DeserializationError::Ok) Serial.println("Error deserialising input");

  const char * type = doc["t"] | "NA";
  Serial.println(type);

  if (strcmp(type, "NA") == 0) return system_event::invalid;
  if (strcmp(type, "connect") == 0) return system_event::attempt_connect;
  if (strcmp(type, "c") == 0) return system_event::start_constant_flow;
  if (strcmp(type, "m") == 0) return system_event::start_manual_flow;
  if (strcmp(type, "d") == 0) return system_event::start_dynamic_flow;
  
  return system_event::invalid;
}

void send_heartbeat() {
  if (runtime > 1000){
    Serial.println("alive");
    runtime = 0;
  }
}

result connect() {
  size_t len = serializeJson(metadata, Serial);
  Serial.print("\r\n");
  if (len > 0) return result::ok;
  return result::err;
}