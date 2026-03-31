#!/usr/bin/env bash
tput civis
stty -echo -icanon time 0 min 0

cleanup() {
    tput cnorm
    stty sane
    printf "\n"
    exit 0
}
trap cleanup INT TERM

soft_clear_screen() {
    local term_lines=$(tput lines)
    local term_cols=$(tput cols)
    for ((r=0; r<term_lines; r++)); do
        tput cup "$r" 0
        printf "%${term_cols}s" ""
    done
    tput cup 0 0
}

get_process_info() {
    local pid="$1"
    local status="/proc/$pid/status"
    local stat="/proc/$pid/stat"
    [ ! -r "$status" ] && return 1

    local comm="" st="" ppid="" rss=""
    local tty_display="?" pgid="0" sid="0"

    while IFS=":" read -r key val; do
        case "$key" in
            Name)   comm="${val# }" ;;
            State)  st="${val# }" ;;
            PPid)   ppid="${val# }" ;;
            VmRSS)  rss="${val# }" ;;
        esac
    done < "$status"

    if [ -r "$stat" ]; then
        local stat_line=$(< "$stat")
        local front=${stat_line%%)*}
        local rest=${stat_line#*) }
        set -- $rest
        st="$1"
        ppid="$2"
        pgid="$3"
        sid="$4"
        tty_nr="$5"

        if [ "$tty_nr" -eq 0 ] 2>/dev/null; then
            tty_display="?"
        else
            local tty_major=$((tty_nr / 256))
            local tty_minor=$((tty_nr % 256))
            case "$tty_major" in
                136) tty_display="pts/$tty_minor" ;;
                4)   tty_display="tty$tty_minor" ;;
                *)   tty_display="$tty_nr" ;;
            esac
        fi
    fi

    local rss_num="${rss%% kB*}"; rss_num="${rss_num## }"
    [ -z "$rss_num" ] && rss_num=0

    local fd_dir="/proc/$pid/fd"
    local open_fd=0
    [ -d "$fd_dir" ] && open_fd=$(ls "$fd_dir" 2>/dev/null | wc -l)

    printf "%10s %10s %-30s %-4s %-10s %10s %10s %10s %4s\n" \
        "$ppid" "$pid" "$comm" "$st" "$tty_display" "$rss_num" "$pgid" "$sid" "$open_fd"
}

print_header() {
    printf "%10s %10s %-30s %-4s %-10s %10s %10s %10s %4s\n" \
        "PPID" "PID" "COMMAND" "STAT" "TTY" "RSS" "PGID" "SID" "FD"
}

scroll=0
selected=0
handle_input() {
    key=$(dd bs=1 count=1 2>/dev/null) || return
    case "$key" in
        q) cleanup ;;
        A|k) [ $selected -gt 0 ] && selected=$((selected - 1)) ;;
        B|j) [ $selected -lt $((total_count - 1)) ] && selected=$((selected + 1)) ;;
        r)
            pid="${pids[$selected]}"
            read -p "New nice for PID $pid: " nice
            renice "$nice" -p "$pid" >/dev/null 2>&1
            ;;
        k)
            pid="${pids[$selected]}"
            kill -TERM "$pid" 2>/dev/null
            ;;
        s)
            pid="${pids[$selected]}"
            kill -STOP "$pid" 2>/dev/null
            ;;
        c)
            pid="${pids[$selected]}"
            kill -CONT "$pid" 2>/dev/null
            ;;
    esac
}

while true; do
    soft_clear_screen
    print_header

    mapfile -t pids < <(ls /proc | grep -E '^[0-9]+' | sort -n)
    total_count=${#pids[@]}
    term_lines=$(tput lines)
    visible=$((term_lines - 4))

    [ $scroll -gt $((total_count - visible)) ] && scroll=$((total_count - visible))
    [ $scroll -lt 0 ] && scroll=0

    row=1
    for ((i=scroll; i<scroll+visible && i<total_count; i++)); do
        pid="${pids[$i]}"
        info=$(get_process_info "$pid")
        tput cup "$row" 0
        if [ $i -eq $selected ]; then
            tput rev
            printf "%s" "$info"
            tput sgr0
        else
            printf "%s" "$info"
        fi
        row=$((row + 1))
    done

    tput cup $((term_lines - 2)) 0
    printf "Processes: %d | Showing: %d-%d | ↑/k up, ↓/j down, r renice, k kill, s stop, c cont, q quit" \
        "$total_count" "$scroll" "$((scroll + visible))"

    handle_input
    sleep 1
done
