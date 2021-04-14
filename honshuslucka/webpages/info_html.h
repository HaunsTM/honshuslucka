
#ifndef INFO_HTML_H
#define INFO_HTML_H


const char INFO_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Controls - Chicken house hatch</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="http://10.0.0.71/styleSimple_css">
</head>
<body>
    <h1 class="title">Device info - Chicken house hatch</h1>
    <h2>Software</h2>
    <p>This software is intended for Arduino and compatible PLCs. The purpose is to use the PLC to enable control of a linear actuator (12 volt DC motor) via wifi / via pushbuttons, where the ultimate goal is to hoist up / down a guillotine - type door to open / close the entrance to a chicken house.</p>
    <table>
        <thead>
        <tr>
            <th colspan=2>This device</th>
        </tr>
        </thead>
        <tbody>
        <tr>
            <td>Firmware version</td>
            <td>2021-04-07</td>
        </tr>
        <tr>
            <td>Serial monitor communication speed</td>
            <td>115200</td>
        </tr>
        </tbody>
    </table>
    <table>
        <thead>
        <tr>
            <th colspan=2>Wifi</th>
        </tr>
        </thead>
        <tbody>
        <tr>
            <td>MAC</td>
            <td>3C:61:05:D0:47:56</td>
        </tr>
        <tr>
            <td>Assigned IP</td>
            <td>10.0.0.71</td>
        </tr>
        <tr>
            <td>SSID</td>
            <td>clariNET</td>
        </tr>
        </tbody>
    </table>
    <table>
        <thead>
        <tr>
            <th colspan=2>Control requests</th>
        </tr>
        <tr>
            <th>Action</th>
            <th>Request (<code>GET</code>)</th>
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
    <table>
        <thead>
        <tr>
            <th colspan=2>Other requests</th>
        </tr>
        <tr>
            <th>Action</th>
            <th>Request (<code>GET</code>)</th>
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
    <table>
        <thead>
        <tr>
            <th colspan=2>MQTT</th>
        </tr>
        </thead>
        <tbody>
        <tr>
            <td>Broker URL</td>
            <td>10.0.0.6</td>
        </tr>
        <tr>
            <td>Port</td>
            <td>1883</td>
        </tr>
        <tr>
            <td>Username</td>
            <td>mqtt1</td>
        </tr>
        <tr>
            <td>Password</td>
            <td>mqtt1</td>
        </tr>
        <tr>
            <td class="table-sub-head" colspan=2>Published topics</td>
        </tr>
        <tr>
            <td class="table-sub-head" colspan=2>Actuator</td>
        </tr>
        <tr>
            <td><code>iot/hen_house/hatch/</code><code>actuator/action</code></td>
            <td>Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code></td>
        </tr>
        <tr>
            <td class="table-sub-head" colspan=2>Laser distance sensor meter value (LIDAR)</td>
        </tr>
        <tr>
            <td><code>iot/hen_house/hatch/</code><code>lidar/distanceToObjectCm</code></td>
            <td>Distance to target in centimeters/millimeters. Range: <code>0 - 1200</code></td>
        </tr>
        <tr>
            <td><code>iot/hen_house/hatch/</code><code>lidar/strengthOrQualityOfReturnSignal</code></td>
            <td>Strength quality of returned signal in arbitrary units. Range: <code>-1, 0 - 32767</code></td>
        </tr>
        <tr>
            <td><code>iot/hen_house/hatch/</code><code>lidar/temperatureInternalOfLidarSensorChipCelsius</code></td>
            <td>Temperature of LIDAR sensor chip, range: <code>-25 &deg;C - 125 &deg;C</code></td>
        </tr>
        </tbody>
    </table>
    <nav class="navigation-links"><a href="//10.0.0.71/">Control</a>   |   <a href="//10.0.0.71/info">Device info</a></nav>
    <div class="info">
        <p>This software is distributed under <a href = "https://en.wikipedia.org/wiki/MIT_License">MIT License</a>. Source code on <a href="https://github.com/HaunsTM">Github - HaunsTM</a></p>
    </div>
</body>
</html>
)=====";

#endif