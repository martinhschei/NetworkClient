#ifndef NetworkClient_h;
#define NetworkClient_h;

#include "Arduino.h";
#include "ESP8266HTTPClient.h";

class NetworkClient
{
	public:
		NetworkClient(int ok, int working, int error);
		boolean IsConnected();
		boolean IsRegistered();
		boolean ApiRegistration(String url, String arguments);
		boolean ConnectToWifi(char* ssid, char* passphrase);
		String GetLastResultString();
		void Flush();
		void SetLedStatus(String status);
		void Print(String text);
		void Get(String url);
		String NewReading(String reading, String url);
		int GetLastResultCode();

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