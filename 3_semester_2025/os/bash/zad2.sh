#!/bin/bash

cleanup() {
    stty echo icanon
    tput cnorm
    exit 0
}
trap cleanup INT TERM EXIT

tput civis
stty -echo -icanon time 0 min 0

dl_hist=() ul_hist=()
last_rx="" last_tx=""
total_rx=0 total_tx=0
declare -A prev_cpu

get_net_data() {
    rx_total=0 tx_total=0
    while read -r line; do
        [[ $line == *":"* ]] || continue
        iface=$(echo $line | cut -d: -f1 | tr -d ' ')
        [[ $iface == "lo" ]] && continue
        rx=$(echo $line | awk '{print $2}')
        tx=$(echo $line | awk '{print $10}')
        rx_total=$((rx_total + rx))
        tx_total=$((tx_total + tx))
    done < <(tail -n +3 /proc/net/dev)
    echo "$rx_total $tx_total"
}

update_network() {
    data=$(get_net_data)
    read current_rx current_tx <<< "$data"
    current_rx=${current_rx:-0}
    current_tx=${current_tx:-0}

    if [[ -n "$last_rx" && -n "$last_tx" ]]; then
        dl=$((current_rx - last_rx))
        ul=$((current_tx - last_tx))

        if [[ $dl -lt 0 ]]; then
            dl=0
        fi
        if [[ $ul -lt 0 ]]; then
            ul=0
        fi

        dl_hist+=("$dl")
        ul_hist+=("$ul")

        total_rx=$((total_rx + dl))
        total_tx=$((total_tx + ul))
        if [[ ${#dl_hist[@]} -gt 40 ]]; then
            dl_hist=("${dl_hist[@]:1}")
            ul_hist=("${ul_hist[@]:1}")
        fi
    fi

    last_rx=$current_rx
    last_tx=$current_tx
}

get_cpu_usage() {
    local cpu_id=$1
    local line
    line=$(grep "^cpu$cpu_id " /proc/stat 2>/dev/null)

    if [[ -z "$line" ]]; then
        echo "0"
        return
    fi
    read -r cpu user nice system idle iowait irq softirq steal _ <<< "$line"
    user=${user:-0}; nice=${nice:-0}; system=${system:-0}; idle=${idle:-0}
    iowait=${iowait:-0}; irq=${irq:-0}; softirq=${softirq:-0}; steal=${steal:-0}
    total=$((user + nice + system + idle + iowait + irq + softirq + steal))
    work=$((user + nice + system + irq + softirq + steal))
    if [[ -n "${prev_cpu[$cpu_id]}" ]]; then
        IFS=',' read -r prev_total prev_work <<< "${prev_cpu[$cpu_id]}"
        total_delta=$((total - prev_total))
        work_delta=$((work - prev_work))
        if [[ $total_delta -gt 0 ]]; then
            usage=$((100 * work_delta / total_delta))
            echo "$usage"
        else
            echo "0"
        fi
    else
        echo "0"
    fi
    prev_cpu[$cpu_id]="$total,$work"
}

format_speed() {
    local bytes=$1
    bytes=${bytes:-0}
    if [[ $bytes -ge 1048576 ]]; then
        value=$(echo "scale=2; $bytes/1048576" | bc -l 2>/dev/null || echo "0")
        printf "%s MB/s" "$value"
    elif [[ $bytes -ge 1024 ]]; then
        value=$(echo "scale=2; $bytes/1024" | bc -l 2>/dev/null || echo "0")
        printf "%s KB/s" "$value"
    else
        printf "%d B/s" "$bytes"
    fi
}

draw_chart() {
    local arr_name=$1
    local -a vals
    eval "vals=(\"\${${arr_name}[@]}\")"
    local max=1 height=8 width=40 row col v threshold
    if [[ ${#vals[@]} -eq 0 ]]; then
        for ((row=height; row>0; row--)); do
            printf "  %${width}s\n" " "
        done
        return
    fi

    max=1000
    for v in "${vals[@]: -$width}"; do
        v=${v:-0}
        if [[ $v -gt $max ]]; then
            max=$v
        fi
    done
    if [[ $max -eq 0 ]]; then
        max=1
    fi
    for ((row=height; row>0; row--)); do
        printf "  "
        threshold=$((max * row / height))
        for v in "${vals[@]: -$width}"; do
            v=${v:-0}
            if [[ $v -ge $threshold ]]; then
                if [[ $row -eq $height ]]; then
                    printf "█"
                elif [[ $v -ge $((max * (row + 1) / height)) ]]; then
                    printf "█"
                else
                    printf "▄"
                fi
            else
                printf " "
            fi
        done
        printf "\n"
    done
    printf "  "
    for ((i=0; i<width; i++)); do
        if [[ $((i % 10)) -eq 0 ]]; then
            printf "┴"
        else
            printf "─"
        fi
    done
    printf "\n"
}

display() {
    tput clear
    tput cup 0 0
    uptime_sec=$(awk '{print int($1)}' /proc/uptime)
    days=$((uptime_sec/86400))
    hours=$((uptime_sec%86400/3600))
    mins=$((uptime_sec%3600/60))
    secs=$((uptime_sec%60))
    read -r load1 load5 load15 _ < /proc/loadavg
    bat=$(cat /sys/class/power_supply/BAT0/capacity 2>/dev/null || echo "N/A")
    mem_total=$(awk '/MemTotal/ {print $2}' /proc/meminfo)
    mem_free=$(awk '/MemFree/ {print $2}' /proc/meminfo)
    mem_avail=$(awk '/MemAvailable/ {print $2}' /proc/meminfo)
    mem_used=$((mem_total - mem_avail))
    mem_total_mb=$((mem_total/1024))
    mem_used_mb=$((mem_used/1024))
    mem_pct=0
    if [[ $mem_total -gt 0 ]]; then
        mem_pct=$((mem_used*100/mem_total))
    fi
    echo ""
    printf "Uptime: %dd %02dh %02dm %02ds | Load: %s, %s, %s | Battery: %s%%\n" \
           "$days" "$hours" "$mins" "$secs" "$load1" "$load5" "$load15" "$bat"
    echo ""
    echo "CPU Usage"
    ncpus=$(nproc)
    for ((i=0; i<ncpus; i++)); do
        usage=$(get_cpu_usage "$i")
        freq=$(cat /sys/devices/system/cpu/cpu"$i"/cpufreq/scaling_cur_freq 2>/dev/null || echo "0")
        freq_mhz=$((freq/1000))
        printf "CPU%-2d %4d MHz: %3d%%\n" "$i" "$freq_mhz" "$usage"
    done
    echo ""
    echo "Memory"
    printf "Total: %d MB | Used: %d MB | Usage: %d%%\n" \
           "$mem_total_mb" "$mem_used_mb" "$mem_pct"
    echo ""
    echo "Network"
    if [[ ${#dl_hist[@]} -gt 1 ]]; then
        current_dl=${dl_hist[-1]}
        current_ul=${ul_hist[-1]}
        sum_dl=0
        sum_ul=0
        for v in "${dl_hist[@]}"; do
            sum_dl=$((sum_dl + v))
        done
        for v in "${ul_hist[@]}"; do
            sum_ul=$((sum_ul + v))
        done
        avg_dl=$((sum_dl / ${#dl_hist[@]}))
        avg_ul=$((sum_ul / ${#ul_hist[@]}))
        echo "Download: $(format_speed "$current_dl") (avg: $(format_speed "$avg_dl"))"
        echo "Upload:   $(format_speed "$current_ul") (avg: $(format_speed "$avg_ul"))"
        echo ""
        echo "Download history:"
        draw_chart "dl_hist"
        echo ""
        echo "Upload history:"
        draw_chart "ul_hist"
    else
        echo "Collecting network data (${#dl_hist[@]}/2 samples needed)"
        if [[ ${#dl_hist[@]} -eq 1 ]]; then
            echo "Current sample: DL=$(format_speed "${dl_hist[0]}"), UL=$(format_speed "${ul_hist[0]}")"
        fi
    fi
    echo ""
    echo "Press 'q' to quit"
}
ncpus=$(nproc)
for ((i=0; i<ncpus; i++)); do
    get_cpu_usage "$i" > /dev/null
done

initial_data=$(get_net_data)
read last_rx last_tx <<< "$initial_data"

while true; do
    update_network
    display
    if read -t 1 -n 1 key && [[ "$key" == "q" ]]; then
        break
    fi
done

cleanup
