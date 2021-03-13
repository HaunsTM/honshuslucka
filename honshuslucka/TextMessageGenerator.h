#include <string>

#ifndef TEXT_MESSAGE_GENERATOR_H
#define TEXT_MESSAGE_GENERATOR_H

class TextMessageGenerator
{
	private:
		bool _initialized;

		String _credentialsJSONString;
		String _mqttBrokerURL;
		int _mqttPort;
		String _mqttUsername;
		String _mqttPassword;
		String _mqttTopicSubscribe;
		String _mqttTopicPublish;

		const String HEADER_UNDERLINE = String("====================================\n");

	public:
		TextMessageGenerator(String mqttBrokerURL, int mqttPort, String mqttUsername, String mqttPassword, String mqttTopicSubscribe, String mqttTopicPublish);

		void initialize();

		String lookingForWifiConnection();
		String wifiConnectionEstablished(String SSID, String assignedIP);
		String wifiCredentialsJSONParsingError(String error);

		String connectingToMQTTServer(String clientId);
		String mQTTServerConnectionEstablished();
		String mQTTServerConnectionFailed(int state);
		String messageArrived(char* topic, byte* payload, unsigned int length);

		String hatchWereManuallyPositioned(String direction);
		String hatchArrivedToItsDestination(String position);


};

#endif