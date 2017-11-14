/**
 * MySensors_Grove_FM_Radio
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0: GUILLOTON Laurent
 * Version 1.1 - 2017-11-14: Création du sketch initial
 *
 * DESCRIPTION
 *
 * MySensors_Grove_FM_Radio est un sketch d'une radio FM piloté par MySensors interfacé avec Jeedom.
 * Le chip du tuner radio est un RDA5807M
 *
 *
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include "Arduino.h"
#include <Wire.h>
#include <radio.h>
#include <RDA5807M.h>
#include <MySensors.h>
#include <SPI.h>


// ----- Fixed settings here. -----

#define FIX_BAND     RADIO_BAND_FM   ///< The band that will be tuned by this sketch is FM.
#define FIX_STATION  9130            ///< The station that will be tuned by this sketch is 89.30 MHz.
#define FIX_VOLUME   4               ///< The volume that will be set by this sketch is level 4.

RDA5807M radio;    // Create an instance of Class for RDA5807M Chip

/* Bloc de création des Id et message Mysensors */
// ID des capteurs sur le noeud
#define CHILD_ID_STATION	1					// Identificateur de la station écoutée
#define CHILD_ID_NEXT_STATION 2					// Identificateur du bouton prochaine station
#define CHILD_ID_PREVIOUS_STATION 3				// Identificateur du bouton précédente station
#define CHILD_ID_VOLUME 	4					// Identificateur du volume d'écoute
#define CHILD_ID_MUTE		5					// Identificateur du bouton mute
#define CHILD_ID_BASS		6					// Identificateur du bouton bass boost



// Message MySensors
MyMessage stationMsg(CHILD_ID_STATION, V_VAR1);
MyMessage nextStationMsg(CHILD_ID_NEXT_STATION, V_STATUS);
MyMessage previousStationMsg(CHILD_ID_PREVIOUS_STATION, V_STATUS);
MyMessage volumeMsg(CHILD_ID_VOLUME, V_PERCENTAGE);
MyMessage muteMsg(CHILD_ID_MUTE, V_STATUS);
MyMessage bassMsg(CHILD_ID_BASS, V_STATUS);



void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("MySensors_Grove_FM_Radio", "1.1");
	// Register all sensors to mySensors (they will be created as child devices)
	present(CHILD_ID_STATION, S_CUSTOM);
	present(CHILD_ID_NEXT_STATION, S_BINARY);
	present(CHILD_ID_PREVIOUS_STATION, S_BINARY);
	present(CHILD_ID_VOLUME, S_DIMMER);
	present(CHILD_ID_MUTE, S_BINARY);
	present(CHILD_ID_BASS, S_BINARY);
}

/// Setup a FM only radio configuration
/// with some debugging on the Serial port
void setup() {
  // open the Serial port
  Serial.begin(115200);
  Serial.println("Radio...");
  delay(200);

  // Initialize the Radio
  radio.init();

  // Enable information to the Serial port
  radio.debugEnable();

  // Set all radio setting to the fixed values.
  radio.setBandFrequency(FIX_BAND, loadState(CHILD_ID_STATION));
  send(stationMsg.set(loadState(CHILD_ID_STATION)));
  radio.setVolume(loadState(CHILD_ID_VOLUME));
  send(volumeMsg.set(loadState(CHILD_ID_VOLUME)));
  radio.setMono(false);
  radio.setMute(loadState(CHILD_ID_MUTE));
  send(muteMsg.set(loadState(CHILD_ID_MUTE)));
  radio.setBassBoost(loadState(CHILD_ID_BASS));
  send(bassMsg.set(loadState(CHILD_ID_BASS)));
} // setup


/// show the current chip data every 3 seconds.
void loop() {

} // loop

void receive(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type==V_STATUS)
	{
		bool state = message.getBool();
		if(message.sensor==CHILD_ID_NEXT_STATION)
		{
			send(nextStationMsg.set(true));
			radio.seekUp(true);
			send(nextStationMsg.set(false));
		}
		if(message.sensor==CHILD_ID_NEXT_STATION)
		{
			send(previousStationMsg.set(true));
			radio.seekDown(true);
			send(previousStationMsg.set(false));
		}
		if(message.sensor==CHILD_ID_MUTE)
		{
			radio.setMute(state);
			saveState(CHILD_ID_MUTE, state);
			send(muteMsg.set(state));
		}
		if(message.sensor==CHILD_ID_BASS)
		{
			radio.setBassBoost(state);
			saveState(CHILD_ID_BASS, state);
			send(bassMsg.set(state));
		}
	}
	else if (message.type == V_PERCENTAGE)
		{
			int val = atoi(message.data);
			radio.setVolume(val);
			saveState(CHILD_ID_VOLUME, val);
			send(volumeMsg.set(val));
		}
	else if (message.type == V_VAR1)
		{
			int val = atoi(message.data);
			radio.setFrequency(val);
			saveState(CHILD_ID_STATION, val);
			send(stationMsg.set(val));
		}
}

