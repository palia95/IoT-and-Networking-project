#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "Seeed_BME280.h"

/*
 * set LoraWan_RGB to Active,the RGB active in loraWan
 * RGB red means sending;
 * RGB purple means joined done;
 * RGB blue means RxWindow1;
 * RGB yellow means RxWindow2;
 * RGB green means received done;
 */

/* OTAA para*/
uint8_t devEui[] = { };
uint8_t appEui[] = { };
uint8_t appKey[] = { };

/* ABP para*/
uint8_t nwkSKey[] = { };
uint8_t appSKey[] = { };
uint32_t devAddr =  ( uint32_t )0x########;

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 300000;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 8;

float temperature, humidity, pressure;
uint16_t baseline;
int count;
int maxtry = 50;

BME280 bme280;

/*!
   \brief   Prepares the payload of the frame
*/
static void prepareTxFrame( uint8_t port )
{
	/*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
	*appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
	*if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
	*if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
	*for example, if use REGION_CN470, 
	*the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
	*/

  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(500);

  if(!bme280.init()){
    Serial.println("Device error!");
  }
  
  temperature = bme280.getTemperature();
  pressure = (bme280.getPressure())/100.0;
  humidity = bme280.getHumidity();

  Wire.end();
  digitalWrite(Vext, HIGH);
  uint16_t batteryVoltage = getBatteryVoltage();
  unsigned char *puc;

  puc = (unsigned char *)(&temperature);
  appDataSize = 14;
  appData[0] = puc[0];
  appData[1] = puc[1];
  appData[2] = puc[2];
  appData[3] = puc[3];

  puc = (unsigned char *)(&humidity);
  appData[4] = puc[0];
  appData[5] = puc[1];
  appData[6] = puc[2];
  appData[7] = puc[3];

  puc = (unsigned char *)(&pressure);
  appData[8] = puc[0];
  appData[9] = puc[1];
  appData[10] = puc[2];
  appData[11] = puc[3];

  appData[12] = (uint8_t)(batteryVoltage >> 8);
  appData[13] = (uint8_t)batteryVoltage;

  Serial.print("T=");
  Serial.print(temperature);
  Serial.print("C, RH=");
  Serial.print(humidity);
  Serial.print("%, Pressure=");
  Serial.print(pressure);
  Serial.print(" hPA, BatteryVoltage:");
  Serial.println(batteryVoltage);
}


void setup() {
	boardInitMcu();
  Serial.begin(115200);

#if(AT_SUPPORT)
	enableAt();
#endif
	deviceState = DEVICE_STATE_INIT;
	LoRaWAN.ifskipjoin();
}

void loop()
{
	switch( deviceState )
	{
		case DEVICE_STATE_INIT:
		{
#if(AT_SUPPORT)
			getDevParam();
#endif
			printDevParam();
			LoRaWAN.init(loraWanClass,loraWanRegion);
			deviceState = DEVICE_STATE_JOIN;
			break;
		}
		case DEVICE_STATE_JOIN:
		{
			LoRaWAN.join();
			break;
		}
		case DEVICE_STATE_SEND:
		{
			prepareTxFrame( appPort );
		  LoRaWAN.send();
			deviceState = DEVICE_STATE_CYCLE;
			break;
		}
		case DEVICE_STATE_CYCLE:
		{
			// Schedule next packet transmission
			txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
			LoRaWAN.cycle(txDutyCycleTime);
			deviceState = DEVICE_STATE_SLEEP;
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
			LoRaWAN.sleep();
			break;
		}
		default:
		{
			deviceState = DEVICE_STATE_INIT;
			break;
		}
	}
}
