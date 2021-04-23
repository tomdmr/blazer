Blazer-Projekt
==============

Das Ganze ist inspiriert durch einen Beitrag vom Volleyballfreak
Trainertalk. Ich hatte diese Blink-Dinger schon mal bei den Roten
Raben beim Warm-Up gesehen und damals schon gedacht: "Will haben". Im
TT habe ich dann gehört, wie die heißen, und was sie kosten. "Och nö,
dann nicht". Aber das nagende Gefühl "das kann man doch auch selber
bauen" blieb. So here we go.


Die harte Ware
--------------

Meine erste Mutmaßung war das ist was für einen kleinen RasPi. Nein,
falsch, ein ESP32 oder ESP8266 ist billiger und reicht aus. Bei mir
basiert das auf einem ESP32 DevBoard, das Stück für etwa 7€. Dazu
kommt noch ein LiPo-Akku mit 3.7V und ein kleines Board für die
Ladeelektronik für etwa 1.50€. Dann noch eine Hohlsteckerbuchse für
den Ladestecker und ein Schiebeschalter, um den ESP vom Strom zu trennen.

Dann soll das Teil rote, grüne und blaue LEDs treiben. Dazu brauchen
wir je Farbe einen Treibertransistor, einen Basisvorwiderstand und
einen R zur Strombegrenzung der LEDs.Ich wollte eh mal wider mit KiCAD
spielen, also habe ich da eine kleine Platine designed, die der
Platinenbelichter recht preiswert ätzt. Lochraster mag ich nicht. 
