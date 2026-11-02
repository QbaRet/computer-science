# Lista 1 — podróżnicy na torusie

| Katalog | Język | Treść |
|---|---|---|
| `zad1/` | Go | odpowiednik `travelers.adb` |
| `zad2/` | Ada | co najwyżej 1 podróżnik na polu (wątek na pole + `terminate`), Timeout |
| `zad3/` | Go | Zadanie 2 w Go (kanał o pojemności 1 na pole + `select` z timerem) |
| `zad4/` | Ada | Zadanie 2: k = m = n = 15, start (i, i), stały kierunek |
| `zad5/` | Go | Zadanie 4 w Go |

## Decyzje projektowe

- Timeout = `3 * Max_Delay`. Sąsiad na docelowym polu ma kilka okazji, żeby się ruszyć. Jeśli tego nie zrobi, podróżnik zakłada deadlock: zmienia symbol na małą literę, zapisuje ostatni ślad, wysyła historię do Printera i kończy pracę.
- Podróżnik, który skończył kroki (albo utknął), dalej zajmuje swoje pole. Zgadza się to z tym, co pokazuje `display-travel.bash`, bo ostatni ślad zostaje na planszy.
- Zad. 2 i 3: `Nr_Of_Travelers` to stała, a jej wartość można podać jako pierwszy argument programu (domyślnie 15, zakres 1..225). Dzięki temu do testów zatłoczenia nie trzeba edytować kodu. Przy k > 26 symbole powtarzają się cyklicznie (A..Z).
- Format wyjścia jest taki sam jak w Adzie: `-1 k m n`, a potem `Time_Stamp Id X Y Symbol`.

## Kompilacja

```bash
# Ada (zad2, zad4)
gnatmake travelers.adb
# Go (zad1, zad3, zad5)
go build -o travelers travelers.go
```

## Scenariusz prezentacji (tmux)

```bash
# Imię Nazwisko NrIndeksu
svn export URL_katalogu_z_rozwiązaniami_listy
cd wyeksportowana_ścieżka

# --- Zadanie 1 ---
cd zad1
go build -o travelers travelers.go
for i in 1 2 3; do ./travelers > out$i; done
shasum out1 out2 out3
svn import out1 URL_sprawozdania/zad1/out1 -m "zad1 out1"
svn import out2 URL_sprawozdania/zad1/out2 -m "zad1 out2"
svn import out3 URL_sprawozdania/zad1/out3 -m "zad1 out3"
bash ../../PW/bash-tools/display-travel.bash out1   # ścieżka zależy od miejsca PW

# --- Zadanie 2 --- (różne zatłoczenie)
cd ../zad2
gnatmake travelers.adb
./travelers 5   > out_k5
./travelers 15  > out_k15
./travelers 60  > out_k60
./travelers 150 > out_k150
shasum out_k*
svn import out_k5 URL_sprawozdania/zad2/out_k5 -m "zad2 k=5"   # itd. dla pozostałych

# --- Zadanie 3 --- (analogicznie do Zadania 2)
cd ../zad3
go build -o travelers travelers.go
./travelers 5 > out_k5   # ... 15, 60, 150

# --- Zadanie 4 / 5 --- (2-3 przebiegi, najlepiej z deadlockiem = małe litery)
cd ../zad4
gnatmake travelers.adb
./travelers > out1; ./travelers > out2; ./travelers > out3
grep -c '[a-z]$' out1 out2 out3   # liczba podróżników, którzy utknęli
cd ../zad5
go build -o travelers travelers.go
./travelers > out1; ./travelers > out2; ./travelers > out3
```
