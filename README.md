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

Die weiche Ware
---------------

Das Designziel war: Es soll überall laufen. Das beudeutet eigentlich
immer: Irgendwie HTML plus JavaScript. Das hat ein paar Implikationen
zur Sicherheit. Mein Set-up ist ein passwort gesichertes Internet, das
keine Verbindung zur Welt hat. Also eine FritzBox in der Halle, die
einen Access Point aufspannt. Jeder andere WLAN-AP tut es natürlich
auch, zur Not ein Handy.

Nächstes Ziel: Es soll ohne extra Server arbeiten, sondern nur mit
lokalen HTML-Files. Das erfordert ein paar Tweaks, die es nicht ratsam
erscheinen lassen, das ganze Konstrukt in einem zugänglichen Netz
aufzuhängen.

Die wichtigen Dateien hier sind `blazer.css`, `blazer.js`, sowie die
HTML-Files. In `blazer.css` ist etwas CSS für das Aussehen, aber ich
bin Ingenieur, kann also mühelos 256 Grautöne unterscheiden, habe aber
kein Farbempfinden...  `blazer.js` enthält eine Sammplung von
JavaScript-Funktionen, die aus den Beispielen heraus fakturiert wurden
und sich irgendwie als sinnvoll erwiesen haben. 

### Konfiguration ###

Dies erfolgt über

### sample01 ###

Das file holt sich beim Laden den Cookie und baut dynamisch eine
Tabelle mit den bekannten Blazern auf. Alle werden angetriggert, ihre
LEDs auszuschalten. Danach können einzelne LEDs per Klick ein- und
ausgeschaltet werden.

### sample02 ###



<!-- Find: http://IP#/get : Returns the status of LED
as RGB triple 111 (all on) to 000 (all off)

     Change one:
     http://IP#/set?[R|G|B]=[0|1]

     Change all:
     http://IP#/SET?[0|1][0|1][0|1]  : Send triplet

     http://IP#/delay?
-->
