#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
YELLOW="\e[33m"
BLUE="\e[34m"
RESET="\e[0m"

if [ -z "$1" ]; then
    echo "Użycie: $0 <katalog>"
    exit 1
fi

DIR="$1"

if [ ! -d "$DIR" ]; then
    echo "Błąd: katalog $DIR nie istnieje!"
    exit 2
fi

COUNTER_FILE=$(mktemp)
echo "0 0 0 0" > "$COUNTER_FILE"

LOGFILE="monitor.log"
echo "Start monitorowania katalogu: $DIR (Start: $(date +"%Y-%m-%d %H:%M:%S")) ===" >> "$LOGFILE"

get_counters() {
    read -r created deleted modified moved < "$COUNTER_FILE"
}

update_counter() {
    get_counters
    case $1 in
        created) ((created++)) ;;
        deleted) ((deleted++)) ;;
        modified) ((modified++)) ;;
        moved) ((moved++)) ;;
    esac
    echo "$created $deleted $modified $moved" > "$COUNTER_FILE"
}

print_stats() {
    get_counters
    echo -e "\n---------------------------------------------------" > /dev/tty
    echo -e "${BLUE}Aktualne statystyki (od startu):${RESET}" > /dev/tty
    echo "Dodano:          $created" > /dev/tty
    echo "Usunięto:        $deleted" > /dev/tty
    echo "Zmodyfikowano:   $modified" > /dev/tty
    echo "Zmieniono nazwę: $moved" > /dev/tty
    echo -e "---------------------------------------------------" > /dev/tty
}

cleanup() {
    echo -e "\n${YELLOW}Zatrzymywanie monitorowania${RESET}" > /dev/tty
    kill $INOTIFY_PID $STATS_PID 2>/dev/null
    rm -f "$COUNTER_FILE"
    echo "Koniec monitorowania (Koniec: $(date +"%Y-%m-%d %H:%M:%S"))" >> "$LOGFILE"
    exit 0
}

trap cleanup INT TERM EXIT

stats_background() {
    while true; do
        sleep 10
        print_stats
    done
}

stats_background &
STATS_PID=$!

echo -e "${GREEN}Monitorowanie katalogu '$DIR' rozpoczęte.${RESET}\n" > /dev/tty
echo -e "${YELLOW}Statystyki będą wyświetlane co 10 sekund.${RESET}" > /dev/tty
PIPE=$(mktemp -u)
mkfifo "$PIPE"

inotifywait -m -r -e create -e delete -e modify -e move \
    --exclude 'monitor\.log' \
    --format '%e %w%f' \
    "$DIR" 2>/dev/null > "$PIPE" &

INOTIFY_PID=$!

while IFS= read -r line; do
    event=$(echo "$line" | awk '{print $1}')
    file=$(echo "$line" | cut -d' ' -f2-)
    timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    case "$event" in
        CREATE|CREATE,ISDIR)
            echo -e "$timestamp ${GREEN}[DODANO]${RESET} $file" > /dev/tty
            echo "$timestamp CREATE $file" >> "$LOGFILE"
            update_counter "created"
            ;;
        DELETE|DELETE,ISDIR)
            echo -e "$timestamp ${RED}[USUNIĘTO]${RESET} $file" > /dev/tty
            echo "$timestamp DELETE $file" >> "$LOGFILE"
            update_counter "deleted"
            ;;
        MODIFY)
            echo -e "$timestamp ${YELLOW}[ZMODYFIKOWANO]${RESET} $file" > /dev/tty
            echo "$timestamp MODIFY $file" >> "$LOGFILE"
            update_counter "modified"
            ;;
        MOVED_TO|MOVED_TO,ISDIR)
            echo -e "$timestamp ${BLUE}[ZMIANA NAZWY/PRZENIESIENIE]${RESET} $file" > /dev/tty
            echo "$timestamp MOVE $file" >> "$LOGFILE"
            update_counter "moved"
            ;;
        *)
            #Ignorowwanie reszty
            ;;
    esac
done < "$PIPE"

rm -f "$PIPE" "$COUNTER_FILE"
