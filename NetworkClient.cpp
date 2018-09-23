#include "Arduino.h";
#include "NetworkClient.h";
#include "ESP8266WiFi.h";

NetworkClient::NetworkClient(int ok, int working, int error)
{ 
	pinMode(ok, OUTPUT);
	pinMode(working, OUTPUT);
	pinMode(error, OUTPUT);
	_pinOk = ok;
	_pinIsWorking = working;
	_pinError = error;
}

void NetworkClient::setLedStatus(String status)
{
	if (status == "ok") {
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

boolean NetworkClient::ConnectToWifi(String ssid, String passphrase) 
{
	Print("Connecting...");
	setLedStatus("working");

	while (WiFi.status() != WL_CONNECTED && _connectionAttempts < 10) {
	    delay(1000);
	    Print("Trying Wifi...");
	    _connectionAttempts++;
	}

	if (WiFi.status() == WL_CONNECTED) {
		Print("Connected to wifi");
		setLedStatus("ok");
		return true;
	}

	return false;
}

void NetworkClient::get(String url)
{
	_client.begin(url);
	_lastResultCode = _client.GET();
	_lastResultString = _client.getString();
	_client.end();
}

void NetworkClient::post(String url, String arguments)
{
	_client.begin(url);
	_client.addHeader("Content-Type", "application/x-www-form-urlencoded");
	_lastResultCode = _client.POST(arguments);
	_lastResultString = _client.getString();
	_client.end();
}

void NetworkClient::Ping(String url)
{
	setLedStatus("working");

	String arguments = "token=";
	arguments.concat(_token);

	post(url, arguments);

	if (getLastResultCode() == 201) {
		Print("Ping recevied pong");
		setLedStatus("ok");
	} else {
		Print("Error - Ping did not receive pong:");
		setLedStatus("error");
	}

	flush();
}

boolean NetworkClient::ApiRegistration(String url, String arguments)
{
	setLedStatus("working");
	
	Print("Trying to register with api...");

	post(url, arguments);

	if (getLastResultCode() == 201) {
		_token = getLastResultString();
		Print("201 - Registration OK!");
		setLedStatus("ok");
		flush();
		_isRegistered = true;
	}
	else {
		Print("Error - Not registered");
		setLedStatus("error");
		flush();
		_isRegistered = false;
	}
}

String NetworkClient::getLastResultString() 
{
	return _lastResultString;
}

void NetworkClient::flush()
{
	_lastResultString = "";
	_lastResultCode = -1;
}

int NetworkClient::getLastResultCode()
{
	return _lastResultCode;
}

void NetworkClient::Print(String text)
{
	Serial.println(text);
}