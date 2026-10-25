# Zadanie 2

| Plik testowy | Typ | n | m | Wynik | Porządek topologiczny | Czas działania |
| :--- | :---: | :---: | :---: | :--- | :--- | :---: |
| `dag.txt` | D | 14 | 19 | Acykliczny (DAG) | 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 13 | 0.0005 ms |
| `cykl.txt` | D | 14 | 20 | Cykl skierowany | - | 0.0005 ms |
| `g2a-1.txt` | D | 16 | 33 | Acykliczny (DAG) | 1, 2, 5, 3, 6, 9, 4, 7, 10, 13, 8, 11, 14, 12, 15, 16 | 0.0134 ms |
| `g2b-1.txt` | D | 16 | 34 | Cykl skierowany | - | 0.0008 ms |
| `g2a-2.txt` | D | 100 | ~300 | Acykliczny (DAG) | 1, 2, 11, 3, 12, ..., 99, 100 | 0.0018 ms |
| `g2b-2.txt` | D | 100 | ~300 | Cykl skierowany | - | 0.0025 ms |
| `g2a-3.txt` | D | $10^3$ | ~$3 \cdot 10^3$ | Acykliczny (DAG) | $n > 200$ | 0.0285 ms |
| `g2b-3.txt` | D | $10^3$ | ~$3 \cdot 10^3$ | Cykl skierowany | - | 0.0255 ms |
| `g2a-4.txt` | D | $10^4$ | ~$3 \cdot 10^4$ | Acykliczny (DAG) | $n > 200$ | 0.1611 ms |
| `g2b-4.txt` | D | $10^4$ | ~$3 \cdot 10^4$ | Cykl skierowany | - | 0.1307 ms |
| `g2a-5.txt` | D | $10^5$ | ~$3 \cdot 10^5$ | Acykliczny (DAG) | $n > 200$ | 2.9618 ms |
| `g2b-5.txt` | D | $10^5$ | ~$3 \cdot 10^5$ | Cykl skierowany | - | 2.2169 ms |
| `g2a-6.txt` | D | $10^6$ | ~$3 \cdot 10^6$ | Acykliczny (DAG) | $n > 200$ | 12.0207 ms |
| `g2b-6.txt` | D | $10^6$ | ~$3 \cdot 10^6$ | Cykl skierowany | - | 8.9869 ms |
