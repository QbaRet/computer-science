#!/bin/bash
# Nagrywa prezentację Listy 1 w tmux i zapisuje całą historię panelu do pliku *.tmux.
#
# Kolejność zgodna z wymaganiami listy:
#   1. komentarz "# imię nazwisko numer_indeksu"
#   2. svn export URL_katalogu_z_rozwiązaniami_listy
#   3. cd wyeksportowana_ścieżka
#   4. w podkatalogu każdego zadania: kompilacja, uruchomienie, shasum, svn import
#
# Użycie:  bash prezentacja.sh            (najpierw uzupełnij zmienne poniżej)
# Wynik:   $WYNIK (domyślnie ~/lab1.tmux), sesja tmux "$SESJA" zostaje otwarta do podglądu:
#          tmux attach -t lab1

# ======== UZUPEŁNIJ ========
IMIE_NAZWISKO="Imie Nazwisko"
INDEKS="000000"
URL_ROZWIAZAN="https://repo.cs.pwr.edu.pl/XXXXXX/PW/lab1"      # katalog z rozwiązaniami (svn export)
URL_SPRAWOZDANIA="https://repo.cs.pwr.edu.pl/XXXXXX/PW/lab1-sprawozdanie"  # tu trafią outputy (svn import)
# ===========================

KATALOG_ROBOCZY="${KATALOG_ROBOCZY:-$HOME/pw-prezentacja}"   # tu wykonany zostanie svn export
EXPORT="lab1"                                                # nazwa wyeksportowanego katalogu
WYNIK="${WYNIK:-$HOME/lab1.tmux}"
SESJA="lab1"

K_LIST="5 15 60 150"   # zatłoczenie planszy w zad2/zad3 (Nr_Of_Travelers)

set -e
command -v tmux >/dev/null || { echo "brak tmux"; exit 1; }
command -v svn  >/dev/null || { echo "brak svn"; exit 1; }
command -v go   >/dev/null || { echo "brak go"; exit 1; }
command -v gnatmake >/dev/null || { echo "brak gnatmake (sudo apt install gnat)"; exit 1; }

mkdir -p "$KATALOG_ROBOCZY"
if [ -e "$KATALOG_ROBOCZY/$EXPORT" ]; then
  echo "$KATALOG_ROBOCZY/$EXPORT już istnieje - usuń go, svn export wymaga pustego miejsca"; exit 1
fi
tmux has-session -t "$SESJA" 2>/dev/null && { echo "sesja tmux '$SESJA' już istnieje (tmux kill-session -t $SESJA)"; exit 1; }

tmux new-session -d -s "$SESJA" -x 200 -y 50 -c "$KATALOG_ROBOCZY"
tmux set-option -t "$SESJA" history-limit 1000000 >/dev/null
KANAL="$SESJA-prompt-$$"
sleep 1
# po każdym poleceniu powłoka w panelu daje sygnał na kanale $KANAL (niewidoczne w prezentacji)
tmux send-keys -t "$SESJA" " export PROMPT_COMMAND='tmux wait-for -S $KANAL'; clear" Enter
tmux wait-for "$KANAL"
tmux clear-history -t "$SESJA"

# wpisuje polecenie do panelu i czeka na kolejny prompt (= koniec polecenia)
run() {
  tmux send-keys -t "$SESJA" -l "$1"
  tmux send-keys -t "$SESJA" Enter
  tmux wait-for "$KANAL"
}

# wyświetla końcowy stan planszy (ostatni krok) skryptem display-travel.bash
plansza() {
  run "echo 999999 | bash ../bash-tools/display-travel.bash $1 | tail -n 16 | sed 's/\x1b\[[0-9;]*[A-Za-z]//g'   # ostatni krok"
}

run "# $IMIE_NAZWISKO $INDEKS"
run "svn export $URL_ROZWIAZAN $EXPORT"
run "cd $EXPORT"

# ---- Zadanie 1 (Go): trzy outputy ----
run "# ===== Zadanie 1 ====="
run "cd zad1"
run "go build -o travelers travelers.go"
for i in 1 2 3; do
  run "./travelers > out$i"
  run "shasum out$i"
  run "svn import out$i $URL_SPRAWOZDANIA/zad1/out$i -m 'zad1 out$i'"
done
plansza out1

# ---- Zadanie 2 (Ada): różne zatłoczenie ----
run "# ===== Zadanie 2 ====="
run "cd ../zad2"
run "gnatmake travelers.adb"
for k in $K_LIST; do
  run "./travelers $k > out_k$k   # Nr_Of_Travelers = $k"
  run "shasum out_k$k"
  run "svn import out_k$k $URL_SPRAWOZDANIA/zad2/out_k$k -m 'zad2 k=$k'"
done
plansza out_k60

# ---- Zadanie 3 (Go): Zadanie 2 w Go ----
run "# ===== Zadanie 3 ====="
run "cd ../zad3"
run "go build -o travelers travelers.go"
for k in $K_LIST; do
  run "./travelers $k > out_k$k   # Nr_Of_Travelers = $k"
  run "shasum out_k$k"
  run "svn import out_k$k $URL_SPRAWOZDANIA/zad3/out_k$k -m 'zad3 k=$k'"
done
plansza out_k60

# ---- Zadanie 4 (Ada): k = m = n = 15, start (i,i), stały kierunek ----
run "# ===== Zadanie 4 ====="
run "cd ../zad4"
run "gnatmake travelers.adb"
for i in 1 2 3; do
  run "./travelers > out$i"
  run "shasum out$i"
  run "svn import out$i $URL_SPRAWOZDANIA/zad4/out$i -m 'zad4 out$i'"
done
run "grep -c '[a-z]\$' out1 out2 out3   # liczba podróżników, którzy wykryli deadlock (mała litera)"
plansza out1

# ---- Zadanie 5 (Go): Zadanie 4 w Go ----
run "# ===== Zadanie 5 ====="
run "cd ../zad5"
run "go build -o travelers travelers.go"
for i in 1 2 3; do
  run "./travelers > out$i"
  run "shasum out$i"
  run "svn import out$i $URL_SPRAWOZDANIA/zad5/out$i -m 'zad5 out$i'"
done
run "grep -c '[a-z]\$' out1 out2 out3   # liczba podróżników, którzy wykryli deadlock (mała litera)"
plansza out1

run "# koniec prezentacji"

# zapis całej historii panelu (-J skleja zawinięte wiersze)
tmux capture-pane -t "$SESJA" -p -J -S - -E - | sed -e :a -e '/^\n*$/{$d;N;ba' -e '}' > "$WYNIK"
echo "Zapisano: $WYNIK  ($(wc -l < "$WYNIK") wierszy)"
echo "Podgląd sesji: tmux attach -t $SESJA"
