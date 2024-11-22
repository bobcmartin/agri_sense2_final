/* 
  SPDX-License-Identifier: MIT
  Copyright (C) 2024 Avnet
  Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
#include "log.h"
#include "lte.h"
#include "mqtt_client.h"
#include "led_ctrl.h"
#include "iotconnect.h"
#include "iotc_ecc608.h"
#include "iotc_provisioning.h"
#include "agri-sense2.h"

#define APP_VERSION "03.00.00"

#define GENERATED_ID_PREFIX "sn"

static char duid_from_serial_buf[sizeof(GENERATED_ID_PREFIX) + ATCA_SERIAL_NUM_SIZE * 2];

void publish_telemetry(void);



extern struct _data_frame frame1,*frame_ptr;

static bool connected_to_network = false;


static void on_connection_status(IotConnectConnectionStatus status) {
    // Add your own status handling
    switch (status) {
        case IOTC_CS_MQTT_CONNECTED:
            Log.info(F("IoTConnect Client Connected"));
            break;
        case IOTC_CS_MQTT_DISCONNECTED:
            Log.info(F("IoTConnect Client Disconnected"));
            break;
        default:
            Log.error(F("IoTConnect Client ERROR"));
            break;
    }
}

static void command_status(const char* ack_id, bool command_success, const char *command_name, const char *message) {
    Log.infof(F("command: %s status=%s: %s\n"), command_name, command_success ? "OK" : "Failed", message);
    if (ack_id) {
      delay(1000);
      iotcl_mqtt_send_cmd_ack(
        ack_id,
        command_success ? IOTCL_C2D_EVT_CMD_SUCCESS_WITH_ACK : IOTCL_C2D_EVT_CMD_FAILED,
        message // allowed to be null, but should not be null if failed, we'd hope
      );
      delay(1000);
      Log.info("Message sent");
    }
}

static void on_command(IotclC2dEventData data) {
    const char *command = iotcl_c2d_get_command(data);
    const char *ack_id = iotcl_c2d_get_ack_id(data);
    if (NULL != command) {
        if(NULL != strstr(command, "led-user") ) {
            if (NULL != strstr(command, " on")) {
              LedCtrl.on(Led::USER);
            } else {
              LedCtrl.off(Led::USER);
            }
            command_status(ack_id, true, command, "OK");
        } else if(NULL != strstr(command, "led-error") ) {
            if (NULL != strstr(command, " on")) {
              LedCtrl.on(Led::ERROR);
            } else {
              LedCtrl.off(Led::ERROR);
            }
            command_status(ack_id, true, command, "OK");
        } else {
            Log.errorf(F("Unknown command:%s\r\n"), command);
            command_status(ack_id, false, command, "Not implemented");
        }
    } else {
        command_status(ack_id, false, "?", "Internal error");
    }
}

static void on_ota(IotclC2dEventData data) {
    const char *url = iotcl_c2d_get_ota_url(data, 0);
    if (url == NULL) {
      Log.error(F("OTA URL is missing?"));
      return;
    }
    Log.infof(F("OTA download request received for https://%s, but it is not implemented.\n"), url);
}


char data_str[40];

void iotconnect_publish(void)    
{
      if (!connected_to_network || !iotconnect_sdk_is_connected()) 
         {
          
            iotconnect_sdk_disconnect();
            Lte.end();
            Log.error(F("network disconnect"));
            while(1);
         }
      publish_telemetry();  
      iotconnect_sdk_loop();
}


void publish_telemetry(void) 
{
    IotclMessageHandle msg = iotcl_telemetry_create();

    // sprintf(data_str,"%ld",frame_ptr->frame_count);
    iotcl_telemetry_set_number(msg, "frame",frame_ptr->frame_count);
        
    // sprintf(data_str,"%u",frame_ptr->ss1_moisture);
    iotcl_telemetry_set_number(msg, "ss1_moisture",frame_ptr->ss1_moisture);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->ss1_temp);
    iotcl_telemetry_set_number(msg, "ss1_temp",(double)frame_ptr->ss1_temp);
    
    // sprintf(data_str,"%u",frame_ptr->ss2_moisture);
    iotcl_telemetry_set_number(msg, "ss2_moisture",frame_ptr->ss2_moisture);
    
    sprintf(data_str,"%4.2f",(double)frame_ptr->ss2_temp);
    iotcl_telemetry_set_number(msg, "ss2_temp",(double)frame_ptr->ss2_temp);
    
    // sprintf(data_str,"%u",frame_ptr->ss3_moisture);
    iotcl_telemetry_set_number(msg, "ss3_moisture",frame_ptr->ss3_moisture);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->ss3_temp);
    iotcl_telemetry_set_number(msg, "ss3_temp",(double)frame_ptr->ss3_temp);
    
    // sprintf(data_str,"%u",frame_ptr->ss4_moisture);
    iotcl_telemetry_set_number(msg, "ss4_moisture",frame_ptr->ss4_moisture);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->ss4_temp);
    iotcl_telemetry_set_number(msg, "ss4_temp",(double)frame_ptr->ss4_temp);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->pth_temp);
    iotcl_telemetry_set_number(msg, "pth_temp",(double)frame_ptr->pth_temp);
    
    // sprintf(data_str,"%6.1f",(double)frame_ptr->pth_pressure);
    iotcl_telemetry_set_number(msg, "pth_pressure",(double)frame_ptr->pth_pressure);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->pth_humidity);
    iotcl_telemetry_set_number(msg, "pth_humidity",(double)frame_ptr->pth_humidity);
    
    // sprintf(data_str,"%4.2f",(double)frame_ptr->bat_voltage);
    iotcl_telemetry_set_number(msg, "bat_voltage",(double)frame_ptr->bat_voltage);
    
    iotcl_telemetry_set_number(msg, "CO2_level",frame_ptr->CO2_level);


    iotcl_mqtt_send_telemetry(msg, false);
    iotcl_telemetry_destroy(msg);
}







static void on_lte_disconnect(void) {
  connected_to_network = false;
}

static bool connect_lte() {
    // Connect with a maximum timeout value of 30 000 ms, if the connection is
    // not up and running within 30 seconds, abort and retry later
    if (!Lte.begin()) {
        return false;
    } else {
        return true;
    }
}

static bool load_provisioned_data(IotConnectClientConfig *config) {
  if (ATCA_SUCCESS != iotc_ecc608_get_string_value(IOTC_ECC608_PROV_CPID, &(config->cpid))) {
    return false; // caller will print the error
  }
  if (ATCA_SUCCESS != iotc_ecc608_get_string_value(IOTC_ECC608_PROV_ENV, &(config->env))) {
    return false; // caller will print the error
  }
  if (ATCA_SUCCESS != iotc_ecc608_get_string_value(IOTC_ECC608_PROV_DUID, &(config->duid))) {
    return false; // caller will print the error
  }
  if (ATCA_SUCCESS != iotc_ecc608_get_platform(&(config->connection_type))) {
    return false; // caller will print the error
  }

  return true;
}

void iotconnect_init(void)
{
  Log.begin(115200);
  Log.setLogLevel(LogLevel::INFO);
  delay(2000);


  LedCtrl.begin();
  LedCtrl.startupCycle();

  if (ATCA_SUCCESS != iotc_ecc608_init_provision()) {
    Log.error(F("Failed to read provisioning data!"));
    delay(10000);
    return; // caller will print the error
  }

  IotConnectClientConfig config = {0};
  if (!load_provisioned_data(&config)
    || !config.cpid || 0 == strlen(config.cpid)
    || !config.env || 0 == strlen(config.env)
    || config.connection_type == IOTC_CT_UNDEFINED
  ) {
      Log.error("Invalid provisioning data. Please run the avr-iot-provision sketch.");
      return;
  }

  if (!config.duid || 0 == strlen(config.duid)) {
    strcpy(duid_from_serial_buf, GENERATED_ID_PREFIX);
    if (ATCA_SUCCESS != iotc_ecc608_get_serial_as_string(&duid_from_serial_buf[strlen(GENERATED_ID_PREFIX)])) {
      return; // caller will print the error
    }

    config.duid = duid_from_serial_buf;
  }

  Log.infof(F("Platform: %s\r\n"), config.connection_type == IOTC_CT_AWS ? "AWS" : "Azure");
  Log.infof(F("CPID: %s\r\n"), config.cpid);
  Log.infof(F("ENV : %s\r\n"), config.env);
  Log.infof(F("DUID: %s\r\n"), config.duid);

  if (!connect_lte()) {
      return;
  }
  connected_to_network = true;

  config.ota_cb = on_ota;
  config.status_cb = on_connection_status;
  config.cmd_cb = on_command;
  config.verbose = true;

  if (iotconnect_sdk_init(&config))
      Lte.onDisconnect(on_lte_disconnect); 
  
  else 
  {
    Log.error(F("Error SDK init"));
    iotconnect_sdk_disconnect();
    Lte.end();
    
    return;
  }

  Log.infof(F("Done.\n"));
}




