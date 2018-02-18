# Advanced Gestures

<a name="closed-trajectory"></a>
## Eine geschlossene Trajektorie (Anfangspunkt = Endpunkt) soll an einem beliebigen Punkt angefangen werden können.
Nehmen wir als Beispiel-Pattern ein Quadrat, das in der linken oberen Ecke
beginnt und im Uhrzeigersinn gezeichnet wird:  
![pattern](img/algorithm/begin-closed-trajectory-at-any-point/pattern.png)  
Wir möchten dem Nutzer erlauben, dass Quadrat an einem beliebigen Punkt zu
beginnen.
Außerdem soll er es auch gegen den Uhrzeigersinn zeichnen dürfen.
Die Eingabe-Trajektorie im folgenden Beispiel soll als ähnlich angesehen werden: 
![initial](img/algorithm/begin-closed-trajectory-at-any-point/0-initial.png)  
Dafür muss der Startpunkt und die Punktreihenfolge bei der Normierung an das
Pattern angeglichen werden.
Das Ziel ist es diese normierte Eingabe-Trajektorie zu erhalten:  
![join input trajectory parts](img/algorithm/begin-closed-trajectory-at-any-point/4-join.png)

Dafür gehen wir wie folgt vor:

1. Berechne Indizes der Punkte mit minimalem Abstand zu Punkten des Patterns
   ![min points](img/algorithm/begin-closed-trajectory-at-any-point/1-min-points.png)  
   Im Beispiel hat der Punkt `2` der Eingabe-Trajektorie den kleinsten Abstand
   zum ersten Punkt des Patterns.
   Punkt `4` der Eingabe-Trajektorie hat den
   kleinsten Abstand zum zweiten Punkt des Patterns.
   Insgesamt erhält man `[2, 4, 5, 1]`.
   
   Implementiert in `estimate_corners` in [trajecmp/transform/rearrange_closed_trajectory.hpp](../src/trajecmp/transform/rearrange_closed_trajectory.hpp#L48).
   Bei einem Rechteck kann man ausnutzen, dass die Eckpunkte gegenüberliegen
   und muss nur für zwei Eckpunkte die minimalen und maximalen Abstände
   berechnen (siehe `estimate_rectangle_corners` in [trajecmp/gesture/rectangle.hpp](../src/trajecmp/gesture/rectangle.hpp#L62)) 
2. Zeichenrichtung herausfinden (siehe [`is_reversed_corner_index_order`](../src/trajecmp/transform/rearrange_closed_trajectory.hpp#L88))
   1. Sortiere die Indizes: `[1, 2, 4, 5]`
   2. Prüfe anhand der Indizes, ob der Punkt nach dem Startpunkt des Patterns
      auf den Startpunkt folgt (=> gleiche Zeichenrichtung wie Pattern).
      
      Punkt `2` der Eingabe-Trajektorie ist am ähnlichsten zum Startpunkt des
      Patterns.
      Punkt `4` der Eingabe-Trajektorie ist am ähnlichsten zum zweiten Punkt des
      Patterns.
      In den sortierten Indizes muss folglich auf `2` die `4` folgen.
      Dies ist im Beispiel der Fall.
      
      **Bemerkung:** Wäre die Eingabe-Trajektorie gegen den Uhrzeigersinn
      gezeichnet worden, hätte man die Indizes `[4, 2, 1, 5]` erhalten.  
      ![min points counterclockwise](img/algorithm/begin-closed-trajectory-at-any-point/1-min-points-counterclockwise.png)  
      Dann wäre Punkt `4` der Eingabe-Trajektorie am ähnlichsten zum Startpunkt
      des Patterns.
      Punkt `2` der Eingabe-Trajektorie wäre am ähnlichsten zum zweiten Punkt
      des Patterns.
      In den sortierten Indizes `[1, 2, 4, 5]` müsste auf `4` die `2` folgen.
      Da dies nicht der Fall ist, ist die Eingabe-Trajektorie in umgekehrter
      Richtung zum Pattern gezeichnet worden.

3. Teile die Eingabetrajektorie am Index des Startpunkts des Patterns
   (siehe [`rearrange_corners_according_to_indices`](../src/trajecmp/transform/rearrange_closed_trajectory.hpp#L123))  
   ![split input trajectory](img/algorithm/begin-closed-trajectory-at-any-point/3-split.png)
4. Füge Teile unter Berücksichtigung der Zeichenrichtung zusammen, um für die
   Eingabetrajektorie die gleiche Punktabfolge wie beim Pattern zu erreichen
   (siehe [`rearrange_corners_according_to_indices`](../src/trajecmp/transform/rearrange_closed_trajectory.hpp#L126))  
   ![join input trajectory parts](img/algorithm/begin-closed-trajectory-at-any-point/4-join.png)
5. Regulärer Vergleich (wie bei nicht geschlossener Trajektorie)

**Beispiele:**

* [example/sdl2/app/sdl2-rectangle.cpp](../example/sdl2/app/sdl2-rectangle.cpp)
* [example/sdl2/app/sdl2-triangle.cpp](../example/sdl2/app/sdl2-triangle.cpp)

**Bemerkung:** Sollte das Pattern ähnliche Punkte bzw. mehrmals den gleichen
Punkt enthalten, wird beim 1. Schritt für diese jeweils der gleiche Punkt der
Eingabe-Trajektorie (mit minimalem Abstand) bestimmt.
Stattdessen muss man die lokalen Minima bestimmen, um die Punkte herauszufinden,
die in Frage kommen.
Sollte das Pattern ausreichend andere Punkte enthalten, um die Reihenfolge zu
schätzen, können die ähnlichen Punkte auch ignoriert werden oder gezielt
zwischen ihren Nachbarpunkten gesucht werden.

## Kombinierte Trajektorien sollen definiert werden können (z.B. ein X, das aus 2 Strichen/Trajektorien besteht)
Eine kombinierte Trajektorie besteht aus mehreren Teil-Trajektorien.
Demnach müssen mehrere Trajektorien hintereinander aufgezeichnet werden.
Daraus ergeben sich folgende Fragen: 

- Wann beginnt eine kombinierte Trajektorie?
- Wann endet eine kombinierte Trajektorie?

Wenn die Teil-Trajektorien in einer bestimmten Reihenfolge gezeichnet werden
müssen, beginnt die aufzeichnung einer kombinierten Trajektorie, wenn die erste
Teil-Trajektorie matcht.
Die nächste aufgezeichnete Trajektorie wird mit der nächsten Teil-Trajektorie
der kombinierten Trajektorie verglichen.
Wenn sie matcht, wird die nächste Teil-Trajektorie in der Reihenfolge als
nächstes Pattern verwendet.
Ansonsten matcht die kombinierte Trajektorie nicht und der Vergleich wird
abgebrochen, d.h. der Zustand des kombinierten Trajektorien Vergleichs wird
wieder zurückgesetzt.
Der Vergleich der kombinierten Trajektorie endet spätestens mit der letzten
Teil-Trajektorie.
Sollte auch diese matchen, wird dies als Match der kombinerten Trajektorie
angesehen.

Wenn die Teil-Trajektorien in einer beliebigen Reihenfolge gezeichnet werden
dürfen, muss mit allen Teil-Trajektorien verglichen werden.
Die kombinierte Trajektorie beginnt mit der ersten Teil-Trajektorie, die matcht.
Danach wird nur noch mit den noch nicht gematchten Teil-Trajektorien verglichen.
Dies macht man solange bis alle Teil-Trajektorien gezeichnet worden sind oder
der erste Mismatch auftritt.
Sollten alle Teil-Trajektorien gematcht haben, matcht auch die kombinierte
Trajektorie. 

**Bemerkung:** Solange der Vergleich mit der kombinierten Trajektorie läuft,
können keine anderen Patterns matchen, d.h. der Vergleich mit diesen entfällt.
Außerdem müssen alle bereits gezeichneten Teile der kombinierten Trajektorie
dem Nutzer angezeigt werden.

### Beispiel
Im Beispiel [sdl2-rectangle-in-circle][sdl2-rectangle-in-circle]
soll zuerst ein Kreis und anschließend ein Rechteck gezeichnet werden.
Die genaue Vorgehensweise ist im Beispiel beschrieben.


<a name="position-size-orientation-dependend"></a>
## Es soll möglich sein, die Ähnlichkeit abhängig von der Größe, Position und Lage im Raum zu definieren
Bei Position, Größe und Lage (Orientierung) handelt es sich um Eigenschaften
einer Trajektorie, die als zusätzliche Kriterien beim Vergleich dienen können.
Oft ist eine Prüfung dieser Kriterien noch während der Vorverarbeitung der
Eingabe-Trajektorie möglich.
Für Position und Größe reicht in der Regel die Minimum Bounding Sphere aus,
die normalerweise für die Normalisierung von Größe und Position als Vorbereitung
für den Vergleich mit dem Pattern benötigt wird.
Ist ein Kriterium nicht erfüllt, kann auf weitere Vorverarbeitungsschritte
(z.B. Normalisierung der Eingabe-Trajektorie) und den Vergleich mit dem Pattern
verzichtet werden.

### Position
Der Mittelpunkt der Minimum Bounding Sphere (MBS) der Eingabe-Trajektorie wird
mit der gewünschten Position im Raum verglichen.
Soll zum Beispiel eine Trajektorie in der linken oberen Ecke gezeichnet
werden, darf das Zentrum der MBS nicht weiter als ein festgelegtes Maximum
von der linken oberen Ecke entfernt sein.  
![compare-position-using-MBS](img/algorithm/position-size-orientation-dependend/compare-position-using-MBS.png)  
Das Zentrum der MBS der roten Eingabe-Trajektorie ist zu weit entfernt.
Daher kann es sich nicht um einen Match handeln.
Ein Vergleich mit dem gewünschten Pattern ist nicht notwendig.
Das Zentrum der MBS der grünen Eingabe-Trajektorie ist nah genug.
Die gewünschte Position ist somit gegeben, aber es muss noch ein Vergleich mit
dem gewünschten Pattern erfolgen, um den vollständigen Match (z.B. Kreis oder
Rechteck in der linken oberen Ecke) zu bestätigen.
Soll z.B. ein Dreieck in der linken oberen Ecke gezeichnet werden, wäre die
Position der grünen Trajektorie zwar richtig, aber beim anschließenden
Vergleich mit dem Dreieck-Pattern käme eine Nichtübereinstimmung heraus.

Im Beispiel [sdl2-rectangle-in-circle][sdl2-rectangle-in-circle]
soll ein Rechteck in einem Kreis gezeichnet werden.
Der Kreis wird zuerst gezeichnet.
Ob das anschließende Rechteck matcht, hängt von der Position des zuvor
gezeichneten Kreises ab.
Mehr dazu am Ende des folgenden Kapitels [Größe](#Größe).

### Größe
Als Größe lässt sich der Radius bzw. Durchmesser der MBS verwenden.
Zum Beispiel kann man prüfen, ob der Durchmesser in etwa 100 beträgt,
d.h. man legt eine maximal erlaubte Abweichung fest.
Wenn diese z.B. 5 beträgt, muss der Durchmesser zwischen 95 und 105 liegen.
Sonst wird die Eingabe-Trajektorie als Nichtübereinstimmung angesehen und der
Vergleich mit dem Pattern entfällt.

Im Beispiel [sdl2-rectangle-in-circle][sdl2-rectangle-in-circle]
soll ein Rechteck in einem Kreis gezeichnet werden.
Der Kreis wird zuerst gezeichnet.
Ob das anschließende Rechteck matcht, hängt von der Position und Größe des
zuvor gezeichneten Kreises ab.
Es muss der Mittelpunkt und der Radius des Kreises `k` gespeichert werden.
Von der anschließend gezeichneten Trajektorie wird die MBS `r` berechnet.
Position und Größe passen, wenn die Mittelpunkte von `k` und `r` maximal
`k.radius + r.radius` entfernt sind.
Eine genauere Beschreibung findet man in [sdl2-rectangle-in-circle][sdl2-rectangle-in-circle].

### Lage (Orientierung)
Die Orientierung der Eingabe-Trajektorie kann bestimmt werden und ebenfalls in
die Ähnlichkeitsdefinition einfließen.
Dies ist hauptsächlich im 3D-Raum von Bedeutung.
3D zu zeichnen erfordert ein gutes räumliches Vorstellungsvermögen.
Es ist ratsam hauptsächlich in einer 2D-Ebene im 3D-Raum zeichenen zu lassen.
Dies gelingt den Nutzern leichter.

Im folgenden Beispiel hat der Nutzer in der yz-Ebene gezeichnet:  
![layer-in-3D-space](img/algorithm/position-size-orientation-dependend/layer-in-3D-space.png)  
Nach der Normalisierung befindet sich der Mittelpunkt der Minimum Bounding
Sphere im Ursprung:  
![normalized-layer-in-3D-space](img/algorithm/position-size-orientation-dependend/normalized-layer-in-3D-space.png)  
Wir möchten in diesem Beispiel den Nutzer nur in Ebenen zeichnen lassen,
die im rechten Winkel zum Boden stehen.
Der Nutzer soll also nur auf einer imaginären Wand zeichnen, die sich vor ihm
befindet.
Am Boden sollen seine Zeichnungen nicht erkannt werden.
Um dies zu erreichen, legen wir den Ortsvektor des Startpunkts der
Eingabe-Trajektorie als Orientierung fest:  
![orientation-in-3D-space](img/algorithm/position-size-orientation-dependend/orientation-in-3D-space.png)  
Wir wollen alle gezeichneten Trajektorien so um die y-Achse rotieren,
dass der Ortsvektor des Startpunkts in Richtung der x-Achse zeigt:  
![visualization-of-normalized-orientation-in-3D-space](img/algorithm/position-size-orientation-dependend/visualization-of-normalized-orientation-in-3D-space.png)  
In diesem Fall wird die Eingabe-Trajektorie also um 90° gedreht:  
![orientation-normalized-in-3D-space](img/algorithm/position-size-orientation-dependend/orientation-normalized-in-3D-space.png)  
Nun kann man die Eingabe-Trajektorie problemlos mit einem Pattern in der
xy-Ebene vergleichen.
Diese Vorgehensweise wird [hier](https://github.com/maiermic/MagicVR/blob/4cdb7eed0b4aa7b85bd25f10b0bb6abe039a1b80/sources/magicvr/MagicTricks.cpp#L176)
in der Virtual Reality Anwendung MagicVR angewandt.

Es besteht auch die Möglichkeit, die Orientierung anders zu definieren oder
einzuschränken und frühzeitig (ohne Berechnung der Distanz zum Pattern) einen
Mismatch zu erkennen.
Die Vorgehensweise hängt vom Pattern und dem gewünschten Ergebnis ab.


## Zirkulare Trajektorien (z.B. ein Kreis mehrmals hintereinander am Stück) sollen erkannt werden können (Anzahl Umrundungen).
Wie bei geschlossener Trajektorie, nur dass Minima für jeden Punkt bestimmt
werden müssen. Dann ermittelt man, wie oft die Punktsequenz des Patterns
auftritt.

## Einbeziehung der Orientierung der Wand in die Definition einer Geste.
Wenn man z.B. eine Stichbewegung nach vorne definieren möchte, muss man die
Orientierung der Wand berücksichtigen, um die Stich-Trajektorie von einer
waagrecht gezeicheten Trajektorie (seitliche Bewegung der Wand) unterscheiden
zu können.

### Aufnahme der Orientierung in den Punkt
Bei einem 3D-Punkt kann die Orientierung der Wand durch einen 3D-Richtungsvektor
angegeben werden. Der Punkt und der Richtungsvektor können zu einem 6D-Punkt
kombiniert werden. Folglich bestehen Pattern und Eingabetrajektorie aus
6D-Punkten. Es kann der gleiche Vergleich (Modified Hausdorff) verwendet werden.
Eine Rotations-Transformation für Trajektorien mit 6D-Punkten muss eingeführt
werden. Weitere Anpassungen von Algorithmen könnten notwendig sein.

## Ähnlichkeit soll abhängig von der Zeit (z.B. Geschwindigkeit/Beschleunigung) definiert werden können
Wenn man die Zeit seit beginn der Aufzeichnung in die Koordinaten des Punkts
aufnimmt, kann man diese bei der Ähnlichkeitsdefinition berücksichtigen.
Das gleiche lässt sich mit der Beschleunigung machen.

**Bemerkung:** Ob es wirklich sinnvoll ist die Zeit bzw.
Geschwindigkeit/Beschleunigung in die Ähnlichkeitsdefinition aufzunehmen,
gilt es noch auszuprobieren.

## Erkennung einer Geste während sie gezeichnet wird (Vorhersage was der Nutzer versucht zu zeichnen)
Das Problem ist, dass bisher beim Trajektorien-Vergleich von einer vollständigen
Eingabe-Trajektorie ausgegangen wird, die mit einem bestimmten Pattern
verglichen wird.
Der aktuell aufgezeichnete Teil der Eingabe-Trajektorie stellt nur eine
Sub-Trajektorie der vollendeten Eingabe-Trajektorie dar.
Deswegen müsste die Sub-Trajektorie der Eingabe-Trajektorie mit einer
Sub-Trajektorie des Pattern verglichen werden.
Eine Sub-Trajektorie des Patterns kann andere Eigenschaften und Besonderheiten
aufweisen, die vom Trajektorien-Vergleich für das Pattern nicht berücksichtigt
werden.
Man bräuchte daher einen anderen Trajektorien-Vergleich für eine Sub-Trajektorie
des Patterns.

Sollte der gleiche Trajektorien-Vergleich verwendet werden können, kann die
Sub-Trajektorie des Patterns anhand der Länge der Eingabe-Trajektorie, der
Zeichen-Zeit, -Geschwindigkeit oder -Beschleunigung abgeschätzt werden.
Das Problem ist, dass weitere Unbekannte, wie z.B. die Größe der
Eingabe-Trajektorie, die Ungenauigkeit der Ähnlichkeitsbestimmung erhöhen.

Eigentlich widerspricht das Problem dem Ansatz von trajecmp.
Eine wesentliche Idee ist es, die Komplexität des Problems zu reduzieren,
in dem man Anfangs- und End-Punkt der Trajektorien gegeben bekommt.
Wenn eine Sub-Trajektorie des Patterns bestimmt werden muss, ist der End-Punkt
nicht gegeben.

Eine Alternative zum Schätzen einer Sub-Trajektorie des Patterns ist es,
mehrere Sub-Patterns (Sub-Trajektorien des Patterns mit 10%, 20%, ..., 90%, 100%
des Patterns) zu erstellen.
Für jedes Sub-Pattern kann wenn Nötig ein anderer Vergleich definiert werden.
Man vergleicht nun immer mit all diesen Sub-Patterns und weiß bei einem Match,
wie viel Prozent des Patterns gezeichnet worden ist.
Ein Nachteil dieser Variante ist eine relativ hohe Laufzeit, da viele Vergleiche
ausgeführt werden müssen.

Hinzu kommt, dass es strittig sein dürfte was der Nutzer versucht zu zeichnen.
Unzuverlässige Vorhersagen sind zu befürchten und für den Benutzer schlecht
nachzuvollziehen.

### Beispiel
Das Beispiel [sdl2-match-rectangle-while-drawing][sdl2-match-rectangle-while-drawing]
zeigt wie man anhand von Sub-Patterns erkennen kann, wie viel der Nutzer von
einem Rechteck gezeichnet hat.

[sdl2-match-rectangle-while-drawing]: example/sdl2-match-rectangle-while-drawing.md
[sdl2-rectangle-in-circle]: example/sdl2-rectangle-in-circle.md