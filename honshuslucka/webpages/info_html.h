
#ifndef INFO_HTML_H
#define INFO_HTML_H


const char INFO_HTML[] PROGMEM = R"=====(
    <!DOCTYPE html>
<html>
<head>
    <title>Device info - Chicken house hatch</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="http://10.0.0.71/styleSimple_css">
</head>
<body>
    <h1 class="title">Device info - Chicken house hatch</h1>
    <h2>Software</h2>
    <p>This software is intended for Arduino and compatible PLCs. The purpose is to use the PLC to enable control of a linear actuator (12 volt DC motor) via wifi / via pushbuttons, where the ultimate goal is to hoist up / down a guillotine - type door to open / close the entrance to a chicken house.</p>
    <h3>Device</h3>
    <table>
        <thead>
            <tr>
                <th>Parameter</th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Firmware version</td>
                <td><span data-bind="text: device.firmwareVersion"></span></td>
            </tr>        
            <tr>
                <td>MAC address</td>
                <td><span data-bind="text: device.macAddress"></span></td>
            </tr>
            <tr>
                <td>Serial monitor communication speed</td>
                <td><span data-bind="text: device.serialMonitorBaud"></span></td>
            </tr>
        </tbody>
    </table>
    <h3>Wifi</h3>
    <table>
        <thead>
            <tr>
                <th>Parameter</th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Channel</td>
                <td><span data-bind="text: wifi.channel"></span></td>
            </tr>
            <tr>
                <td>IP</td>
                <td><span data-bind="text: wifi.localIP"></span></td>
            </tr>
            <tr>
                <td>SSID</td>
                <td><span data-bind="text: wifi.SSID"></span></td>
            </tr>
        </tbody>
    </table>
    <h3>HTTP requests</h3>
    <h4>Controls</h4>
    <table>
        <thead>
            <tr>
                <th>HTTP request</th>
                <th>Description</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><a href="//10.0.0.71/closeHatch">//10.0.0.71/closeHatch</a></td>
                <td>Close hatch</td>
            </tr>
            <tr>
                <td><a href="//10.0.0.71/openHatch">//10.0.0.71/openHatch</a></td>
                <td>Open hatch</td>
            </tr>
            <tr>
                <td><a href="//10.0.0.71/pullActuator">//10.0.0.71/pullActuator</a></td>
                <td>Contracting actuator piston</td>
            </tr>
            <tr>
                <td><a href="//10.0.0.71/stopActuator">//10.0.0.71/stopActuator</a></td>
                <td>Stop actuator engine</td>
            </tr>
            <tr>
                <td><a href="//10.0.0.71/pushActuator">//10.0.0.71/pushActuator</a></td>
                <td>Extends actuator piston</td>
            </tr>
        </tbody>
    </table>
    <h4>Other</h4>
    <table>
        <thead>
            <tr>
                <th>HTTP request</th>
                <th>Expected response</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>
                    <a href="//10.0.0.71/lidarSensorData">//10.0.0.71/lidarSensorData</a><br />
                    Get current read measurement from LIDAR detector in a JSON:<br />
                </td>
                <td>
<pre>{
    "distanceToObjectCm":"xxx",
    "strengthOrQualityOfReturnSignal":"yyy",
    "temperatureInternalOfLidarSensorChipCelsius":"zzz"
}</pre>
                </td>
            </tr>
        </tbody>
    </table>
    <h3>MQTT</h3>
    <h4>Settings</h4>
    <table>
        <thead>
            <tr>
                <th></th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Host</td>
                <td><span data-bind="text: mqtt.hostname"></span></td>
            </tr>
            <tr>
                <td>Port</td>
                <td><span data-bind="text: mqtt.port"></span></td>
            </tr>
            <tr>
                <td>Username</td>
                <td><span data-bind="text: mqtt.connectionOptions.userName"></span></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><span data-bind="text: mqtt.connectionOptions.password"></span></td>
            </tr>
        </tbody>
    </table>
    <h4>Topics</h4>
    <table>
        <thead>
            <tr>
                <th>Description</th>
                <th>Topic</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Action {UP, TURN_OFF, DOWN}</code></td>
                <td><code><span data-bind="text: mqtt.publishTopics.actuatorAction"></span></code></td>
            </tr>
            <tr>
                <td>Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code></td>
                <td><code><span data-bind="text: mqtt.publishTopics.lidarDistanceToObjectCm"></span></code></td>
            </tr>
            <tr>
                <td>Strength quality of returned signal in arbitrary units. Range: <code>-1, 0 - 32767</code></td>
                <td><code><span data-bind="text: mqtt.publishTopics.lidarStrengthOrQualityOfReturnSignal"></span></td>
            </tr>
            <tr>
                <td>Temperature of LIDAR sensor chip, range: <code>-25 &deg;C - 125 &deg;C</code></td>
                <td><code><span data-bind="text: mqtt.publishTopics.lidarTemperatureInternalOfLidarSensorChipCelsius"></span></td>
            </tr>
        </tbody>
    </table>

    <nav class="navigation-links"><div><a data-bind="attr: { href: computed.href.controls }">Controls </a></div><div>|</div><div><a data-bind="attr: { href: computed.href.info }">Device info</a></div></nav>
    <div class="info">
        <p>This software is distributed under <a href = "https://en.wikipedia.org/wiki/MIT_License">MIT License</a>. Source code on <a href="https://github.com/HaunsTM">Github - HaunsTM</a></p>
    </div>
    
    <script src="http://10.0.0.71/javascriptKnockout_js">
    </script>

    <script src="http://10.0.0.71/javascriptParameters_js">
    </script>

    <script>
        function viewModelKnockout() {
            const _self = this;
            
            _self.device = {
                firmwareVersion: ko.observable(parameters.device.firmwareVersion),
                macAddress: ko.observable(parameters.device.macAddress),
                serialMonitorBaud: ko.observable(parameters.device.serialMonitorBaud),
            };
            _self.mqtt = {
                publishTopics: {
                    actuatorAction: ko.observable(parameters.mqtt.publishTopics.actuatorAction),
                    lidarDistanceToObjectCm: ko.observable(parameters.mqtt.publishTopics.lidarDistanceToObjectCm),
                    lidarStrengthOrQualityOfReturnSignal: ko.observable(parameters.mqtt.publishTopics.lidarStrengthOrQualityOfReturnSignal),
                    lidarTemperatureInternalOfLidarSensorChipCelsius: ko.observable(parameters.mqtt.publishTopics.lidarTemperatureInternalOfLidarSensorChipCelsius),
                },
                hostname: ko.observable(parameters.mqtt.hostname),
                clientId: ko.observable(parameters.mqtt.clientId),
                connectionOptions: {
                    userName: ko.observable(parameters.mqtt.connectionOptions.userName),
                    password: ko.observable(parameters.mqtt.connectionOptions.password),
                    keepAliveInterval: ko.observable(parameters.mqtt.connectionOptions.keepAliveInterval),
                },
                port: ko.observable(parameters.mqtt.port)
            };
            _self.wifi = {
                channel: ko.observable(parameters.wifi.channel),
                localIP: ko.observable(parameters.wifi.localIP),
                SSID: ko.observable(parameters.wifi.SSID),
            };
            _self.computed = (function () {                
                return {
                    href: {
                        closeHatch: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/closeHatch" ; }),
                        controls: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/" ; }),
                        info: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/info" }),
                        lidarSensorData: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/lidarSensorData" }),
                        pullActuator: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/pullActuator" }),
                        pushActuator: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/pushActuator" }),
                        stopActuator: ko.pureComputed( () => { return "//" + _self.wifi.localIP() + "/stopActuator" }),
                    }
                }
            })();
            return _self;
        };
        ko.applyBindings(viewModelKnockout);
    </script>
</body>
</html>
)=====";

#endif