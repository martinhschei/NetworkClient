#include "Arduino.h";
#include "NetworkClient.h";
#include "ESP8266WiFi.h";
#include <ArduinoJson.h>

NetworkClient::NetworkClient(int ok, int working, int error)
{ 
	pinMode(ok, OUTPUT);
	pinMode(working, OUTPUT);
	pinMode(error, OUTPUT);
	_pinOk = ok;
	_pinIsWorking = working;
	_pinError = error;
}

String NetworkClient::NewReading(String value, String url)
{
	SetLedStatus("working");

	StaticJsonBuffer<300> JSONbuffer;
	JsonObject& JSONencoder = JSONbuffer.createObject();
	JSONencoder["value"] = value;
	JSONencoder["token"] = _token;

	char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

	_client.begin(url);
	_client.addHeader("Content-Type", "application/json");
	_lastResultCode = _client.POST(JSONmessageBuffer);
	_lastResultString = _client.getString();
	_client.end();
	SetLedStatus("ready");
	return _lastResultString;
}

void NetworkClient::SetLedStatus(String status)
{
	if (status == "ready") {
		digitalWrite(_pinOk, HIGH);
		digitalWrite(_pinIsWorking, LOW);
		digitalWrite(_pinError, LOW);
	}
	if (status == "working") {
		digitalWrite(_pinIsWorking, HIGH);
		digitalWrite(_pinOk, LOW);
		digitalWrite(_pinError, LOW);
	}
	if (status == "error") {
		digitalWrite(_pinError, HIGH);
		digitalWrite(_pinIsWorking, LOW);
		digitalWrite(_pinOk, LOW);
	}
}

boolean NetworkClient::IsConnected()
{
	return WiFi.status() == WL_CONNECTED;
}

boolean NetworkClient::IsRegistered()
{
	return _isRegistered;
}

boolean NetworkClient::ConnectToWifi(char* ssid, char* passphrase) 
{
	Print("Connecting...");
	SetLedStatus("working");
	
	WiFi.begin(ssid,passphrase);

	while (WiFi.status() != WL_CONNECTED && _connectionAttempts < 1000) {
	    delay(1000);
	    Print("Trying Wifi...");
	    _connectionAttempts++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		Print("Connected to wifi");
		SetLedStatus("ready");
		return true;
	}

	return false;
}

void NetworkClient::Get(String url)
{
	_client.begin(url);
	_lastResultCode = _client.GET();
	_lastResultString = _client.getString();
	_client.end();
}

boolean NetworkClient::ApiRegistration(String url, String arguments)
{
	SetLedStatus("working");
	
	Print("Trying to register with api...");

	StaticJsonBuffer<300> JSONbuffer;
	JsonObject& JSONencoder = JSONbuffer.createObject();
	JSONencoder["name"] = "sensor unit";
	JSONencoder["description"] = "potentially several different sensors";
	JSONencoder["type"] = "sensor";
	JSONencoder["url"] = "";
	JSONencoder["token"] = "";

	char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    // Serial.println(JSONmessageBuffer);

	_client.begin(url);
	_client.addHeader("Content-Type", "application/json");
	_lastResultCode = _client.POST(JSONmessageBuffer);
	_lastResultString = _client.getString();
	_client.end();

	if (GetLastResultCode() == 201) {
		StaticJsonBuffer<300> jsonBuffer;

		JsonObject& root = jsonBuffer.parseObject(_lastResultString);
		_token = root["status_message"]["token"].as<String>();

		Print(_lastResultString);

		Print("201 - Registration OK!");
		SetLedStatus("ready");
		Flush();
		_isRegistered = true;
	}
	else {
		Print("Error - Not registered");
		SetLedStatus("error");
		Flush();
		_isRegistered = false;
	}
}

String NetworkClient::GetLastResultString() 
{
	return _lastResultString;
}

void NetworkClient::Flush()
{
	_lastResultString = "";
	_lastResultCode = -1;
}

int NetworkClient::GetLastResultCode()
{
	return _lastResultCode;
}

void NetworkClient::Print(String text)
{
	Serial.println(text);
}