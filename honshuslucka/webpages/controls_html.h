
#ifndef CONTROLS_HTML_H
#define CONTROLS_HTML_H


const char CONTROLS_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Controls - Chicken house hatch</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <link rel="stylesheet" href="http://10.0.0.71/styleSimple_css">
</head>
<body>
    <h1 class="title">Controls - Chicken house hatch</h1>
    <h3>Hatch processes</h3>
    <table>
        <thead>
            <tr>
                <th>Process</th>
                <th>Description</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><button id='btnOpenHatch' type='button' class="button-symbol">&UpArrowBar;</button></td>
                <td>Open hatch</td>
            </tr>
            <tr>
                <td><button id='btnCloseHatch' type='button' class="button-symbol">&DownArrowBar;</button></td>
                <td>Close hatch</td>
            </tr>
        </tbody>
    </table>
    <h3>Actuator control</h3>
    <table>
        <thead>
            <tr>
                <th>Direction</th>
                <th>Description</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><button id="btnPull" type="button" class="button-symbol">&#x21e7;</button></td>
                <td>Contracting actuator piston</td>
            </tr>
            <tr>
                <td><button id="btnTurnOff" type="button" class="button-symbol">&#x220E;</button></td>
                <td>Stop actuator engine</td>
            </tr>
            <tr>
                <td><button id="btnPush" type="button" class="button-symbol">&#x21e9;</button></td>
                <td>Extends actuator piston</td>
            </tr>
        </tbody>
    </table>
    <h3>Detector</h3>
    <table>
        <thead>
            <tr>
                <th>Value</th>
                <th>Measurement description</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><span id="spanLidarDistanceToObjectCm"></span></td>
                <td>Distance to object from LIDAR sensor</td>
            </tr>
        </tbody>
    </table>

    <nav class="navigation-links"><div><a href="//10.0.0.71/">Control</a></div><div>|</div><div><a href="//10.0.0.71/info">Device info</a></div></nav>
    <div class="info">
        <p>This software is distributed under <a href = "https://en.wikipedia.org/wiki/MIT_License">MIT License</a>. Source code on <a href="https://github.com/HaunsTM">Github - HaunsTM</a></p>
    </div>

    <script src="http://10.0.0.71/javascriptAxios_js">
    </script>

    <script src="http://10.0.0.71/javascriptKnockout_js">
    </script>

    <script src="http://10.0.0.71/javascriptPaho_js">
    </script>

    <script src="http://10.0.0.71/javascriptParameters_js">
    </script>

    <script>

        const grpPressButtons = [
            {'id': 'btnOpenHatch', 'urlStart': 'pullActuator'},
            {'id': 'btnCloseHatch', 'urlStart': 'pullActuator'},
            {'id': 'btnPull', 'urlStart': 'pullActuator', 'urlEnd': 'stopActuator'},
            {'id': 'btnPush', 'urlStart': 'pushActuator', 'urlEnd': 'stopActuator'},
            {'id': 'btnTurnOff', 'urlStart': 'stopActuator', 'urlEnd': 'stopActuator'}
        ];
        
        const BASE_API_URL = 'http://10.0.0.71';

        const Timer = (ms) => {
            let id;
            const start = () => new Promise(
                resolve => {
                    if (id === -1) {
                        throw new Error('Timer already aborted');
                    }
                    id = setTimeout(resolve, ms);
                }
            );

            const abort = () => {
                if (id !== -1 || id === undefined) {
                    clearTimeout(id);
                    id = -1;
                }
            };

            return {start, abort}
        };

        const removeAllButtonsStatusClasses = () => {
            for (let i = 0; i < grpPressButtons.length; i++) {                
                let button = document.getElementById(grpPressButtons[i].id);
                button.classList.remove('button-success', 'button-remove');
            }
        };
        
        const setButtonClass = async (buttonId, success, timeoutResponseStatusColorOnButton) => {
            const button = document.getElementById(buttonId);
            
            removeAllButtonsStatusClasses();

            const statusClass = success ? 'button-success' : 'button-error';

            if (timeoutResponseStatusColorOnButton) {
                const addPause = Timer(3*1000);
                button.classList.add(statusClass);
                await addPause.start();
                button.classList.remove(statusClass);
            } else {                
                button.classList.add(statusClass);
            }
        };

        const callAPIFromButton = async (e, endpoint, timeoutResponseStatusColorOnButton) => {

            try {
                const response = await axios.get(`${BASE_API_URL}/${endpoint}`);
                // setButtonClass(e.target.id, response.status, true);
                return response;
            } catch (errors) {                
                // setButtonClass(e.target.id, false, true);
                console.error(errors);
            }
        };

        const setUpEventHandler = (pressButton) => {

            let button = document.getElementById(pressButton.id);

            button.urlStart = pressButton.urlStart;
            button.urlEnd = pressButton.urlEnd;

            button.addEventListener('touchstart', handleButtonPress, {passive: false});
            button.addEventListener('touchend', handleButtonRelease, {passive: false});
            button.addEventListener('mousedown', handleButtonPress);
            button.addEventListener('mouseup', handleButtonRelease);
        };

        const handleButtonPress = (e) => {
            callAPIFromButton(e, e.currentTarget.urlStart, true);
            e.stopPropagation();
            e.preventDefault();
        };

        const handleButtonRelease = (e) => {
            callAPIFromButton(e, e.currentTarget.urlEnd, true);
            e.stopPropagation();
            e.preventDefault();
        };

        grpPressButtons.forEach( 
            (pressButton, index, array) => { setUpEventHandler(pressButton); }
        );

// MQTT
        // called when the client connects
        const onConnect = () => {
            client.subscribe(parameters.mqtt.publishTopics.lidarDistanceToObjectCm);
            client.subscribe(parameters.mqtt.publishTopics.actuatorAction);
        }

        // called when the client loses its connection
        const onConnectionLost = async (responseObject) => {
            if (responseObject.errorCode !== 0) {
                console.log(`Failed in connection with MQTT-broker: ${responseObject.errorMessage}`);
            }
        }

        // called when a message arrives
        const onMessageArrived = async (message) => {
            const topic = message.destinationName;
            switch (topic) {
                case parameters.mqtt.publishTopics.lidarDistanceToObjectCm:
                    document.getElementById('spanLidarDistanceToObjectCm').textContent = message.payloadString;                
                    break;
                case parameters.mqtt.publishTopics.actuatorAction:
                    removeAllButtonsStatusClasses();
                    switch (message.payloadString) {
                        case 'PULL':
                            await setButtonClass('btnPull', true, false);
                            break;
                        case 'TURN_OFF':
                            await setButtonClass('btnTurnOff', true, true);
                            break;
                        case 'PUSH':
                            await setButtonClass('btnPush', true, false);
                            break;
                        default:
                            console.error(`Unknown Actuator action: ${topic}`);
                            break;
                    }
                    break;
                default:
                    console.error(`Unknown MQTT-topic: ${topic}`);
                    break;
            }

        }   

        // Create a client instance
        client = new Paho.MQTT.Client(parameters.mqtt.hostname, Number(parameters.mqtt.port), '/', parameters.mqtt.clientId);

        parameters.mqtt.connectionOptions.onSuccess = onConnect;
        // connect the client
        client.connect(parameters.mqtt.connectionOptions);
        
        // set callback handlers
        client.onConnectionLost = onConnectionLost;
        client.onMessageArrived = onMessageArrived;

    
    </script>
</body>
</html>
)=====";

#endif