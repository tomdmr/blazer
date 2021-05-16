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

Als Gehäuse habe ich mir ein formschönes HT-Rohr mit zwei Deckeln
ausgesucht. Technisch heisst das "Doppelmuffe" und die Deckel
firmieren unter "Muffenstopfen". Es gibt sie in verschiedenen
Nennweiten, DN32, DN40, DN50 und DN70. Die Zylinderhöhe ist immer so
etwa 90-110mm. Was man tatsächlich einsetzt bleibt jedem selbst
überlassen. DN32 könnte knap sein, ich habe mich für DN50 entschieden. 

Auf den einen Stopfen kommt eine runde Holzscheibe, die als Träger für
die LEDs dienen. Im Kreis werden 5mm Löcher für je 3 rote, grüne, und
blaue LEDs gebohrt. In die Mitte kommt ein weiteres Loch. Rund um das
zentrale Loch wird Alufolie geklebt, und von oben wird eine Schraube
mit einer Unterlegscheibe durch das Loch geführt, mit einer weiteren
Unterlegscheibe gehalten und mit einer Mutter fixiert.Zwischen der
unteren Scheibe und der Mutter wird ein Kabel fixiert. Dies wird auf
der Außenseite die Sensorfläche.  Die Löcher gehen durch den
Muffenstopfen, nud die Holzscheibe wird mit Pattex aufgeklebt. Die
LEDs werden mit Heißkleber in den Löchern fxiert. Sie sollten ein
bischen mit der Kuppel herausschauen, damit sie aus einem größeren
Winkel zu sehen sind. Die Kathoden werden alle mit V+ verbunden. Die
Anoden der jeweiligen Farben gehen auch zusammen und dann zur
Treiberplatine.

Im unteren Teil des Zylinders werden zwei Öffnungen vorgesehen, ein
rundes für die Ladebuchse und ein rechteckiges für den Powerschalter
für den MCU.

Im inneren werkelt als zentrales Element ein ESP32-NodeMCU. Versorgt
wird er von einem Akku und einer einer Ladereglerplatine. Aus den
Infos im Internet war nicht ganz klar, wie Laden und Betrieb
zusammenpassen, deswegen ist in der V+ Leitung zum Board der Schalter
eingebaut.

Pin 15 geht auf die Sensorfläche für die Berührungsrkennung. Pins 17,
18 und 19 gehen als Ausgänge auf die Treiberplatine.


Die weiche Ware
---------------

Die ESPs können über Bluetooth oder WLAN angesprochen werden. BT ist
nicht wirklich Multipoint-fähig, also war WLAN erste Wahl. Das
Designziel war: Clients sollen auf allem laufen. Das beudeutet eigentlich
immer: Irgendwie HTML plus JavaScript. Das hat ein paar Implikationen
zur Sicherheit. Mein Set-up ist ein Passwort gesichertes Internet, das
keine Verbindung zur Welt hat. Also eine FritzBox in der Halle, die
einen Access Point aufspannt. Jeder andere WLAN-AP tut es natürlich
auch zur Not ein Handy.

Nächstes Ziel: Es soll ohne extra Server arbeiten, sondern nur mit
lokalen HTML-Files. Das erfordert ein paar Tweaks, die es nicht ratsam
erscheinen lassen, das ganze Konstrukt in einem zugänglichen Netz
aufzuhängen.

Die erste Version basierte im Wesentlichen auf GET-Requests, die war
mir aber zu unflexibel. Die jetzt realisierte Version basiert auf
Web-Sockets.

Die wichtigen Dateien hier sind `blazer.css`, `blazer2.js`, sowie die
HTML-Files. In `blazer.css` ist etwas CSS für das Aussehen, aber ich
bin Ingenieur, kann also mühelos 256 Grautöne unterscheiden, habe aber
kein Farbempfinden...  `blazer2.js` enthält eine Sammplung von
JavaScript-Funktionen, die aus den Beispielen heraus fakturiert wurden
und sich irgendwie als sinnvoll erwiesen haben. 

### Server-Seite ###

Die Programmierung erfolgte mit der Arduino-IDE. Durch die
Entscheidung für die Websockets wurde auf die Standard-Implementation
verzichtet. Stattdessen wurde die ESPAsyncWebServer-Implementation mit
asynchronem TCP eingesetzt. Die Implementierung orientiert sich im
Wesentlichen an der Referenzimplementierung. Auf der HTML-Seiite
passiert relativ wenig. Die Websocket-Seite reagiert asynchron auf
Anfragen von den Clients. Aus diesen Anfragen resultieren meist
Änderungen des LED-Zustands, der dann an die Clients zurückgemeldet
werden. Genauso wird bei Berührung eine Nachricht an die Cllients geschickt. 

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
