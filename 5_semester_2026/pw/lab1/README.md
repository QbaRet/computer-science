# Lista 1 — podróżnicy na torusie

| Katalog | Język | Treść |
|---|---|---|
| `zad1/` | Go | odpowiednik `travelers.adb` |
| `zad2/` | Ada | najwyżej 1 podróżnik na polu (task na pole + `terminate`), Timeout |
| `zad3/` | Go | Zadanie 2 w Go (kanał o pojemności 1 na pole + `select` z timerem) |
| `zad4/` | Ada | Zadanie 2 przy k = m = n = 15, start w (i, i), stały kierunek |
| `zad5/` | Go | Zadanie 4 w Go |
| `bash-tools/` | bash | `display-travel.bash` z repo kursu (odtwarzanie historii) |

## Instalacja (Ubuntu)

```bash
sudo apt install -y gnat golang-go subversion tmux
```

## Kompilacja i uruchomienie

```bash
# Ada (zad2, zad4)
gnatmake travelers.adb
./travelers            # zad2: opcjonalny argument = Nr_Of_Travelers (1..225), domyślnie 15

# Go (zad1, zad3, zad5)
go build -o travelers travelers.go
./travelers            # zad3: opcjonalny argument = Nr_Of_Travelers (1..225), domyślnie 15
```

## Odtwarzanie historii

```bash
./travelers > out1
bash ../bash-tools/display-travel.bash out1
```

`TO STEP?`: ENTER pokazuje następny krok, a liczba (np. `99999`) przewija do tego kroku albo do końca.
Wielka litera to podróżnik, który skończył normalnie. Mała litera to podróżnik, który uznał, że utknął w deadlocku.

## Decyzje projektowe

- Timeout = `3 * Max_Delay`. Jeśli docelowe pole nie zwolni się w tym czasie, podróżnik zakłada deadlock: zmienia symbol na małą literę, zapisuje ostatni ślad, wysyła historię do Printera i kończy pracę.
- Podróżnik, który skończył kroki (albo utknął), dalej zajmuje swoje pole.
- Ślad ruchu jest zapisywany po wejściu na nowe pole, ale **przed** zwolnieniem starego. Dzięki temu po posortowaniu według czasu historia nigdy nie pokazuje dwóch podróżników na jednym polu.
- Przy k > 26 symbole powtarzają się cyklicznie (A..Z).
- Format wyjścia jest taki sam jak w Adzie: `-1 k m n`, a potem `Time_Stamp Id X Y Symbol`.

## Scenariusz prezentacji (tmux)

```bash
tmux
# Imię Nazwisko NrIndeksu
svn export URL_katalogu_z_rozwiązaniami_listy
cd wyeksportowana_ścieżka

# --- Zadanie 1 --- (3 outputy)
cd zad1
go build -o travelers travelers.go
for i in 1 2 3; do ./travelers > out$i; done
shasum out1 out2 out3
svn import out1 URL_sprawozdania/zad1/out1 -m "zad1 out1"
svn import out2 URL_sprawozdania/zad1/out2 -m "zad1 out2"
svn import out3 URL_sprawozdania/zad1/out3 -m "zad1 out3"
bash ../bash-tools/display-travel.bash out1

# --- Zadanie 2 --- (różne zatłoczenie)
cd ../zad2
gnatmake travelers.adb
for k in 5 15 60 150; do ./travelers $k > out_k$k; done
shasum out_k*
for k in 5 15 60 150; do svn import out_k$k URL_sprawozdania/zad2/out_k$k -m "zad2 k=$k"; done
bash ../bash-tools/display-travel.bash out_k60

# --- Zadanie 3 --- (jak Zadanie 2, w Go)
cd ../zad3
go build -o travelers travelers.go
for k in 5 15 60 150; do ./travelers $k > out_k$k; done
shasum out_k*
for k in 5 15 60 150; do svn import out_k$k URL_sprawozdania/zad3/out_k$k -m "zad3 k=$k"; done

# --- Zadanie 4 --- (2-3 przebiegi, najlepiej z deadlockiem = małe litery)
cd ../zad4
gnatmake travelers.adb
for i in 1 2 3; do ./travelers > out$i; done
grep -c '[a-z]$' out1 out2 out3   # ile śladów "utknięcia"
shasum out1 out2 out3
for i in 1 2 3; do svn import out$i URL_sprawozdania/zad4/out$i -m "zad4 out$i"; done
bash ../bash-tools/display-travel.bash out1

# --- Zadanie 5 --- (jak Zadanie 4, w Go)
cd ../zad5
go build -o travelers travelers.go
for i in 1 2 3; do ./travelers > out$i; done
grep -c '[a-z]$' out1 out2 out3
shasum out1 out2 out3
for i in 1 2 3; do svn import out$i URL_sprawozdania/zad5/out$i -m "zad5 out$i"; done
```
