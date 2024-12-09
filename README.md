
# Agri-Sense2 README.md

## Summary

This document cover the software assets and steps required to build the Argi-sense2 example project as featured on 
the Hackster.io Impact Spotlight espisode that was presented on Nov 8th 2024. The complete project documentation including the hardware build
instructions, BOM, schematic and Gerber files for the PCB can be found at the following Hackster.io project link below

[Agri-sense 2](https://www.hackster.io/wizardofmake/agri-sense-2-fab593)


This project combined the AVR-IoT Mini Cellular Board, a collection of I2C based sensors that collected soil moisture, soil temperature, 
atmospheric pressure, relative humidy and air temperture and then transmitted this data upto the AVNET IoConnect Platform.  
The entire project is built and programmed using the Arduino 2.x IDE and the onboard programmer included on the AVR-IoT Mini Cell platform.

Clone this repository in a suitable working directory and then install all of the software requirements listed below. 


The software assets required (Arduino IDE + the support libraries)
+ Arduino 2.x IDE (the lastast version at time of publish is 2.3.3) (The older 1.8.x version wil also work but is not recommended)
+ [DxCore by Spence Konde v1.5.11](https://github.com/SpenceKonde/DxCore)
+ [AVR-IoT Cellular Library](https://github.com/microchip-pic-avr-solutions/avr-iot-cellular-arduino-library?tab=readme-ov-file) 
+ [Avnet IoTConnect library](https://github.com/avnet-iotconnect/iotc-arduino-mchp-avr-sdk)
+ Adafruit Sensor Library
+ Adafruit MS8607 Sensor Library
+ Adafruit Seesaw Library

Install all of the software as indicated above according their respective README.md or installation instructions. The last three Adafruit libraries
can be installed via the Library Manger tab within the Adruino IDE.

While adding the custom download URL to the File --> Prefences settings in the Arduino IDE it is also highly recommended to check the following boxes

### Show verbose output during [x]compile [x]upload

Having these boxes checked helps to debug problem in either the build or upload process

Two online accounts wil be required to complete the project, one for the SIM card activation and another one for the AVNET IoT Connect account. 
A free subscription of IoTConnect is available to allow evelaution of this project at no charge. 


## Procedure

### Provisioning
Before the final application can be uploaded to AVR IoT Mini Cellular board the board must be configured and provisioned according to the Avnet IoTConnect cloud platform 
according to the procedure found here.  Follow the process all of the way through including creating the sample application device and template.
This Quick Start Guide also covers setting up and activating the TruPhone SIM card. SIM card activation and basic 
board test is also covered at the AVR-IoT Cellular Library website
[AVNET IOtConnect Provisioning - Quick Start Guide](https://github.com/avnet-iotconnect/iotc-arduino-mchp-avr-sdk/blob/main/QUICKSTART.md)
### ! Important !
#### Follow all of the procedures specific to the AWS version of the IoConnect configuration.  
At the conclusion of the procedure you should have a CPID (Company ID), a X509 Certificate that has been attached to the example device created. 
If the sample application is able to publish data to your IoTConnect dashboard using the supplied template it's now possible to proceed to the 
Agri-Sense2 Device Creation

### Creating the Agri-sense 2 Device and Dashboard 
Create a new Agri-Sense 2 device either by editing the sample application device already created or delete the current device and start from scratch. 
However now use the /template/agrisense2_template.JSON file included in this repository.
Similarily upload the /templates/Microchip AgriSense Smart Agriculture Demo_dashboard_export.json file when creating a new dashboard for the Agri-Sense2 device

### Building and Programming the Agri-Sense2 application
+ Load the Agri_sense2_final project into Arduino IDE and connect the AVR-IoT board via a USB Cable.  
+ Using the Select Other Board and Port dialog box just to the right of the Debug icon selection AVR-DB series (no bootloader) 
+ select the proper COM port for the board as well
+ Using the Tools pulldown from the main menu make sure the following specifc settings are correct, all other settings can remain as default
    + Chip:"AVR128DB48"
    + MultiVoltage I/O: "Enabled"
    + printf():"Full, 2.6k, prints floats"
    + Wire (Wire,h/I2C) Library mode:"2x Wire, Master or Slave (32+ pin devices only)"
    + Programmer: "Curiosity Nano (nEDBG, debug chip:ATSAMD21E18)"

These settings will be the correct defaults for all project being built for the AVR IoT Mini Cell board.

Verify the application using the Verify button

The end of the build process should be close to or exactly the size as shown below

    Sketch uses 77464 bytes (59%) of program storage space. Maximum is 131072 bytes.
    Global variables use 8061 bytes (49%) of dynamic memory, leaving 8323 bytes for local variables. Maximum is 16384 bytes.

On succesful build the application can now be uploaded via the
Sketch --> Upload Using Programmer menu selection.
The normal Upload button assumes a Bootloader is present and will fail.

## Serial Console Output
Using Tera Term or equivalent set to 115200 baud connect to the board and perform a hard reset by pressing the small reset button located on the AVR IoT Mini Cell Button located nearest the large square silver chip which is the Sequans modem. 
Assuming that the Agri-sense2 hardware platform has been properly wired the following serial stream will appear. 


====== Agrisense 2 Core ======

soil sensor 1 detected

soil sensor 2 detected

soil sensor 3 detected

soil sensor 4 detected

PTH sensor detected

T6793 CO2 sensor detected

[INFO] Platform: AWS
[INFO] CPID: B5B4BD238C50401EA8B9AFA62AA57E71
[INFO] ENV : poc
[INFO] DUID: sn0123790FE70E053501
[INFO] Connecting to operator.... OK!
[INFO] +CCLK: "24/11/24,21:55:24-32"
[INFO] Using AWS discovery URL https://awsdiscovery.iotconnect.io/api/v2.1/dsdk/cpId/B5B4BD238C50401EA8B9AFA62AA57E71/env/poc

[INFO] Using identity URL https://awspocdi.iotconnect.io/api/2.1/agent/device-identity/cg/e3b4ff16-ad32-465b-a9bf-23ae840a3e62/uid/sn0123790FE70E053501

[INFO] Identity response parsing successful.

[INFO] Attempting to connect to MQTT host:a110maijzy82z6-ats.iot.us-east-1.amazonaws.com, client id:B5B4BD238C50401EA8B9AFA62AA57E71-sn0123790FE70E053501, 
username:[empty]

[INFO] Connecting to MQTT broker... OK!
[INFO] Done.

====== frame : 1 =========
air temp : 22.03  humidity : 46.5 atm pressure : 1015.3
ss1 temp : 25.86 ss1 moisture lvl 6
ss2 temp : 24.54 ss2 moisture lvl 6
ss3 temp : 23.08 ss3 moisture lvl 6
ss4 temp : 26.14 ss4 moisture lvl 6
CO2 Level 1075  bat voltage 6.9

[INFO] >: {"d":[{"d":{"frame":1,"ss1_moisture":6,"ss1_temp":25.856247,"ss2_moisture":6,"ss2_temp":24.535461,"ss3_moisture":6,"ss3_temp":23.079254,"ss4_moisture":6,"ss4_temp":26.140579,"pth_temp":22.030001,"pth_pressure":1015.3,"pth_humidity":46.501678,"bat_voltage":12.892179,"CO2_level":1075}}]}

====== frame : 2 =========
air temp : 22.03  humidity : 46.5 atm pressure : 1015.3
ss1 temp : 25.57 ss1 moisture lvl 6
ss2 temp : 24.64 ss2 moisture lvl 6
ss3 temp : 22.78 ss3 moisture lvl 6
ss4 temp : 25.94 ss4 moisture lvl 6
CO2 Level 1076  bat voltage 6.9

[INFO] >: {"d":[{"d":{"frame":2,"ss1_moisture":6,"ss1_temp":25.566086,"ss2_moisture":6,"ss2_temp":24.640045,"ss3_moisture":6,"ss3_temp":22.777756,"ss4_moisture":6,"ss4_temp":25.936096,"pth_temp":22.030001,"pth_pressure":1015.3,"pth_humidity":46.501678,"bat_voltage":6.9389491,"CO2_level":1076}}]}


The same data stream should also be appearing on the IoT Conenct dashboard at this time.

## Summary
For further code commentary including debugging hints please refer to the 

[Agri-sense 2](https://www.hackster.io/wizardofmake/agri-sense-2-fab593) project on Hackster IO.

There are some simple modifications to the main application that can be used to publish synthetic data to the IoT Connect dashboard in the case of not having the entire Agri-sense2 hardware system available.






