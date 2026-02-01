# Technische toelichting – Semaphore Alphabet Trainer

Deze toelichting is bedoeld voor **scouts, leiding en makers** die willen begrijpen *hoe* het project werkt, niet alleen *wat* het doet.

Je hoeft geen programmeerexpert te zijn, maar een beetje interesse in techniek helpt 🙂

---

## 1. Wat doet dit systeem eigenlijk?

De Arduino bestuurt **7 LEDs** die samen de standen van een **semafoor-alfabet** voorstellen.

Met één drukknop ga je steeds **één stap verder**:
- een letter
- of (soms) een cijfer

Er is bewust gekozen voor **stap-voor-stap bediening**:
- jij bepaalt het tempo
- geen informatie gaat te snel

---

## 2. Twee hoofdmodi

De stand van een schakelaar bepaalt de werking.

### Text-modus
- De Arduino stuurt een **vaste tekst**
- Elke knopdruk = **volgende letter**
- Spaties → alle LEDs uit

### Random-modus
- Elke knopdruk = **nieuw, willekeurig teken**
- Meestal letters, af en toe een cijfer
- Geschikt om te oefenen

---

## 3. Waarom cijfers anders werken dan letters

In semafoorverkeer worden cijfers **niet zomaar** verstuurd.

Daarom gebruikt het systeem:
- een **start-teken** vóór cijfers
- een **eind-teken** na cijfers

Zo weet de ontvanger:
> “Nu komen er cijfers aan”

In random-modus wordt **elk cijfer apart verzonden**, met eigen start- en eindteken.

---

## 4. Slimme random-logica (geen echte chaos)

Hoewel het ‘random’ heet, zit er **bewuste logica** achter.

### 4.1 Meer letters dan cijfers

Letters zijn belangrijker om te oefenen dan cijfers.

Daarom is de kansverdeling:
- **20× letter**
- **2× cijfer**

Dat voelt natuurlijker dan 50/50.

---

### 4.2 Nooit twee cijfers achter elkaar

Na een cijfer volgen altijd **minstens 5 letters**.

Waarom?
- cijfers zijn moeilijker
- cijfers moeten opvallen
- het voorkomt verwarring

---

### 4.3 Geen herhaling van tekens

Het systeem onthoudt:
- wat het **laatste teken** was

Daarna geldt:
- nooit dezelfde letter twee keer
- nooit hetzelfde cijfer twee keer

Dit houdt je **alert** tijdens het oefenen.

---

## 5. Knoppen en betrouwbaarheid (belangrijk!)

Echte drukknoppen zijn mechanisch.
Dat betekent:
- ze maken geen perfect contact
- vooral oude knoppen ‘stuiteren’

Dit heet **contactdender**.

### De oplossing in dit project

- De Arduino kijkt **niet meteen** naar de knop
- Hij wacht tot het signaal **stabiel** is
- Elke druk wordt **maar één keer** geaccepteerd

Resultaat:
- geen dubbele stappen
- geen overslaan
- ook betrouwbaar met oude hardware

---

## 6. Opstarttest & idle-modus

### Opstarttest

Na inschakelen:
- loopt automatisch een **vaste testreeks**
- controleert:
  - alle LEDs
  - alle standen

Dit gebeurt **één keer** en kan niet onderbroken worden.

---

### Idle / attract mode

Als:
- de knop **15 seconden niet** wordt ingedrukt

dan:
- start automatisch de testreeks
- blijft herhalen

Zodra iemand op de knop drukt:
- stopt dit meteen
- gebruiker krijgt controle terug

Handig voor:
- demonstraties
- tentoonstellingen
- open dagen

---

## 7. Waarom sommige delen ‘langzaam’ zijn

Je ziet in de code soms `delay()` staan.

Dat is **bewust gekozen**:
- semafoor is visueel
- timing moet voorspelbaar zijn
- eenvoud is hier beter dan complexiteit

Voor leerdoeleinden is dit juist een voordeel.

---

## 8. Wat kun je zelf aanpassen?

Zonder veel kennis kun je al veel veranderen:

- de tekst die verzonden wordt
- hoe vaak cijfers voorkomen
- hoe snel alles gaat
- hoe lang het systeem wacht voor idle-modus

Dit maakt het project **perfect om mee te experimenteren**.

---

## 9. Wat leer je van dit project?

Dit project laat zien:

- hoe hardware en software samenwerken
- hoe je met simpele logica gedrag ‘menselijk’ maakt
- hoe je een systeem robuust maakt voor echte gebruikers
- dat techniek niet ingewikkeld hoeft te zijn om slim te zijn

---

## 10. Tot slot

Zie dit niet als een ‘af product’, maar als:

> een startpunt om zelf te ontdekken

Probeer dingen.
Verander iets.
Kijk wat er gebeurt.

Dat is precies waar scouting en techniek elkaar raken 🚀

