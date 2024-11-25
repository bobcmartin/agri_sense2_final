
# Agri-sense2 README.md

## Summary

This document cover the software assets and steps required to build the Argi-sense2 example project as featured on 
the Hackster.io Impact Spotlight espisode that was presented on Nov 8th 2024. The complete project documentation including the hardware build
instructions, BOM, schematic and Gerber files for the PCB can be found at the following Hackster.io project link below

[Agri-sense 2](https://www.hackster.io/wizardofmake/agri-sense-2-fab593)


This project combined the AVR-IoT Mini Cellular Board, a collection of I2C based sensors that collected soil moisture, soil temperature, 
atmospheric pressure, relative humidy and air temperture and then transmitted this data upto the AVNET IoConnect Platform.  
The entire project is built and programmed using the Arduino 2.x IDE and the onboard programmer included on teh AVR-IoT Mini Cell platform.


The software assets required (Arduino IDE + the support libraries)
+ Arduino 2.x IDE (the lastast version at time of publish is 2.3.3)
+ [DxCore by Spence Konde v1.5.11](https://github.com/SpenceKonde/DxCore)
+ [AVR-IoT Cellular Library](https://github.com/microchip-pic-avr-solutions/avr-iot-cellular-arduino-library?tab=readme-ov-file) 
+ [Avnet IoTConnect library](https://github.com/avnet-iotconnect/iotc-arduino-mchp-avr-sdk)
+ Adafruit Sensor Library
+ Adafruit MS8607 Sensor Library
+ Adafruit Seesaw Library

Install all of the software as indicated above according their respective README.md or installation instructions. The last three Adafruit libraries
can be installed via the Library Manger tab within the Adruino IDE.

## Procedure

### Provisioning
Before the final application can be uploaded to AVR IoT Mini Cellular board the board must be configured and provisioned according to the Avnet IoTConnect cloud platform 
according to the procedure found here.  
This Quick Start Guide also covers setting up and activating the TruPhone SIM card. SIM card activation and basic 
board test is also covered at the AVR-IoT Cellular Library website
[AVNET IOtConnect Provisioning](https://github.com/avnet-iotconnect/iotc-arduino-mchp-avr-sdk/blob/main/QUICKSTART.md)
### ! Important !
#### Follow all of the procedures specific to the AWS version of the IoConnect configuration.  



