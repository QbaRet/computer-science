# Lista 2 — dzicy lokatorzy i pułapki

| Katalog | Język | Treść |
|---|---|---|
| `zad1/` | Ada | Lista 1 Zad. 2 + dzicy lokatorzy (task na każde pole) |
| `zad2/` | Go | Zadanie 1 w Go (pole = „stateful goroutine”) |
| `zad3/` | Ada | Zadanie 1 + pułapki `#` |
| `zad4/` | Go | Zadanie 3 w Go |
| `bash-tools/` | bash | `display-travel.bash` z repo kursu |

Programy powstały na bazie Listy 1 (`lab1/zad2`, `lab1/zad3`). Zostały te same stałe, ten sam format wyjścia i ten sam timeout na deadlock.

## Kompilacja i uruchomienie

```bash
# Ada (zad1, zad3)
gnatmake travelers.adb
./travelers [k] > out1        # k = liczba podróżników, domyślnie 15

# Go (zad2, zad4)
go build -o travelers travelers.go
./travelers [k] > out1

bash ../bash-tools/display-travel.bash out1
```

## Symbole na planszy

| Symbol | Znaczenie |
|---|---|
| `A`..`Z` | podróżnik |
| `a`..`z` | podróżnik, który utknął (timeout) albo wpadł w pułapkę |
| `0`..`9` | dziki lokator |
| `!` | dziki lokator złapany w pułapkę (zad3/4) |
| `#` | pułapka (zad3/4) |

Numery id w nagłówku `-1 N 15 15`: najpierw podróżnicy `0..k-1`, potem 10 lokatorów, potem 10 pułapek.
Gdy coś znika z planszy (lokator albo ofiara pułapki), zostawia ślad na pozycji `(15, 15)`, czyli poza planszą.

## Jak to działa

### Pole to wątek, który wie, kto na nim stoi
Każde z 225 pól ma własny wątek (Ada: `task`, Go: gorutyna). Stan pola: nikt / podróżnik / lokator.
Inne wątki nie zmieniają tego stanu same, tylko **wysyłają prośby**:

| Prośba | Kto wysyła | Co robi pole |
|---|---|---|
| `Enter(From)` | podróżnik | wpuszcza go; jeśli stoi tu lokator, najpierw próbuje go przenieść |
| `Leave` | podróżnik | zwalnia pole |
| `Squatter_Arrive` | „pojawiacz” albo sąsiednie pole | przyjmuje lokatora (tylko gdy pole jest wolne) |
| `Get_Traces` | Printer (zad3/4) | oddaje ślady pułapki |

### Dziki lokator
- Lokator to **rekord z danymi** (id, cyfra, czas śmierci, ślady), a nie osobny wątek. Rekord „mieszka” w wątku pola, na którym stoi lokator.
- Pojawia się dzięki małemu wątkowi-„pojawiaczowi”: ten czeka losowy czas (0–2 s) i stawia lokatora na losowym wolnym polu.
- Gdy podróżnik wchodzi na pole lokatora, pole próbuje oddać lokatora sąsiadowi (innemu niż pole, z którego przychodzi podróżnik). Pyta po kolei sąsiadów, **nie czekając**: Ada `select ... else`, Go `select ... default`.
  - Jeśli któryś sąsiad przyjmie lokatora, podróżnik wchodzi.
  - Jeśli żaden nie przyjmie, podróżnik dostaje `Blocked` i w następnym kroku losuje nowy kierunek.
- Pole pilnuje czasu życia lokatora (Ada `delay until`, Go `time.After`). Gdy czas minie, pole zapisuje ślad poza planszą i wysyła historię lokatora do Printera.

**Dlaczego lokator nie jest wątkiem?** Gdyby był, pole musiałoby go powiadamiać o przeprowadzce, a on w tej samej chwili mógłby chcieć zniknąć i czekać na pole. Wtedy pole i lokator czekaliby na siebie nawzajem, czyli doszłoby do deadlocku. Gdy rekord lokatora przechodzi z pola na pole, w każdej chwili ma dokładnie jednego właściciela i problem znika.

**Dlaczego pola pytają sąsiadów bez czekania?** Gdyby pole A czekało na pole B, a B jednocześnie na A, oba by stanęły. Pytanie bez czekania nie może się zablokować.

**Brak sztucznych zależności:** każde pole ma własny wątek i rozmawia najwyżej ze swoimi 4 sąsiadami, więc ruchy w odległych częściach planszy w ogóle na siebie nie wpływają.

### Pułapki (zad3, zad4)
- Program główny losuje 10 pól na pułapki **przed startem** podróżników. Każda pułapka zapisuje pierwszy ślad `#`.
- Ani podróżnicy, ani lokatorzy nie startują na pułapce.
- **Podróżnik w pułapce:** mała litera, ślad, sen `Trap_Sleep` (pole zablokowane), ślad poza planszą, `Leave`. Pole-pułapka zapisuje wtedy `#`.
- **Lokator w pułapce:** może tam trafić tylko przy przeprowadzce. Dostaje `!`, ślad i krótki sen (pole zablokowane). Potem znika, a pułapka zapisuje `#`.
- **Printer** najpierw zbiera raporty wszystkich podróżników i lokatorów, a dopiero potem prosi każdą pułapkę o jej ślady (`Get_Traces`).

### Ada a Go
| | Ada (zad1, zad3) | Go (zad2, zad4) |
|---|---|---|
| Pole | `task type Field_Task_Type` | gorutyna `(*Field).run` |
| Prośba | `entry` / `accept` | kanał z prośbą (`enterRequest` z kanałem `reply`) |
| Warunek „tylko gdy wolne” | `when Occupant = None =>` | kanał ustawiony na `nil` = gałąź `select` wyłączona |
| Pytanie bez czekania | `select ... else` | `select ... default` |
| Czas życia lokatora | `delay until Sq.Death_Time` | `case <-death` (`time.After`) |
| Koniec wątków pól | `terminate` | niepotrzebny: `main` kończy program po `<-done` |

W Adzie `terminate` i `delay` nie mogą być w jednym `select`. Dlatego pole ma dwa `select`: jeden, gdy stoi na nim lokator (z `delay until`), i drugi w pozostałych przypadkach (z `terminate`).

## Scenariusz prezentacji (tmux)

```bash
tmux
# Imię Nazwisko NrIndeksu
svn export URL_katalogu_z_rozwiązaniami_listy
cd wyeksportowana_ścieżka

# --- Zadanie 1 (Ada) i 2 (Go): różne zatłoczenie ---
cd zad1
gnatmake travelers.adb
for k in 5 15 60; do ./travelers $k > out_k$k; done
shasum out_k*
for k in 5 15 60; do svn import out_k$k URL_sprawozdania/zad1/out_k$k -m "zad1 k=$k"; done
bash ../bash-tools/display-travel.bash out_k15

cd ../zad2
go build -o travelers travelers.go
for k in 5 15 60; do ./travelers $k > out_k$k; done
shasum out_k*
for k in 5 15 60; do svn import out_k$k URL_sprawozdania/zad2/out_k$k -m "zad2 k=$k"; done

# --- Zadanie 3 (Ada) i 4 (Go): pułapki ---
cd ../zad3
gnatmake travelers.adb
for k in 5 15 60; do ./travelers $k > out_k$k; done
grep -c '!$' out_k*          # ile razy lokator wpadł w pułapkę
shasum out_k*
for k in 5 15 60; do svn import out_k$k URL_sprawozdania/zad3/out_k$k -m "zad3 k=$k"; done

cd ../zad4
go build -o travelers travelers.go
for k in 5 15 60; do ./travelers $k > out_k$k; done
shasum out_k*
for k in 5 15 60; do svn import out_k$k URL_sprawozdania/zad4/out_k$k -m "zad4 k=$k"; done
```

Lokator wpada w pułapkę (`!`) rzadko, bo trafia tam tylko przy przeprowadzce. Jeśli chcesz to pokazać, uruchom program kilka razy z większym `k` i wybierz output, w którym `grep -c '!$'` daje więcej niż 0.
