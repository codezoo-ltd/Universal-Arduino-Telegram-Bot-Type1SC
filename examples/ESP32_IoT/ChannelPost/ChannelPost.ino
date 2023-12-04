/*******************************************************************
  An example of bot that echos back any messages received,
  including ones from channels

Parts:
ESP32 D1 Mini stlye Dev board* - http://s.click.aliexpress.com/e/C6ds4my
(or any ESP32 board)

= Affilate

If you find what I do useful and would like to support me,
please consider becoming a sponsor on Github
https://github.com/sponsors/witnessmenow/


Written by Brian Lough
YouTube: https://www.youtube.com/brianlough
Tindie: https://www.tindie.com/stores/brianlough/
Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
/*******************************************************************
 *  An example of bot that echos back any messages received,
 *  including ones from channels
 *                                                                           
 *  written by Brian Lough
 *******************************************************************/
#include <PPPOSClientSecure.h>
#include <PPPOSSecure.h>
#include <UniversalTelegramBot.h>
#include "TYPE1SC.h"

#define SERIAL_BR 115200
#define GSM_SERIAL 1
#define GSM_RX 16
#define GSM_TX 17
#define GSM_BR 115200

#define PWR_PIN 5
#define RST_PIN 18
#define WAKEUP_PIN 19

#define DebugSerial Serial
#define M1Serial Serial2 // ESP32

char *ppp_user = "codezoo";
char *ppp_pass = "codezoo";
String APN = "simplio.apn";

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

PPPOSClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

TYPE1SC TYPE1SC(M1Serial, DebugSerial, PWR_PIN, RST_PIN, WAKEUP_PIN);

bool startPPPOS() {
	PPPOS_start();
	unsigned long _tg = millis();
	while (!PPPOS_isConnected()) {
		DebugSerial.println("ppp Ready...");
		if (millis() > (_tg + 30000)) {
			PPPOS_stop();
			return false;
		}
		delay(3000);
	}

	DebugSerial.println("PPPOS Started");
	return true;
}

void handleNewMessages(int numNewMessages)
{
	for (int i = 0; i < numNewMessages; i++)
	{
		if (bot.messages[i].type == "channel_post")
		{
			bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].chat_title + " " + bot.messages[i].text, "");
		}
		else
		{
			bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].text, "");
		}
	}
}

void setup()
{
	/* CATM1 Modem PowerUp sequence */
	pinMode(PWR_PIN, OUTPUT);
	pinMode(RST_PIN, OUTPUT);
	pinMode(WAKEUP_PIN, OUTPUT);

	digitalWrite(PWR_PIN, HIGH);
	digitalWrite(WAKEUP_PIN, HIGH);
	digitalWrite(RST_PIN, LOW);
	delay(100);
	digitalWrite(RST_PIN, HIGH);
	delay(2000);
	/********************************/
	// put your setup code here, to run once:
	M1Serial.begin(SERIAL_BR);
	DebugSerial.begin(SERIAL_BR);

	DebugSerial.println("TYPE1SC Module Start!!!"); 

	/* TYPE1SC Module Initialization */
	if (TYPE1SC.init()) {
		DebugSerial.println("TYPE1SC Module Error!!!");
	}

	/* Network Regsistraiton Check */
	while (TYPE1SC.canConnect() != 0) {
		DebugSerial.println("Network not Ready !!!");
		delay(2000);
	}

	if (TYPE1SC.setPPP() == 0) {
		DebugSerial.println("PPP mode change");
	}

	secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

	/* PPPOS Setup */
	PPPOS_init(GSM_TX, GSM_RX, GSM_BR, GSM_SERIAL, ppp_user, ppp_pass);
	DebugSerial.println("Starting PPPOS...");

	if (startPPPOS()) {
		DebugSerial.println("Starting PPPOS... OK");
	} else {
		DebugSerial.println("Starting PPPOS... Failed");
	}

	DebugSerial.print("Retrieving time: ");
	configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
	time_t now = time(nullptr);
	while (now < 24 * 3600)
	{
		DebugSerial.print(".");
		delay(100);
		now = time(nullptr);
	}
	DebugSerial.println(now);
}

void loop()
{
	if (millis() - bot_lasttime > BOT_MTBS)
	{
		int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

		while (numNewMessages)
		{
			DebugSerial.println("got response");
			handleNewMessages(numNewMessages);
			numNewMessages = bot.getUpdates(bot.last_message_received + 1);
		}

		bot_lasttime = millis();
	}
}
