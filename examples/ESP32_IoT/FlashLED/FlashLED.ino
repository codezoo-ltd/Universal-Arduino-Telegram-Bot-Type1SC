/*******************************************************************
  A telegram bot for your ESP32 that controls the 
  onboard LED. The LED in this example is active low.

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

const int RED_LED = 23;
const int GREEN_LED = 0;
const int BLUE_LED = 2;
int ledStatus = 0;

void handleNewMessages(int numNewMessages)
{
	DebugSerial.print("handleNewMessages ");
	DebugSerial.println(numNewMessages);

	for (int i = 0; i < numNewMessages; i++)
	{
		String chat_id = bot.messages[i].chat_id;
		String text = bot.messages[i].text;

		String from_name = bot.messages[i].from_name;
		if (from_name == "")
			from_name = "Guest";

		if (text == "/red")
		{
			digitalWrite(RED_LED, LOW); // turn the RED LED off (LOW is the voltage level)
			digitalWrite(GREEN_LED, LOW); // turn the GREEN LED off (LOW is the voltage level)
			digitalWrite(BLUE_LED, LOW); // turn the BLUE LED off (LOW is the voltage level)

			digitalWrite(RED_LED, HIGH); // turn the LED on (HIGH is the voltage level)
			ledStatus = 1;
			bot.sendMessage(chat_id, "RED_Led is ON", "");
		}
		if (text == "/green")
		{
			digitalWrite(RED_LED, LOW); // turn the RED LED off (LOW is the voltage level)
			digitalWrite(GREEN_LED, LOW); // turn the GREEN LED off (LOW is the voltage level)
			digitalWrite(BLUE_LED, LOW); // turn the BLUE LED off (LOW is the voltage level)

			digitalWrite(GREEN_LED, HIGH); // turn the LED on (HIGH is the voltage level)
			ledStatus = 2;
			bot.sendMessage(chat_id, "GREEN_Led is ON", "");
		}
		if (text == "/blue")
		{
			digitalWrite(RED_LED, LOW); // turn the RED LED off (LOW is the voltage level)
			digitalWrite(GREEN_LED, LOW); // turn the GREEN LED off (LOW is the voltage level)
			digitalWrite(BLUE_LED, LOW); // turn the BLUE LED off (LOW is the voltage level)
			
			digitalWrite(BLUE_LED, HIGH); // turn the LED on (HIGH is the voltage level)
			ledStatus = 3;
			bot.sendMessage(chat_id, "BLUE_Led is ON", "");
		}

		if (text == "/off")
		{
			ledStatus = 0;
			digitalWrite(RED_LED, LOW); // turn the RED LED off (LOW is the voltage level)
			digitalWrite(GREEN_LED, LOW); // turn the GREEN LED off (LOW is the voltage level)
			digitalWrite(BLUE_LED, LOW); // turn the BLUE LED off (LOW is the voltage level)
			bot.sendMessage(chat_id, "Led is OFF", "");
		}

		if (text == "/status")
		{
			if (ledStatus == 1){
				bot.sendMessage(chat_id, "Red Led is ON", "");
			}else if (ledStatus == 2){
				bot.sendMessage(chat_id, "Green Led is ON", "");
			}else if (ledStatus == 3){
				bot.sendMessage(chat_id, "Blue Led is ON", "");
			}
			else{
				bot.sendMessage(chat_id, "Led is OFF", "");
			}
		}

		if (text == "/start")
		{
			String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
			welcome += "This is Flash Led Bot example.\n\n";
			welcome += "/red : to switch the Red Led ON\n";
			welcome += "/green : to switch the Green Led ON\n";
			welcome += "/blue : to switch the Blue Led ON\n";
			welcome += "/off : to switch the Led OFF\n";
			welcome += "/status : Returns current status of LED\n";
			bot.sendMessage(chat_id, welcome, "Markdown");
		}

		if (text == "/help")
		{
			String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
			welcome += "This is Flash Led Bot example.\n\n";
			welcome += "/red : to switch the Red Led ON\n";
			welcome += "/green : to switch the Green Led ON\n";
			welcome += "/blue : to switch the Blue Led ON\n";
			welcome += "/off : to switch the Led OFF\n";
			welcome += "/status : Returns current status of LED\n";
			bot.sendMessage(chat_id, welcome, "Markdown");
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

	pinMode(RED_LED, OUTPUT); // initialize digital ledPin as an output.
	pinMode(GREEN_LED, OUTPUT); // initialize digital ledPin as an output.
	pinMode(BLUE_LED, OUTPUT); // initialize digital ledPin as an output.
	delay(10);
	digitalWrite(RED_LED, LOW); // initialize pin as off (active HIGH)
	digitalWrite(GREEN_LED, LOW); // initialize pin as off (active HIGH)
	digitalWrite(BLUE_LED, LOW); // initialize pin as off (active HIGH)

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
