
# Homework 29

### Aufgabe 1

    Our first question focuses on main-two-cvs-while.c (the working solution). 
    First, study the code. Do you think you have an understanding of 
    what should happen when you run the program?
    
- Ziel des Programms: Bounded Buffer (Producer/Consumer) korrekt synchronisieren, ohne Busy-Waiting.
- Shared State (unter Lock `m` geschützt): `buffer[]`, `fill_ptr`, `use_ptr`, `num_full`, `max`.
- Producer-Verhalten:
    - nimmt `m`
    - wenn Buffer voll (`num_full == max`) → wartet auf `empty` (`Cond_wait(&empty, &m)`)
    - sonst: `do_fill(...)` schreibt ins Buffer, erhöht `num_full`, bewegt `fill_ptr`
    - signalisiert danach `fill` (weckt Consumer)
    - gibt `m` frei
- Consumer-Verhalten:
    - nimmt `m`
    - wenn Buffer leer (`num_full == 0`) → wartet auf `fill` `Cond_wait(&fill, &m)`
    - sonst: `do_get()` liest aus Buffer, verringert num_full, bewegt `use_ptr`
    - signalisiert danach `empty` (weckt Producer)
    - gibt `m` frei
- Warum `while` statt `if`:
    - nach dem Aufwachen wird die Bedingung immer erneut geprüft (spurious wakeups / falsches Timing)
    - Was man beim Run erwartet:
    - Producer und Consumer laufen abwechselnd/überlappend, aber nur wenn der Zustand passt
    - keine Überfüllung (`num_full` geht nie über `max`)
    - kein Unterlauf (`num_full` geht nie unter `0`)
    - Threads schlafen, wenn sie nicht weiter können, und werden durch Signale passend geweckt
- Beendigung:
    - Am Ende wird ein END_OF_STREAM-Marker ins System gebracht (aus `main-common.c`)
    - Consumer laufen, bis sie END_OF_STREAM konsumieren, dann stoppen sie.

### Aufgabe 2

    Run with one producer and one consumer, and have the producer
    produce a few values. Start with a buffer (size 1), and then increase
    it. How does the behavior of the code change with larger buffers?
    (or does it?) What would you predict num full to be with different
    buffer sizes (e.g., -m 10) and different numbers of produced items
    (e.g., -l 100), when you change the consumer sleep string from
    default (no sleep) to -C 0,0,0,0,0,0,1?

```sh
./main-two-cvs-while -p 1 -c 1 -l 2 -m 4 -v
```

- Buffergröße 1 (`-m 1`):
    - `num_full` kann nur 0 oder 1 sein
    - pendelt ständig zwischen 0 ↔ 1
- Größerer Buffer (`-m 10`) + langsamer Consumer (`-C ...1`):
    - Producer füllt schnell auf
    - `num_full` steigt bis max (10)
    - danach pendelt es fast nur zwischen 9 und 10
- Ohne Consumer-Sleep (Default):
    - Consumer hält Schritt
    - `num_full` bleibt meist niedrig (0 oder 1), auch bei großem Buffer
- Mit -C 0,0,0,0,0,0,1 (Consumer langsam):
    - Producer füllt schnell bis voll
    - bei -m 10, -l 100:
        - num_full wird sehr schnell 10
        - danach pendelt es fast nur zwischen 9 und 10
- generell:
    - wenn l >= m → num_full ≈ m (meist voll)
    - wenn l < m → num_full ≈ l (nie voll, weil nicht genug produziert)

### Aufgabe 4

    Let’s look at some timings. How long do you think the follow-
    ing execution, with one producer, three consumers, a single-entry
    shared buffer, and each consumer pausing at point c3 for a sec-
    ond, will take? ./main-two-cvs-while -p 1 -c 3 -m 1 -C
    0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v
    -t

- Pro Item wird’s ~1 Sekunde dauern, weil:
    - alle Consumer schlafen bei c3 (direkt nach Cond_wait(&fill, &m))
    - dabei halten sie den Mutex m → blockieren Producer + andere Consumer
- Wie viele “Items” müssen durch?
    - Producer macht -l 10 → 10 Daten-Items
    - plus 1× END_OF_STREAM (damit Consumer stoppen)
- Vorhersage Laufzeit:
    - ca. 11 Sekunden (10 Items + EOS)
    - plus kleiner Overhead → ~11–12 s in echt

### Aufgabe 5

    Now change the size of the shared buffer to 3 (-m 3). Will this make
    any difference in the total time?

- Nein, praktisch keine Verbesserung.
- Grund:
    - der Bottleneck bleibt: Consumer schlafen bei c3 mit gehaltenem Mutex m
    - dadurch kann der Producer währenddessen nicht weiter füllen, auch wenn der Buffer größer ist
- Erwartung:
    - Gesamtzeit bleibt ungefähr gleich wie vorher (also wieder ~11–12 s)

### Aufgabe 6

    Now change the location of the sleep to c6 (this models a con-
    sumer taking something off the queue and then doing something
    with it), again using a single-entry buffer. What time do you pre-
    dict in this case? ./main-two-cvs-while -p 1 -c 3 -m 1
    -C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10
    -v -t

- Vorhersage: ca. 4 Sekunden (plus kleiner Overhead)
- Warum (kurz):
    - Sleep ist jetzt bei c6 → nach Mutex_unlock(&m)
    - heißt: Consumer blockieren den Mutex nicht beim Schlafen
    - bei -m 1 kann der Producer aber trotzdem nur 1 Item voraus produzieren
    - mit 3 Consumern werden die 10 Items aufgeteilt → im Schnitt ~10/3 ≈ 3.3 Sleeps à 1s pro Consumer
    - die Sleeps laufen parallel, daher dominiert der langsamste Consumer: ~4 Sekunden

### Aufgabe 7

    Finally, change the buffer size to 3 again (-m 3). What time do you
    predict now?

- wieder ca. 4-5 
    - Größerer Buffer hilft hier kaum, weil nicht der Producer der Bottleneck ist, sondern die 1s “Work” pro konsumiertem Item.

### Aufgabe 8 

    Now let’s look at main-one-cv-while.c. Can you configure
    a sleep string, assuming a single producer, one consumer, and a
    buffer of size 1, to cause a problem with this code?

- Nein (bei 1 Producer, 1 Consumer, Buffergröße 1) kannst du mit Sleep-Strings kein Correctness-Problem erzwingen.
- Grund:
    - beide Seiten benutzen while-Checks
    - es gibt nur einen Producer und einen Consumer → signal() weckt nie “den falschen Typ” (weil es keinen zweiten Typ wartet außer dem einen)
    - Ergebnis: höchstens anders getaktet / langsamer, aber kein Deadlock / kein Fehlerzustand

### AUfgabe 9

    Now change the number of consumers to two. Can you construct
    sleep strings for the producer and the consumers so as to cause a
    problem in the code?

Ja.
- Mit 1 Producer, 2 Consumern, -m 1 kannst du mit Sleep-Strings ein Problem in main-one-cv-while.c zeigen.
- Problem = Unfairness/Starvation:
    - weil nur eine Condition Variable genutzt wird
    - Cond_signal(&cv) weckt beliebig einen Wartenden
    - dadurch kann immer derselbe Consumer die Items bekommen, der andere 0
- Beleg aus dem Run:
```sh
./main-one-cv-while -p 1 -c 2 -m 1 -l 5 \
-C 0,0,1,0,0,0,0:0,0,1,0,0,0,0 \
-P 0 -v
```
- C0 -> 5, C1 -> 0 (ein Consumer bekommt alles)
Wichtig: Das Programm ist dabei nicht zwingend “falsch” im Sinne von Crash, aber nicht fair und zeigt die Schwäche von one-cv bei mehreren Consumern.

### Aufgabe 10

    Now examine main-two-cvs-if.c. Can you cause a problem to
    happen in this code? Again consider the case where there is only
    one consumer, and then the case where there is more than one.

- Ja, man kann Probleme auslösen.
- Grund (kurz): Es wird `if` statt `while` benutzt → nach `Cond_wait()` wird die Bedingung nicht erneut geprüft.
    #### Fall 1: nur 1 Consumer
- Problem möglich: Consumer wacht auf, obwohl `num_full == 0` (spurious wakeup / Timing), läuft weiter und macht `do_get()`
- Folge: `ensure(tmp != EMPTY, ...)` kann triggern → Fehler/Abort
    #### Fall 2: mehr als 1 Consumer
- Problem noch leichter: ein Consumer wird geweckt, aber ein anderer Consumer “klaut” das Item zuerst
- Der geweckte Consumer läuft weiter (weil nur `if`) obwohl der Buffer wieder leer ist
- Folge: wieder `do_get()` auf leerem Buffer → ensure-Fehler/Abort
- Zusätzlich möglich: Producer-Seite analog (Producer macht `do_fill()` obwohl Buffer wieder voll ist) → ensure “fill non-empty”
Kernaussage:
- Mit 1 Consumer: Problem möglich, aber seltener.
- Mit >1 Consumer: Problem deutlich wahrscheinlicher (klassisches “wakeup but condition no longer true”).

### Aufgabe 11

    Finally, examine main-two-cvs-while-extra-unlock.c. What
    problem arises when you release the lock before doing a put or a
    get? Can you reliably cause such a problem to happen, given the
    sleep strings? What bad thing can happen?


- Problem beim Unlock vor `do_fill()` / `do_get()`:
    - du verlierst die Garantie: “Lock schützt Shared State”
    - Check (`num_full == max` / `num_full == 0`) und Aktion (fill/get) sind nicht mehr atomar
    - andere Threads können dazwischen laufen → Race Condition
- Was kann konkret schiefgehen (bad thing):
    - Producer: zwei Producer können beide “sehen: nicht voll” und dann beide `do_fill()` ausführen
        - `ensure(buffer[fill_ptr] == EMPTY)` kann fehlschlagen (füllt in nicht-leeren Slot)
        - `num_full` kann falsch werden (z.B. > `max`)
    - Consumer: zwei Consumer können beide “sehen: nicht leer” und dann beide `do_get()` ausführen
        - `ensure(tmp != EMPTY)` kann fehlschlagen (holt aus leerem Slot)
        - `num_full` kann falsch werden (z.B. < 0)
- Kann man das mit Sleep-Strings zuverlässig auslösen?
    - Nicht 100% zuverlässig, weil Thread-Scheduling nicht deterministisch ist.
    - Aber: Mit Sleep an genau den Stellen zwischen Unlock und do_fill/do_get kannst du die Wahrscheinlichkeit stark erhöhen.
- Kurzfassung:
- Unlock vor put/get ⇒ TOCTOU-Bug (check-then-act ohne Schutz)
- mögliches Resultat ⇒ Assertion/ensure-Crash oder korruptes Buffer- und `num_full`-State