#include <Arduino.h>
#include "HatchRequestAction.h"
#include "HTTPWebServer.h"
#include "TextMessageGenerator.h"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

HTTPWebServer::HTTPWebServer(HatchRequest& hatchRequest, ESP8266WebServer& server, TextMessageGenerator& tMG, DistanceMeterData& currentMeterData)
    :   _hatchRequest(hatchRequest),
        _server(server),
        _tMG(tMG),
        _currentMeterData(currentMeterData)
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


        _hatchRequest.setAction(HatchRequestAction::STOP);
        _hatchRequest.setAcknowledged(true);

        setUpRouteHandlers();
        _server.begin();

        _initialized = true;
    }
}

void HTTPWebServer::setUpRouteHandlers() {

    _server.on("/", [this]() { routeGetControl(); });
    _server.on("/info", [this]() { routeGetInfo(); });

    _server.on("/openHatch", [this]() { routeGetOpenHatch(); });
    _server.on("/closeHatch", [this]() { routeGetCloseHatch(); });

    _server.on("/pushActuator", [this]() { routeGetPushActuator(); });
    _server.on("/stopActuator", [this]() { routeGetStopActuator(); });
    _server.on("/pullActuator", [this]() { routeGetPullActuator(); });

    _server.on("/lidarSensorData", [this]() { routeGetLidarSensorData(); });

    _server.onNotFound([this]() { routeGetNotFound(); });
}

String HTTPWebServer::htmlEnveloper(String title, String bodyContent) {
    const String localIP = WiFi.localIP().toString();
    const String html =
        String("<!DOCTYPE html>") +
        String("<html>") +
            String("<head>") +
                String("<title>") + title + String("</title>") +
                String("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">") +
                String("<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\">") +
                String("<style>") +
                    String("body {font-family: Arial, sans-serif; color: Black; padding: 1rem;}") +
                    String("table {width: 70vw; font-family: 'Courier New', monospace; padding: 1rem 0;}") +
                    String("table thead tr:first-child th {font-weight: bold; background-color: LightGrey;}") +
                    String("table thead tr:last-child th {border-bottom: 1px solid LightGrey;}") +
                    String(".table-sub-head { text-align:center; background-color: LightGrey;}") +
                    String("td a { font-size: 0.8rem;}") +
                    String(".button-symbol {width: 3rem; height: 3rem; font-size: 2rem; font-weight: bold;}") +
                    String(".button-success {background-color: LightGreen;}") +
                    String(".button-error {background-color: Tomato;}") +
                    String(".info { text-align: right; border-top: 1px solid DarkGray; color: Gray; font-style: italic; font-size: small;}") +
                    String(".title { border-top: 1px solid Black;}") +
                    String(".response-result { font-size: 0.8rem; font-family: 'Courier New', monospace; padding: 0 1rem;}") +
                    String(".navigation-links { display: flex; justify-content: flex-end;}") +
                String("</style>") +
            String("</head>") +
            String("<body>") +
                String("<h1 class=\"title\">") + title + String("</h1>") +
                bodyContent +
                String("<nav class=\"navigation-links\">") +
                    String("<a href=\"//") + localIP + String("/\">Control</a>   |   ") +
                    String("<a href=\"//") + localIP + String("/info\">Device info</a>") +
                String("</nav>") +
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
        
            String("const Timer = (ms) => {") +

	            String("let id;") +
	
	            String("const start = () => new Promise(resolve => {") +
		            String("if (id === -1) {") +
			            String("throw new Error('Timer already aborted');") +
		            String("}") +
		            String("id = setTimeout(resolve, ms);") +
	            String("});") +
	
	            String("const abort = () => {") +
		            String("if (id !== -1 || id === undefined) {") +
			            String("clearTimeout(id);") +
			            String("id = -1;") +
		            String("}") +
	            String("};") +
	
	            String("return {") +
		            String("start, abort") +
	            String("}") +
            String("};") +

            String("const callAPIFromButton = async (url, buttonId, responseTextOutputId, timeoutResponseStatusColorOnButton) => {") +

                String("const addPause = Timer(5*1000);") +
                String("const button = document.getElementById(buttonId);") +

                String("try {") +

                    String("const response = await fetch(`${url}`);") +
        
                    String("if (responseTextOutputId && response) {") +
                        String("const responseTextOutput = document.getElementById(responseTextOutputId);") +
                        String("const responseText = await response.text();") +
                        String("responseTextOutput.innerHTML = `<i>${responseText}</i>`;") +
                    String("}") +

                    String("if (response.status == 200) {") +
                        String("button.classList.remove(\"button-error\");") +
                        String("button.classList.add(\"button-success\");") +

                        String("if (timeoutResponseStatusColorOnButton) {") +
                            String("await addPause.start();") +
                            String("button.classList.remove(\"button-success\");") +
                        String("}") +

                    String("} else {") +
                        String("button.classList.remove(\"button-success\");") +
                        String("button.classList.add(\"button-error\");") +

                        String("console.error(`error calling api [${url}]: ${response.status}`);") +
                        String("if (timeoutResponseStatusColorOnButton) {") +
                            String("await addPause.start();") +
                            String("button.classList.remove(\"button-error\");") +
                        String("}") +
                    String("}") +

                String("} catch (e) {") +
                    String("button.classList.remove(\"button-success\");") +
                    String("console.error(`error calling api [${url}]: ${e}`);") +
                    String("button.classList.add(\"button-error\");") +
                String("}") +

            String("};") +

        String("</script>");
    return javascript;
}

String HTTPWebServer::javascriptAddActuatorControlButtonEventListeners() {

    const String localIP = WiFi.localIP().toString();
    const String javascript =
        String("<script>") +

            String("const grpPressButtons = [") +
	            String("{\"id\": \"btnPull\", \"urlStart\": \"//") + localIP + String("/pullActuator\", \"urlEnd\": \"//") + localIP + String("/stopActuator\"},") +
	            String("{\"id\": \"btnPush\", \"urlStart\": \"//") + localIP + String("/pushActuator\", \"urlEnd\": \"//") + localIP + String("/stopActuator\"},") +
	            String("{\"id\": \"btnTurnOff\", \"urlStart\": \"//") + localIP + String("/stopActuator\", \"urlEnd\": \"//") + localIP + String("/stopActuator\"}") +
            String("];") +

       
            String("const setUpEventHandler = (pressButton) => {") +
	            String("let button = document.getElementById(pressButton.id);") +
	            String("button.urlStart = pressButton.urlStart;") +
	            String("button.urlEnd = pressButton.urlEnd;") +

	            String("button.addEventListener(\"touchstart\", handleStart, {passive: false});") +
	            String("button.addEventListener(\"touchend\", handleEnd, {passive: false});") +
	            String("button.addEventListener(\"mousedown\", handleStart);") +
	            String("button.addEventListener(\"mouseup\", handleEnd);") +
            String("};") +

            String("const handleStart = (e) => {") +
                String("callAPIFromButton(e.currentTarget.urlStart, e.currentTarget.id, \"tdActuatorMovementResult\", false);") +
	            String("e.stopPropagation();") +
	            String("e.preventDefault();") +
            String("};") +

            String("const handleEnd = (e) => {") +
	            String("callAPIFromButton(e.currentTarget.urlEnd, e.currentTarget.id, \"tdActuatorMovementResult\", true);") +
	            String("e.stopPropagation();") +
	            String("e.preventDefault();") +
            String("};") +
 
            String("grpPressButtons.forEach( (pressButton, index, array) => {") +
	            String("setUpEventHandler(pressButton);") +
            String("});") +

        String("</script>");
    return javascript;
}

void HTTPWebServer::routeGetControl() {
    const String localIP = WiFi.localIP().toString();

    const String htmlBodyContent =
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>Process</th>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<th id=\"tdActuatorProcessResult\" class=\"response-result\" colspan=2>-</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("<button id='btnOpenHatch' type='button' onclick='callAPIFromButton(\"//") + localIP + String("/openHatch\", \"btnOpenHatch\", \"tdActuatorProcessResult\", true)' class=\"button-symbol\">&UpArrowBar;</button>") + String("</td>") +
                    String("<td>Open hatch</td>") +
		        String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("<button id='btnCloseHatch' type='button' onclick='callAPIFromButton(\"//") + localIP + String("/closeHatch\", \"btnCloseHatch\", \"tdActuatorProcessResult\", true)' class=\"button-symbol\">&DownArrowBar;</button>") + String("</td>") +
                    String("<td>Close hatch</td>") +                   
		        String("</tr>") +
	        String("</tbody>") +
        String("</table>") +
        String("<h3>Movements</h3>") +
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>Movement</th>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<th id=\"tdActuatorMovementResult\" class=\"response-result\" colspan=2>-</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td><button id=\"btnPull\" type=\"button\" class=\"button-symbol\">&#x21e7;</button></td>") +
                    String("<td>Contracting actuator piston</td>") +
		        String("</tr>") +
		        String("<tr>") +
                    String("<td><button id=\"btnTurnOff\" type=\"button\" class=\"button-symbol\">&#x220E;</button></td>") +
                    String("<td>Stop actuator engine</td>") +
		        String("</tr>") +
		        String("<tr>") +
                    String("<td><button id=\"btnPush\" type=\"button\" class=\"button-symbol\">&#x21e9;</button></td>") +
                    String("<td>Extends actuator piston</td>") +
		        String("</tr>") +
	        String("</tbody>") +
        String("</table>");

    const String htmlBodyAndJavascript = htmlBodyContent + javascriptCallAPIFromButton() + javascriptAddActuatorControlButtonEventListeners();
    String html = htmlEnveloper("Controls - Chicken house hatch", htmlBodyAndJavascript);
    _server.send(200, "text/html", html);
}

String HTTPWebServer::baseMQTTHen_HouseHatchTopic() {
    return "<code>iot/hen_house/hatch/</code>";
};

void HTTPWebServer::routeGetInfo() {
    const String localIP = WiFi.localIP().toString();
    const String htmlBodyContent =

        String("<h2>Software</h2>") +
        String("<p>") +
	        String("This software is intended for Arduino and compatible PLCs. The purpose is to use the PLC to enable control of a linear actuator (12 volt DC motor) via wifi / via pushbuttons, where the ultimate goal is to hoist up / down a guillotine - type door to open / close the entrance to a chicken house.") +
        String("</p>") +

        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>This device</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("Firmware version")  + String("</td>") +
                    String("<td>") + _tMG.firmwareVersion() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Serial monitor communication speed")  + String("</td>") +
                    String("<td>") + _tMG.serialMonitorBaud() + String("</td>") +
                String("</tr>") +
	        String("</tbody>") +
        String("</table>") +

        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>Wifi</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("MAC")  + String("</td>") +
                    String("<td>") + WiFi.macAddress() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Assigned IP")  + String("</td>") +
                    String("<td>") + WiFi.localIP().toString() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("SSID")  + String("</td>") +
                    String("<td>") + WiFi.SSID() + String("</td>") +
                String("</tr>") +
	        String("</tbody>") +
        String("</table>") +
        
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>Control requests</th>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<th>Action</th>") +
                    String("<th>Request (<code>GET</code>)</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("Close hatch")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/closeHatch\">//") + localIP + String("/closeHatch</a>") + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Open hatch")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/openHatch\">//") + localIP + String("/openHatch</a>") + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Contracting actuator piston")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/pullActuator\">//") + localIP + String("/pullActuator</a>") + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Stop actuator engine")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/stopActuator\">//") + localIP + String("/stopActuator</a>") + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Extends actuator piston")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/pushActuator\">//") + localIP + String("/pushActuator</a>") + String("</td>") +
                String("</tr>") +
	        String("</tbody>") +
        String("</table>") +
        
        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>Other requests</th>") +
		        String("</tr>") +
		        String("<tr>") +
			        String("<th>Action</th>") +
                    String("<th>Request (<code>GET</code>)</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("Get current read measurement from LIDAR detector in a JSON:<br /><br /><pre>{\n\t\"distanceToObjectCm\":\"xxx\",\n\t\"strengthOrQualityOfReturnSignal\":\"yyy\",\n\t\"temperatureInternalOfLidarSensorChipCelsius\":\"zzz\"\n}</pre>")  + String("</td>") +
                    String("<td>") + String("<a href=\"//") + localIP + String("/lidarSensorData\">//") + localIP + String("/lidarSensorData</a>") + String("</td>") +
                String("</tr>") +
	        String("</tbody>") +
        String("</table>") +

        String("<table>") +
	        String("<thead>") +
		        String("<tr>") +
			        String("<th colspan=2>MQTT</th>") +
		        String("</tr>") +
	        String("</thead>") +
	        String("<tbody>") +
		        String("<tr>") +
                    String("<td>") + String("Broker URL")  + String("</td>") +
                    String("<td>") + _tMG.mqttBrokerURL() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Port")  + String("</td>") +
                    String("<td>") + _tMG.mqttPort() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Username")  + String("</td>") +
                    String("<td>") + _tMG.mqttUsername() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + String("Password")  + String("</td>") +
                    String("<td>") + _tMG.mqttPassword() + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td class=\"table-sub-head\" colspan=2>") + String("Published topics")  + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td class=\"table-sub-head\" colspan=2>") + String("Actuator")  + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + baseMQTTHen_HouseHatchTopic() + String("<code>actuator/action</code>") + String("</td>") +
                    String("<td>") + String("Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code>")  + String("</td>") +
                String("</tr>") +




		        String("<tr>") +
                    String("<td class=\"table-sub-head\" colspan=2>") + String("Laser distance sensor meter value (LIDAR)")  + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + baseMQTTHen_HouseHatchTopic() + String("<code>lidar/distanceToObjectCm</code>") + String("</td>") +
                    String("<td>") + String("Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code>")  + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + baseMQTTHen_HouseHatchTopic() + String("<code>lidar/strengthOrQualityOfReturnSignal</code>") + String("</td>") +
                    String("<td>") + String("Strength quality of returned signal in arbitrary units. Range: <code>-1, 0 - 32767</code>")  + String("</td>") +
                String("</tr>") +
		        String("<tr>") +
                    String("<td>") + baseMQTTHen_HouseHatchTopic() + String("<code>lidar/temperatureInternalOfLidarSensorChipCelsius</code>") + String("</td>") +
                    String("<td>") + String("Temperature of LIDAR sensor chip, range: <code>-25 &deg;C - 125 &deg;C</code>")  + String("</td>") +
                String("</tr>") +
	        String("</tbody>") +
        String("</table>");

    String html = htmlEnveloper("Device info - Chicken house hatch", htmlBodyContent);
    _server.send(200, "text/html", html);
}

void HTTPWebServer::routeGetOpenHatch() {
    _hatchRequest.setAction(HatchRequestAction::OPEN_HATCH);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Opening hatch");
}

void HTTPWebServer::routeGetCloseHatch() {
    _hatchRequest.setAction(HatchRequestAction::CLOSE_HATCH);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Closing hatch");
}

void HTTPWebServer::routeGetPushActuator() {
   _hatchRequest.setAction(HatchRequestAction::DOWN);
   _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Extends actuator...");
}

void HTTPWebServer::routeGetStopActuator() {
    _hatchRequest.setAction(HatchRequestAction::STOP);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Turned off actuator");
}

void HTTPWebServer::routeGetPullActuator() {
    _hatchRequest.setAction(HatchRequestAction::UP);
    _hatchRequest.setAcknowledged(false);
    _server.send(200, "text/plain", "Contracting actuator...");
}

void HTTPWebServer::routeGetLidarSensorData() {

    String lidarSensorData = 
        String("{") +
            String("\"distanceToObjectCm\":") + String(_currentMeterData.distanceToObjectCm)  + String(",") +
            String("\"strengthOrQualityOfReturnSignal\":") + String(_currentMeterData.strengthOrQualityOfReturnSignal)  + String(",") +
            String("\"temperatureInternalOfLidarSensorChipCelsius\":") + String(_currentMeterData.temperatureInternalOfLidarSensorChipCelsius)   +
        String("}");
    _server.send(200, "application/json", lidarSensorData.c_str());
}

void HTTPWebServer::routeGetNotFound() {
    _server.send(404, "text/plain", "Not found");
}

void HTTPWebServer::handleClient() {
    _server.handleClient();
}