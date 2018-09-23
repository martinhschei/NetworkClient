#ifndef NetworkClient_h;
#define NetworkClient_h;

#include "Arduino.h";
#include "ESP8266HTTPClient.h";

class NetworkClient
{
	public:
		boolean IsConnected();
		boolean IsRegistered();
		NetworkClient(int ok, int working, int error);
		void setLedStatus(String status);
		boolean ConnectToWifi(String ssid, String passphrase);
		void get(String url);
		void post(String url, String json);
		String getLastResultString();
		int getLastResultCode();
		void flush();
		boolean ApiRegistration(String url, String arguments);
		void Ping(String url);
		void Print(String text);

	private:
		int _pinOk;
		int _pinIsWorking;
		int _pinError;
		int _connectionAttempts;
		int _lastResultCode;
		boolean _isRegistered;
		String _lastResultString;
		String _ssid[];
		String _passphrase[];
		String _token;
		HTTPClient _client;
};

#endif