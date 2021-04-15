
#ifndef INFO_HTML_H
#define INFO_HTML_H


const char INFO_HTML[] PROGMEM = R"=====(<!DOCTYPE html>
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
                <th></th>
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
                <th colspan=2>Wifi</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Channel</td>
                <td><span data-bind="text: wifi.channel"></td>
            </tr>
            <tr>
                <td>IP</td>
                <td><span data-bind="text: wifi.localIP"></td>
            </tr>
            <tr>
                <td>SSID</td>
                <td><span data-bind="text: wifi.SSID"></td>
            </tr>
        </tbody>
    </table>
    <h3>Control requests</h3>
    <table>
        <thead>
            <tr>
                <th>Request</th>
                <th><code>GET</code></th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Close hatch</td>
                <td><a href="//10.0.0.71/closeHatch">//10.0.0.71/closeHatch</a></td>
            </tr>
            <tr>
                <td>Open hatch</td>
                <td><a href="//10.0.0.71/openHatch">//10.0.0.71/openHatch</a></td>
            </tr>
            <tr>
                <td>Contracting actuator piston</td>
                <td><a href="//10.0.0.71/pullActuator">//10.0.0.71/pullActuator</a></td>
            </tr>
            <tr>
                <td>Stop actuator engine</td>
                <td><a href="//10.0.0.71/stopActuator">//10.0.0.71/stopActuator</a></td>
            </tr>
            <tr>
                <td>Extends actuator piston</td>
                <td><a href="//10.0.0.71/pushActuator">//10.0.0.71/pushActuator</a></td>
            </tr>
        </tbody>
    </table>
    <h3>Other requests</h3>
    <table>
        <thead>
            <tr>
                <th>Request</th>
                <th><code>GET</code></th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>
                    Get current read measurement from LIDAR detector in a JSON:<br /><br />
<pre>{
    "distanceToObjectCm":"xxx",
    "strengthOrQualityOfReturnSignal":"yyy",
    "temperatureInternalOfLidarSensorChipCelsius":"zzz"
}</pre>
                </td>
                <td><a href="//10.0.0.71/lidarSensorData">//10.0.0.71/lidarSensorData</a></td>
            </tr>
        </tbody>
    </table>
    <h3>MQTT</h3>
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
            <tr>
                <td class="table-sub-head" colspan=2>Published topics</td>
            </tr>
            <tr>
                <td class="table-sub-head" colspan=2>Actuator</td>
            </tr>
            <tr>
                <td>Action {UP, TURN_OFF, DOWN}</code></td>
                <td><code><td><span data-bind="text: mqtt.subscriptionTopics.actuatorAction"></span></td></code></td>
            </tr>
            <tr>
                <td class="table-sub-head" colspan=2>Laser distance sensor meter value (LIDAR)</td>
            </tr>
            <tr>
                <td>Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code></td>
                <td><code><td><span data-bind="text: mqtt.subscriptionTopics.actuatorAction"></span></td></code></td>
            </tr>
            <tr>
                <td>Strength quality of returned signal in arbitrary units. Range: <code>-1, 0 - 32767</code></td>
                <td><code>iot/hen_house/hatch/</code><code>lidar/strengthOrQualityOfReturnSignal</code></td>
            </tr>
            <tr>
                <td>Temperature of LIDAR sensor chip, range: <code>-25 &deg;C - 125 &deg;C</code></td>
                <td><code>iot/hen_house/hatch/</code><code>lidar/temperatureInternalOfLidarSensorChipCelsius</code></td>
            </tr>
        </tbody>
    </table>
    <nav class="navigation-links"><a href="//10.0.0.71/">Control</a>   |   <a href="//10.0.0.71/info">Device info</a></nav>
    <div class="info">
        <p>This software is distributed under <a href = "https://en.wikipedia.org/wiki/MIT_License">MIT License</a>. Source code on <a href="https://github.com/HaunsTM">Github - HaunsTM</a></p>
    </div>
    
    <script src="http://10.0.0.71/javascriptKnockout_js">
    </script>

    <script src="http://10.0.0.71/javascriptParameters_js">
    </script>

    <script>
        const viewModelKnockout = {
            'device': {
                'firmwareVersion': ko.observable(parameters.device.firmwareVersion),
                'macAddress': ko.observable(parameters.device.macAddress),
                'serialMonitorBaud': ko.observable(parameters.device.serialMonitorBaud),
            },
            'mqtt': {
                'subscriptionTopics': {
                    'lidarDistanceToObjectCm': ko.observable(parameters.mqtt.subscriptionTopics.lidarDistanceToObjectCm),
                    'actuatorAction': ko.observable(parameters.mqtt.subscriptionTopics.actuatorAction)
                },
                'hostname': ko.observable(parameters.mqtt.hostname),
                'clientId': ko.observable(parameters.mqtt.clientId),
                'connectionOptions': {
                    'userName': ko.observable(parameters.mqtt.connectionOptions.userName),
                    'password': ko.observable(parameters.mqtt.connectionOptions.password),
                    'keepAliveInterval': ko.observable(parameters.mqtt.connectionOptions.keepAliveInterval),
                },
                'port': ko.observable(parameters.mqtt.port)
            },
            'wifi': {
                'channel': ko.observable(),
                'localIP': ko.observable(),
                'SSID': ko.observable(),
            }
        };
        ko.applyBindings(viewModelKnockout);
    </script>
</body>
</html>
)=====";

#endif