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
    _server.on("/help", [this]() { routeGetInfo(); });
    _server.on("/info", [this]() { routeGetInfo(); });

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
                    String("table {font-family: 'Courier New', monospace;}") +
                    String("table th {font-weight: bold; background-color: LightGrey}") +
                    String(".title {border-top: 1px solid Black;}") +
                    String(".info {text-align: right; border-top: 1px solid DarkGray;color: Gray; font-style: italic; font-size: small;}") +
                String("</style>") +
            String("</head>") +
            String("<body>") +
                String("<h1 class=\"title\">") + title + String("</h1>") +
                bodyContent +
                String("<div class=\"info\">") +
                    String("<p>This software is distributed under <a href = \"https://en.wikipedia.org/wiki/MIT_License\">MIT License</a>.Source code could be found on <a href=\"https://github.com/HaunsTM\">Github - HaunsTM</a></p>") +
                String("</div>") +
            String("</body>") +
        String("</html>");
    return html;
};

String HTTPWebServer::javascriptCallAPIFromButton() {
    const String javascript =
        String("<script>") +
            String("const callAPIFromButton = async (url, buttonId) => {") +
                String("const response = await fetch(`${url}`);") +
                String("const button = document.getElementById(buttonId);") +
                String("if (response.status == 200) {") +
                    String("button.style = 'background-color: LightGreen;';") +
                String("}") +
                String("else {") +
                    String("button.style = 'background-color: Tomato;';") +
                    String("console.error(`error calling api [${url}]: ${response.status}`);") +
                String("}") +
            String("}") +
        String("</script>");
    return javascript;
}

void HTTPWebServer::routeGetInfo() {
    const String localIP = WiFi.localIP().toString();
    const String htmlBodyContent =
        String("<p>") +
	        String("Denna mjukvara &auml;r avsett f&ouml;r Arduino och kompatibla PLCer. Syftet &auml;r att medelst PLCn m&ouml;jligg&ouml;ra styrning av ett linj&auml;rt st&auml;lldon (12 volt DC motor) &ouml;ver wifi/via tryckknappar, ") +
	        String(" d&auml;r slutm&aring;let &auml;r hissa upp/ner en lucka av giljotintyp f&ouml;r att &ouml;ppna/st&auml;nga ing&aring;ngen till ett h&ouml;nshus.") +
        String("</p>") +
        String("<h2>Tillg&auml;nliga http-anrop:</h2>") +
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th></th>") +
			        String("<th>Anrop</th>") +
			        String("<th>F&ouml;rklaring</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +                    
			        String("<td>") + String("<button id='btnPush' type='button' onclick='callAPIFromButton(\"//") + localIP + String("\", \"btnPush\")'>Push</button>") + String("</td>") +
			        String("<td><a href=\"#\">//") + localIP + String("/push</a></td>") +
			        String("<td>Skjuter ut st&auml;lldonets pistong.</td>") +
		        String("</tr>") +
		        String("<tr>") +                            
			        String("<td>") + String("<button id='btnTurnOff' type='button' onclick='callAPIFromButton(\"//") + localIP + String("\", \"btnTurnOff\")'>Turn off</button>") + String("</td>") +
			        String("<td><a href=\"#\">//") + localIP + String("/turnOff</a></td>") +
			        String("<td>St&auml;nger av st&auml;lldonets motor.</td>") +
		        String("</tr>") +
		        String("<tr>") +                            
			        String("<td>") + String("<button id='btnPull' type='button' onclick='callAPIFromButton(\"//") + localIP + String("\", \"btnPull\")'>Pull</button>") + String("</td>") +
			        String("<td><a href=\"#\">//") + localIP + String("/pull</a></td>") +
			        String("<td>Drar in st&auml;lldonets pistong.</td>") +
		        String("</tr>") +
	        String("</tbody>") +
        String("</table>");

    const String htmlBodyAndJavascript = htmlBodyContent + javascriptCallAPIFromButton();
    String html = htmlEnveloper("H&ouml;nshuslucka", htmlBodyAndJavascript);
    _server.send(200, "text/html", html);
}

void HTTPWebServer::routeGetNotFound() {
    _server.send(404, "text/plain", "Not found");
}

void HTTPWebServer::handleClient() {
    _server.handleClient();
}