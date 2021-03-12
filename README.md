# honslucka
Styrlogik till ESP8266 för en linjemotor för en hönslucka


// Complete Instructions: https://robojax.com/learn/arduino/?vid=robojax_arduino_actuator


Indikator-LED
```
allmänt felläge		error state				3 korta blinkningar varje sekund
mqtt mottaget		on mqtt received		2 korta blinkningar
http mottaget		on http request         2 korta blinkningar
rörelse				on performing action	1 kort blinkning varje sekund
stand by			stand by				1 kort blinkning var 5 s
stand by utan wifi  stand by no wifi		3 kort blinkningar var 5 s


```