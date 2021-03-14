#include <Arduino.h>
#include "HTTPWebServer.h"
#include "TextMessageGenerator.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

HTTPWebServer::HTTPWebServer(ESP8266WebServer& server, TextMessageGenerator& tMG)
    :   _server(server),
        _tMG(tMG)
{
    _initialized = false;
}

HTTPWebServer::~HTTPWebServer() {
}


bool HTTPWebServer::isConnectedToWifi() {
    bool isConnected = WiFi.status() == WL_CONNECTED;
    return isConnected;
}

void HTTPWebServer::initialize() {
    if (!_initialized) {
        // https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/

       
        setUpRouteHandlers();
        _server.begin();

        _initialized = true;
    }
}

void HTTPWebServer::setUpRouteHandlers() {

    _server.on("/", [this]() { routeGetInfo(); });

    _server.on("/actuatorOpenHatch", [this]() { routeActuatorOpenHatch(); });
    _server.on("/actuatorCloseHatch", [this]() { routeGetActuatorCloseHatch(); });

    _server.on("/actuatorPush", [this]() { routeActuatorPush(); });
    _server.on("/actuatorTurnOff", [this]() { routeActuatorTurnOff(); });
    _server.on("/actuatorPull", [this]() { routeActuatorPull(); });

    _server.onNotFound([this]() { routeGetNotFound(); });
}

String HTTPWebServer::htmlEnveloper(String title, String bodyContent) {
    const String html =
        String("<!DOCTYPE html>") +
        String("<html>") +
            String("<head>") +
                String("<title>") + title + String("</title>") +
                String("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">") +
                String("<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">") +
                String("<style>") +
                    String("body {font-family: Arial, sans-serif; color: Black; padding: 1rem;}") +
                    String("table {font-family: 'Courier New', monospace; padding: 1rem 0;}") +
                    String("table th {font-weight: bold; background-color: LightGrey}") +
                    String(".title {border-top: 1px solid Black;}") +
                    String(".response-result {text-align: right; background-color: LightGrey; font-family: 'Courier New', monospace; padding: 0 1rem;}") +
                    String(".info {text-align: right; border-top: 1px solid DarkGray; color: Gray; font-style: italic; font-size: small;}") +
                String("</style>") +
            String("</head>") +
            String("<body>") +
                String("<h1 class=\"title\">") + title + String("</h1>") +
                bodyContent +
                String("<div class=\"info\">") +
                    String("<p>This software is distributed under <a href = \"https://en.wikipedia.org/wiki/MIT_License\">MIT License</a>. Source code on <a href=\"https://github.com/HaunsTM\">Github - HaunsTM</a></p>") +
                String("</div>") +
            String("</body>") +
        String("</html>");
    return html;
};

String HTTPWebServer::javascriptCallAPIFromButton() {
    const String javascript =
        String("<script>") +
            String("const callAPIFromButton = async (url, buttonId, responseTextOutputId) => {") +
                String("const response = await fetch(`${url}`);") +
                String("const button = document.getElementById(buttonId);") +
                String("if (response.status == 200) {") +
                    String("button.style = 'background-color: LightGreen;';") +
                String("}") +
                String("else {") +
                    String("button.style = 'background-color: Tomato;';") +
                    String("console.error(`error calling api [${url}]: ${response.status}`);") +
                String("}") +
                String("if (responseTextOutputId && response) {") +
                    String("const responseTextOutput = document.getElementById(responseTextOutputId);") +
                    String("const responseText = await response.text();") +
                    String("responseTextOutput.innerHTML = `Latest request response: <i>${responseText}</i>`;") +
                String("}") +

            String("}") +
        String("</script>");
    return javascript;
}

void HTTPWebServer::routeGetInfo() {
    const String localIP = WiFi.localIP().toString();
    const String htmlBodyContent =
        String("<p>") +
	        String("This software is intended for Arduino and compatible PLCs. The purpose is to use the PLC to enable control of a linear actuator (12 volt DC motor) via wifi / via pushbuttons, where the ultimate goal is to hoist up / down a guillotine - type door to open / close the entrance to a chicken house.") +
        String("</p>") +
        String("<h2>Controls</h2>") +
        String("<h3>Processes</h3>") +
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th>Request</th>") +
			        String("<th colspan=2></th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
			        String("<td><a href=\"#\">//") + localIP + String("/actuatorCloseHatch</a></td>") +
                    String("<td>") + String("<button id='btnActuatorCloseHatch' type='button' onmousedown='callAPIFromButton(\"//") + localIP + String("/actuatorCloseHatch\", \"btnActuatorCloseHatch\", \"tdActuatorProcessResult\")'>Close hatch</button>") + String("</td>") +
			        String("<td>Close hatch</td>") +
		        String("</tr>") +        
		        String("<tr>") +
			        String("<td><a href=\"#\">//") + localIP + String("/actuatorOpenHatch</a></td>") +
                    String("<td>") + String("<button id='btnActuatorOpenHatch' type='button' onclick='callAPIFromButton(\"//") + localIP + String("/actuatorOpenHatch\", \"btnActuatorOpenHatch\", \"tdActuatorProcessResult\")'>Open hatch</button>") + String("</td>") +
			        String("<td>Open hatch</td>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<td id=\"tdActuatorProcessResult\" class=\"response-result\" colspan=3></td>") +
		        String("</tr>") +
	        String("</tbody>") +
        String("</table>") +
        String("<h3>Movements</h3>") +
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th>Request</th>") +
			        String("<th colspan=2></th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
			        String("<td><a href=\"#\">//") + localIP + String("/actuatorPush</a></td>") +
                    String("<td>") + String("<button id='btnActuatorPush' type='button' onmousedown='callAPIFromButton(\"//") + localIP + String("/actuatorPush\", \"btnActuatorPush\", \"tdActuatorMovementResult\")' onmouseup='callAPIFromButton(\"//") + localIP + String("/actuatorTurnOff\", \"btnActuatorPush\", \"tdActuatorMovementResult\")' ontouchstart='callAPIFromButton(\"//") + localIP + String("/actuatorPush\", \"btnActuatorPush\", \"tdActuatorMovementResult\")' ontouchend='callAPIFromButton(\"//") + localIP + String("/actuatorTurnOff\", \"btnActuatorPush\", \"tdActuatorMovementResult\")'>Push</button>") + String("</td>") +
			        String("<td>Extends the actuator piston...</td>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<td><a href=\"#\">//") + localIP + String("/actuatorTurnOff</a></td>") +
			        String("<td>") + String("<button id='btnActuatorTurnOff' type='button' onclick='callAPIFromButton(\"//") + localIP + String("/actuatorTurnOff\", \"btnActuatorTurnOff\", \"tdActuatorMovementResult\")'>Turn off</button>") + String("</td>") +
			        String("<td>Stop actuator engine</td>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<td><a href=\"#\">//") + localIP + String("/actuatorPull</a></td>") +
			        String("<td>") + String("<button id='btnActuatorPull' type='button' onmousedown='callAPIFromButton(\"//") + localIP + String("/actuatorPull\", \"btnActuatorPull\", \"tdActuatorMovementResult\")' onmouseup='callAPIFromButton(\"//") + localIP + String("/actuatorTurnOff\", \"btnActuatorPull\", \"tdActuatorMovementResult\")' ontouchstart='callAPIFromButton(\"//") + localIP + String("/actuatorPull\", \"btnActuatorPull\", \"tdActuatorMovementResult\")' ontouchend='callAPIFromButton(\"//") + localIP + String("/actuatorTurnOff\", \"btnActuatorPull\", \"tdActuatorMovementResult\")'>Pull</button>") + String("</td>") +
			        String("<td>Contracting the actuator piston...</td>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<td id=\"tdActuatorMovementResult\" class=\"response-result\" colspan=3></td>") +
		        String("</tr>") +
	        String("</tbody>") +
        String("</table>");

    const String htmlBodyAndJavascript = htmlBodyContent + javascriptCallAPIFromButton();
    String html = htmlEnveloper("Chicken house hatch", htmlBodyAndJavascript);
    _server.send(200, "text/html", html);
}

void HTTPWebServer::routeActuatorOpenHatch() {
    _server.send(200, "text/plain", "Opening hatch");
}
void HTTPWebServer::routeGetActuatorCloseHatch() {
    _server.send(200, "text/plain", "Closing hatch");
}

void HTTPWebServer::routeActuatorPush() {
    _server.send(200, "text/plain", "Extends actuator...");
}
void HTTPWebServer::routeActuatorTurnOff() {
    _server.send(404, "text/plain", "Turned off actuator");
}
void HTTPWebServer::routeActuatorPull() {
    _server.send(200, "text/plain", "Contracting actuator...");
}

void HTTPWebServer::routeGetNotFound() {
    _server.send(404, "text/plain", "Not found");
}

void HTTPWebServer::handleClient() {
    _server.handleClient();
}