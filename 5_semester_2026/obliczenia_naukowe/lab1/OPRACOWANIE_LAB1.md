# Kompendium Wiedzy: Laboratorium 1 – Arytmetyka IEEE 754 i Analiza Błędów
**Autor opracowania:** Jakub Retmańczyk  
**Przeznaczenie:** Przygotowanie do obrony laboratorium / wejściówki (45 minut nauki)

---

## SPIS TREŚCI
1. [Model Myślowy: Jak komputer widzi liczby rzeczywiste (IEEE 754)](#1-model-myślowy-jak-komputer-widzi-liczby-rzeczywiste)
2. [Zadanie 1: Parametry maszynowe (macheps, eta, MAX)](#2-zadanie-1-parametry-maszynowe)
3. [Zadanie 2: Rozmieszczenie liczb i gęstość siatki](#3-zadanie-2-rozmieszczenie-liczb-i-gęstość-siatki)
4. [Zadanie 3: Zagadka $x \cdot (1/x) \neq 1$](#4-zadanie-3-zagadka-x-cdot-1x-neq-1)
5. [Zadanie 4: Iloczyn skalarny i redukcja cyfr znaczących](#5-zadanie-4-iloczyn-skalarny-i-redukcja-cyfr-znaczących)
6. [Zadanie 5: Stabilność funkcji $f(x)$ vs $g(x)$](#6-zadanie-5-stabilność-funkcji-fx-vs-gx)
7. [Zadanie 6: Różniczkowanie numeryczne i anatomia wykresu V](#7-zadanie-6-różniczkowanie-numeryczne-i-anatomia-wykresu-v)
8. [Ściąga pytań na obronę: O co zapyta prowadzący?](#8-ściąga-pytań-na-obronę)

---

## 1. Model Myślowy: Jak komputer widzi liczby rzeczywiste

Na osi liczb rzeczywistych $\mathbb{R}$ punktów jest nieskończenie wiele i ciągle. Pamięć komputera jest skończona (16, 32 lub 64 bity). Komputer nie potrafi więc zapisać liczb rzeczywistych — potrafi zapisać jedynie **wybrane punkty kratowe (liczby maszynowe)**. Wszystko pomiędzy nimi musi zostać **zaokrąglone** do najbliższego punktu.

Każda liczba maszynowa w standardzie IEEE 754 składa się z 3 pól:
$$\text{wartość} = (-1)^s \cdot 2^{e - B} \cdot (1 + m)$$

1. **Znak ($s$):** 1 bit ($0$ = dodatnia, $1$ = ujemna).
2. **Cecha / Wykładnik ($e$):** Zapisana z obciążeniem (*bias* $B = 2^{E-1}-1$).
   - Przesunięcie pozwala zapisać wykładniki ujemne i dodatnie jako liczby bez znaku.
3. **Mantysa ($m$):** Część ułamkowa o długości $t$ bitów.
   - **Ukryta jedynka (*hidden bit*):** Dla liczb znormalizowanych mantysa zawsze zaczyna się od `1.xxxx...`. Po co marnować bit w pamięci na jedynkę, skoro zawsze tam jest? Zapisujemy więc w pamięci tylko część po przecinku!

### Trzy formaty, które musisz znać na pamięć:
| Typ w Julii | Standard C / IEEE | Bity: Znak + Cecha + Mantysa ($t$) | Liczba bitów mantysy $t$ | Dokładność dziesiętna |
| :--- | :--- | :---: | :---: | :---: |
| `Float16` | half precision | $1 + 5 + 10$ | $t = 10$ | ok. 3-4 cyfry |
| `Float32` | single precision (`float`) | $1 + 8 + 23$ | $t = 23$ | ok. 7 cyfr |
| `Float64` | double precision (`double`)| $1 + 11 + 52$ | $t = 52$ | ok. 15-17 cyfr |

---

## 2. Zadanie 1: Parametry Maszynowe

W tym zadaniu pętlami wyznaczamy 3 fundamentalne stałe arytmetyki dla każdego formatu.

### A. Epsilon Maszynowy ($\text{macheps}$)
* **Definicja:** Najmniejsza liczba $\text{macheps} > 0$ taka, że $\text{fl}(1.0 + \text{macheps}) > 1.0$.
  Jest to odległość od $1.0$ do **następnej** możliwej do zapisania liczby maszynowej.
* **Wzór analityczny:** $\text{macheps} = 2^{-t}$.
  - `Float16`: $2^{-10} \approx 9.77 \cdot 10^{-4}$
  - `Float32`: $2^{-23} \approx 1.19 \cdot 10^{-7}$
  - `Float64`: $2^{-52} \approx 2.22 \cdot 10^{-16}$
* **Jak działa kod?** Startujemy od `m = 1.0` i sprawdzamy: `while 1.0 + m/2.0 > 1.0; m /= 2.0; end`.
  Gdy dodamy $m/2 = 2^{-t-1}$, komputer zaokrągla wynik w dół do $1.0$ (bo to dokładnie połowa odległości do kolejnego punktu, tzw. reguła zaokrąglenia do parzystej). Pętla zatrzymuje się na $m = 2^{-t}$.
* **Związek z precyzją arytmetyki $\epsilon$ (unit roundoff $u$):**
  Precyzja arytmetyki (maksymalny błąd względny zaokrąglenia liczby do siatki) to **połowa** epsilona maszynowego:
  $$\epsilon = u = \frac{1}{2}\text{macheps} = 2^{-t-1}$$
  Dla `Float64`: $\epsilon = 2^{-53} \approx 1.11 \cdot 10^{-16}$.

---

### B. Liczba $\eta$ a $\text{MIN}_{\text{sub}}$ oraz $\text{MIN}_{\text{nor}}$
To jest **ulubione pytanie egzaminacyjne**:
* **Liczby znormalizowane:** Mantysa ma postać $1.m$. Najmniejsza taka liczba dodatnia to:
  $$\text{MIN}_{\text{nor}} = 2^{e_{\min}} \cdot 1.0$$
  Dla `Float64`: cecha minimalna to $e_{\min} = -1022$, więc $\text{MIN}_{\text{nor}} = 2^{-1022} \approx 2.225 \cdot 10^{-308}$.
  Zwraca ją funkcja `floatmin(Float64)`.
* **Liczby podnormalne (*subnormal / denormal*):** Co zrobić, gdy wynik operacji jest mniejszy niż $2^{-1022}$, ale większy od 0? W starych komputerach następował natychmiastowy skok do zera (*flush to zero*), co psuło tożsamości typu $x - y = 0 \iff x = y$. 
  Twórca IEEE 754 (William Kahan) wprowadził liczby podnormalne: gdy cecha osiąga minimum (same zera w bicie cechy), ukryta jedynka znika i staje się **ukrytym zerem** ($0.m$).
  Mantysa zaczyna tracić bity precyzji, ale liczba maleje aż do najmłodszego bitu mantysy:
  $$\text{MIN}_{\text{sub}} = 2^{e_{\min}} \cdot 2^{-t} = 2^{e_{\min} - t}$$
  Dla `Float64`: $2^{-1022 - 52} = 2^{-1074} \approx 4.94 \cdot 10^{-324}$.
* **Liczba $\eta$ z zadania:** Pętla `while e/2.0 > 0.0; e /= 2.0; end` dzieli liczbę przez 2 tak długo, jak długo nie nastąpi całkowity niedomiar (*underflow*) do maszynowego $0.0$. Dlatego $\eta$ wyznacza dokładnie $\text{MIN}_{\text{sub}}$, czyli $2^{-1074}$!
* **Kluczowa relacja:**
  $$\text{MIN}_{\text{sub}} = \text{macheps} \cdot \text{MIN}_{\text{nor}} \quad (2^{-1074} = 2^{-52} \cdot 2^{-1022})$$

---

### C. Liczba $\text{MAX}$
* Największa skończona liczba maszynowa to taka, która ma maksymalny dozwolony wykładnik oraz wszystkie bity mantysy zapalone na 1:
  $$\text{MAX} = (2 - 2^{-t}) \cdot 2^{e_{\max}}$$
  Dla `Float64`: $e_{\max} = 1023$, więc $(2 - 2^{-52}) \cdot 2^{1023} \approx 1.797693 \cdot 10^{308}$.
* **Jak działa algorytm z zadania?**
  1. Krok 1: Mnożymy $x$ przez 2, dopóki `!isinf(x * 2)`. Znajdujemy potęgę $2^{1023}$.
  2. Krok 2: Uzupełniamy mantysę: bierzemy $k = x/2$, potem $x/4$, $x/8$ itd. i dodajemy do $x$, jeśli suma nie staje się nieskończonością (`!isinf(x + k)`). W ten sposób zapalamy kolejne 52 bity mantysy.

---

## 3. Zadanie 2: Rozmieszczenie Liczb i Gęstość Siatki

W tym zadaniu sprawdzamy, jak gęsto leżą liczby zmiennopozycyjne w różnych przedziałach.

### Główny wniosek:
Liczby zmiennopozycyjne **NIE są rozmieszczone równomiernie na całej osi**, ale są **równomierne lokalnie w przedziałach między kolejnymi potęgami dwójki**: $[2^p, 2^{p+1})$.

| Przedział | Wykładnik $2^p$ | Krok siatki $\delta$ (odległość sąsiadów) | Związek z $\text{macheps} = 2^{-52}$ |
| :---: | :---: | :---: | :---: |
| $[1/2, 1)$ | $2^{-1}$ | $\delta = 2^{-1 - 52} = 2^{-53}$ | $\frac{1}{2} \text{macheps} \approx 1.11 \cdot 10^{-16}$ |
| $[1, 2)$ | $2^0$ | $\delta = 2^{0 - 52} = 2^{-52}$ | $1 \cdot \text{macheps} \approx 2.22 \cdot 10^{-16}$ |
| $[2, 4)$ | $2^1$ | $\delta = 2^{1 - 52} = 2^{-51}$ | $2 \cdot \text{macheps} \approx 4.44 \cdot 10^{-16}$ |

### Dlaczego tak się dzieje?
Wewnątrz przedziału $[2^p, 2^{p+1})$ wykładnik cechy jest **stały**. Wszystkie $2^{52}$ liczb maszynowych w tym przedziale powstaje przez zmianę 52-bitowej mantysy. Zmiana najmłodszego bitu mantysy przesuwa wartość dokładnie o:
$$\text{krok} = 2^p \cdot 2^{-52} = 2^{p-52}$$
Gdy przekraczamy potęgę dwójki (np. przechodzimy z $1.999...$ do $2.0$), wykładnik zwiększa się o 1, a zatem **waga każdego bitu mantysy podwaja się**. Odległość między liczbami maszynowymi rośnie dwukrotnie!

---

## 4. Zadanie 3: Zagadka $x \cdot (1/x) \neq 1$

W matematyce na kartce papieru $x \cdot \frac{1}{x} \equiv 1$ dla każdego $x \neq 0$. Dlaczego w komputerze tak nie jest?

### Co się dzieje w komputerze?
Komputer wykonuje to w dwóch krokach zaokrąglenia:
1. $y = \text{fl}(1.0 / x)$ — odwrotność liczby $x$ prawie nigdy nie jest skończonym ułamkiem dwójkowym, więc musi zostać zaokrąglona do najbliższej liczby maszynowej. Powstaje mały błąd $\delta$:
   $$\text{fl}(1/x) = \frac{1}{x} + \delta, \quad \text{gdzie } |\delta| \le 2^{-54}$$
2. Komputer mnoży: $\text{fl}(x \cdot y) = \text{fl}(x \cdot (1/x + \delta)) = \text{fl}(1 + x\delta)$.

### Dlaczego wynik nigdy nie przekroczy 1.0 dla $x \in (1, 2)$?
Dla $x \in (1, 2)$ mamy $x < 2$. Zatem:
$$x\delta < 2 \cdot 2^{-54} = 2^{-53}$$
Przypomnijmy: w przedziale $[1, 2)$ odległość między liczbami wynosi $2^{-52}$. Aby liczba $1.0 + x\delta$ została zaokrąglona w górę do kolejnej liczby maszynowej ($1 + 2^{-52}$), nadwyżka musiałaby przekroczyć połowę odległości, czyli $\ge 2^{-53}$.
Ponieważ $x\delta$ jest **ściśle mniejsze niż $2^{-53}$**, zaokrąglenie w górę jest **niemożliwe fizycznie**! Wynik może wynosić albo dokładnie $1.0$, albo zaokrąglić się w dół do $1.0 - 2^{-53}$.

### Dlaczego najmniejsza taka liczba pojawia się dopiero przy kroku $k = 257\,736\,490$?
Tuż powyżej $1.0$ (dla małych $k$):
$$x = 1 + k \cdot 2^{-52} \approx 1.0$$
Odwrotność wynosi $1/x \approx 1 - k \cdot 2^{-52}$. Błąd zaokrąglenia $\delta$ przy dzieleniu przez $x \approx 1$ jest zbyt mały, by zepchnąć iloczyn poniżej progu zaokrąglenia do jedynki. Dopiero gdy $x$ oddali się wystarczająco od $1.0$ (właśnie przy $k \approx 2.57 \cdot 10^8$, czyli $x \approx 1.000000057$), błąd zaokrąglenia odwrotności $\text{fl}(1/x)$ trafia w tak niefortunny układ bitów, że iloczyn daje $1.0 - 2^{-53} \neq 1.0$.

---

## 5. Zadanie 4: Iloczyn Skalarny i Redukcja Cyfr Znaczących

W tym zadaniu liczymy iloczyn skalarny $x^T y = \sum_{i=1}^5 x_i y_i$ dla wektorów:
$$x = [2.718, -3.141, 1.414, 0.577, 0.301]^T$$
$$y = [1486.2, 878366.9, -22.37, 4773714.6, 0.000185]^T$$

Wartość analityczna wynosi:
$$S_{\text{dokładna}} = -1.00657107 \cdot 10^{-11}$$

### Co jest sednem tego zadania?
Policzmy iloczyny cząstkowe $p_i = x_i y_i$:
- $p_1 \approx +4\,040.05$
- $\mathbf{p_2 \approx -2\,759\,471.28}$
- $p_3 \approx -31.64$
- $\mathbf{p_4 \approx +2\,755\,462.87}$
- $p_5 \approx +0.000055$

Dwa składniki ($p_2$ i $p_4$) są olbrzymie (rzędu $2.76 \cdot 10^6$) i mają przeciwne znaki! Ich suma to $-4008.41$.
Ta wartość niemal dokładnie znosi się z sumą pozostałych wyrazów ($p_1 + p_3 = 4008.41$).
Wynik rzędu $10^{-11}$ powstaje przez **odjęcie od siebie dwóch liczb rzędu $10^6$**.

### Definicja: Redukcja Cyfr Znaczących (*Catastrophic Cancellation*)
Gdy odejmujemy od siebie dwie bardzo bliskie liczby maszynowe, ich najbardziej znaczące cyfry (identyczne) odejmują się do zera. Na początku mantysy powstają same zera. Po normalizacji komputer przesuwa mantysę w lewo, a na koniec dokłada losowy szum bitowy. **Tracimy cyfry znaczące.**

### Dlaczego Float32 daje całkowitą katastrofę (błąd $\approx 0.5$)?
W formacie `Float32` mamy tylko 23 bity mantysy (ok. 7 cyfr dziesiętnych).
Dla liczby rzędu $2.75 \cdot 10^6 \approx 2^{21.4}$, waga najmłodszego bitu ($\text{ulp}$) wynosi:
$$\text{ulp} = 2^{21 - 23} = 2^{-2} = 0.25 \quad (\text{lub } 0.5)$$
**Oznacza to, że w pojedynczej precyzji w okolicach 2 milionów najmniejsza różnica między dwiema kolejnymi liczbami to $0.25$!**
Komputer fizycznie nie ma możliwości odróżnić liczby $2\,759\,471.25$ od $2\,759\,471.26$. Skoro rozdzielczość siatki wynosi $0.25$, to błąd wyniku **musi** być rzędu $10^{-1}$. Otrzymanie dokładnego wyniku rzędu $10^{-11}$ w `Float32` jest absolutnie niemożliwe.

### Dlaczego w Float64 metody (c) i (d) dają dokładnie 0.0?
Metody (c) i (d) najpierw sumują wszystkie liczby dodatnie ($S_1$), a osobno ujemne ($S_2$):
$$S_1 = p_1 + p_4 + p_5 = 2\,759\,502.9196180594$$
$$S_2 = p_2 + p_3 = -2\,759\,502.9196180594$$
Ponieważ `Float64` ma 53 bity precyzji (ok. 16 cyfr dziesiętnych), po zsumowaniu składników o wartości $10^6$ najmłodsze bity rzędu $10^{-11}$ zostały bezpowrotnie utracone (obcięte). W pamięci komputera sumy $S_1$ i $S_2$ miały **dokładnie identyczny ciąg bitów mantysy i cechy**, różniąc się tylko znakiem. 
Po dodaniu: $S_1 + S_2 \equiv 0.0$.
Błąd wynosi wtedy $|-1.0065 \cdot 10^{-11} - 0.0| \approx 1.01 \cdot 10^{-11}$.

---

## 6. Zadanie 5: Stabilność Funkcji $f(x)$ vs $g(x)$

Badamy dwie funkcje dla argumentów $x = 8^{-k}$ ($k = 1, 2, \dots, 25$):
$$f(x) = \sqrt{x^2 + 1} - 1, \qquad g(x) = \frac{x^2}{\sqrt{x^2 + 1} + 1}$$

Algebraicznie $f(x) \equiv g(x)$, bo wzór $g(x)$ powstał przez pomnożenie licznika i mianownika $f(x)$ przez sprzężenie:
$$\frac{(\sqrt{x^2+1}-1)(\sqrt{x^2+1}+1)}{\sqrt{x^2+1}+1} = \frac{(x^2+1)-1}{\sqrt{x^2+1}+1} = \frac{x^2}{\sqrt{x^2+1}+1}$$

### Dlaczego komputer daje drastycznie różne wyniki?
* **W funkcji $f(x)$:** 
  Dla małych $x$ wielkość $x^2$ staje się mikroskopijna. 
  Gdy $k \ge 9$, mamy $x = 8^{-9} = (2^3)^{-9} = 2^{-27}$.
  Wtedy:
  $$x^2 = (2^{-27})^2 = 2^{-54}$$
  W arytmetyce `Float64` odległość od 1 do kolejnej liczby maszynowej to $\text{macheps} = 2^{-52}$.
  Liczba $x^2 = 2^{-54}$ jest mniejsza niż połowa kroku siatki ($2^{-53}$)!
  Zatem przy dodawaniu do 1:
  $$\text{fl}(1.0 + x^2) = 1.0$$
  Wtedy komputer liczy:
  $$\sqrt{1.0} - 1.0 = 1.0 - 1.0 = 0.0$$
  Dla każdego $k \ge 9$ funkcja $f(x)$ zwraca dokładnie **zero maszynowe**! Błąd względny wynosi **100%**.

* **W funkcji $g(x)$:**
  W mianowniku dodajemy dwie liczby dodatnie: $\sqrt{x^2+1} + 1 \approx 1 + 1 = 2$.
  Nawet jeśli $x^2$ zniknie w mianowniku, mianownik wyniesie po prostu $2.0$.
  W liczniku mamy $x^2$, które dla $k=9$ wynosi $2^{-54} \approx 5.55 \cdot 10^{-17}$ (spokojnie mieści się w zakresie `Float64`, bo liczby skończone sięgają do $10^{-308}$).
  Komputer bez problemu dzieli:
  $$g(x) \approx \frac{x^2}{2} = \frac{2^{-54}}{2} = 2^{-55} \approx 2.77 \cdot 10^{-17}$$
  Wynik jest stabilny i precyzyjny aż do $k \approx 180$ (kiedy $x^2$ osiągnie dolną granicę podnormalną `Float64`).

---

## 7. Zadanie 6: Różniczkowanie Numeryczne i Anatomia Wykresu "V"

Przybliżamy pochodną funkcji $f(x) = \sin x + \cos 3x$ w punkcie $x_0 = 1$ za pomocą ilorazu różnicowego w przód:
$$\tilde{f}'(x_0) = \frac{f(x_0 + h) - f(x_0)}{h}, \quad \text{dla } h = 2^{-n},\ n = 0, 1, \dots, 54$$

Wartość analityczna: $f'(1) = \cos(1) - 3\sin(3) \approx 0.11694228$.

### Dlaczego na wykresie błędu w skali log-log powstaje kształt litery "V"?
Błąd całkowity różniczkowania numerycznego to suma dwóch **sprzecznych** zjawisk:
$$E_{\text{całkowity}}(h) = E_{\text{obcięcia}}(h) + E_{\text{zaokrągleń}}(h)$$

```
 Błąd (skala log)
   ^
   |  \                                 /
   |   \  Błąd zaokrągleń              /  Błąd obcięcia (metody)
   |    \   O(ε / h)                  /     O(h)
   |     \                           /
   |      \                         /
   |       \                       /
   |        \                     /
   |         \       *           /
   |          \     / \         /
   |           \___/   \_______/
   |                  |
   +------------------+--------------------> Krok h (skala log)
                     h_opt ≈ √macheps ≈ 10^-8
```

#### 1. Błąd metody / obcięcia ($E_{\text{obc}}$) — PRAWA STRONA WYKRESU:
* Wynika ze wzoru Taylora: odrzucamy wyrazy wyższych rzędów:
  $$f(x_0 + h) = f(x_0) + f'(x_0)h + \frac{f''(x_0)}{2}h^2 + O(h^3)$$
  $$\frac{f(x_0+h) - f(x_0)}{h} - f'(x_0) \approx \frac{|f''(x_0)|}{2} \cdot h = C \cdot h$$
* Błąd obcięcia maleje proporcjonalnie do $h$: $E_{\text{obc}} = O(h)$.
* W skali podwójnie logarytmicznej: $\log(E) \approx \log(C) + 1 \cdot \log(h)$. Jest to **idealnie prosta linia** o nachyleniu 1.
* Na prawej stronie wykresu błąd obcięcia jest tak duży (np. $10^{-2}$ czy $10^{-4}$), że szum maszynowy rzędu $10^{-16}$ jest zupełnie niewidoczny.

#### 2. Błąd zaokrągleń maszynowych ($E_{\text{zaokr}}$) — LEWA STRONA WYKRESU:
* Komputer nie oblicza $f(x_0+h)$ dokładnie, lecz z błędem zaokrąglenia $\epsilon_{\text{mach}} \cdot f(x)$:
  $$\tilde{f}(x_0+h) = f(x_0+h) + \epsilon_1, \quad \tilde{f}(x_0) = f(x_0) + \epsilon_2$$
* W liczniku odejmujemy te dwie bliskie liczby. Następuje redukcja cyfr znaczących: w liczniku zostaje błąd rzędu $\approx 2\epsilon |f(x_0)|$.
* Następnie **dzielimy ten błąd przez $h$**:
  $$E_{\text{zaokr}} \approx \frac{2\epsilon |f(x_0)|}{h} = O\left(\frac{\epsilon}{h}\right)$$
* Im mniejsze $h$, tym przez mniejszą liczbę dzielimy — **błąd zaokrąglenia eksploduje do góry**!
* Ząbkowanie po lewej stronie wynika z faktu, że liczby maszynowe są dyskretne, więc zaokrąglenia losowo przeskakują o 1 ulp w górę lub w dół.

#### 3. Punkt optymalny $h_{\text{opt}}$ (dno litery V):
Minimum błędu zachodzi, gdy obie składowe się zrównują:
$$\frac{M_2}{2} h \approx \frac{2\epsilon M_0}{h} \implies h^2 \approx \frac{4\epsilon M_0}{M_2} \implies h_{\text{opt}} \approx \sqrt{\text{macheps}}$$
Dla `Float64`: $\text{macheps} = 2^{-52}$, więc:
$$h_{\text{opt}} \approx \sqrt{2^{-52}} = 2^{-26} \approx 1.49 \cdot 10^{-8}$$
W naszym eksperymencie najmniejszy błąd otrzymaliśmy dokładnie dla $n = 28$ ($h \approx 3.7 \cdot 10^{-9}$, błąd $4.8 \cdot 10^{-9}$).

#### 4. Co się dzieje dla $n \ge 53$? (Pozioma linia na samym końcu):
Gdy $n \ge 53$, to $h \le 2^{-53}$.
Wtedy $h$ jest mniejsze niż połowa odległości między $1.0$ a kolejną liczbą maszynową ($2^{-52}$).
Komputer zaokrągla:
$$\text{fl}(1.0 + h) = 1.0$$
W rezultacie licznik ilorazu to:
$$f(\text{fl}(1+h)) - f(1) = f(1.0) - f(1.0) = 0.0$$
Przybliżenie pochodnej wynosi tożsamościowo $0.0$, a błąd stabilizuje się na stałym poziomie:
$$|f'(1) - 0.0| = |f'(1)| \approx 0.116942$$

---

## 8. Ściąga Pytań na Obronę (Quick Q&A)

**Q1: Czym różni się `eps(Float64)` od `floatmin(Float64)`?**  
*Odp:* `eps` to odległość od 1.0 do kolejnej liczby maszynowej ($2^{-52}$), miara gęstości siatki. `floatmin` to najmniejsza liczba maszynowa *znormalizowana* ($2^{-1022}$), miara zakresu wielkości liczb.

**Q2: Dlaczego `floatmin` to $2^{-1022}$, a najmniejsza liczba dodatnia w pętli (eta) to $2^{-1074}$?**  
*Odp:* Ponieważ standard IEEE 754 obsługuje liczby podnormalne (*subnormal*). Po przekroczeniu $2^{-1022}$ znika ukryta jedynka w mantysie i tracimy kolejne bity mantysy, aż do $2^{-1022} \cdot 2^{-52} = 2^{-1074}$.

**Q3: Dlaczego liczby w $[2, 4]$ mają 2 razy rzadszy krok niż w $[1, 2]$?**  
*Odp:* Ponieważ cecha (wykładnik) wzrosła o 1 (z $2^0$ na $2^1$). Liczba bitów mantysy jest stała (52), więc w przedziale 2 razy szerszym ta sama liczba punktów musi leżeć 2 razy rzadziej (krok $2 \cdot 2^{-52} = 2^{-51}$).

**Q4: Dlaczego dla $x \in (1, 2)$ iloczyn $x \cdot (1/x)$ nigdy nie jest większy od 1?**  
*Odp:* Ponieważ maksymalny błąd zaokrąglenia odwrotności wynosi $2^{-54}$. Po pomnożeniu przez $x < 2$ błąd wynosi mniej niż $2^{-53}$. Próg zaokrąglenia w górę powyżej 1.0 wynosi dokładnie $2^{-53}$, więc nadwyżka jest zbyt mała, by zaokrąglić w górę.

**Q5: Na czym polega zjawisko redukcji cyfr znaczących (*cancellation*)?**  
*Odp:* Na odjęciu od siebie dwóch bardzo bliskich liczb maszynowych. Identyczne najbardziej znaczące cyfry znoszą się do zera, a na pozycji cyfr znaczących lądują losowe błędy zaokrągleń z poprzednich działań.

**Q6: Dlaczego w zadaniu 4 w Float32 błąd wyniósł aż 0.5?**  
*Odp:* Bo składniki sumy były rzędu $2.75 \cdot 10^6$. W `Float32` dla liczb rzędu milionów waga najmłodszego bitu wynosi $\text{ulp} \approx 0.25 - 0.5$. W tej precyzji fizycznie nie da się zarejestrować różnic na poziomie $10^{-11}$.

**Q7: Skąd bierze się optymalny krok $h \approx 10^{-8}$ przy różniczkowaniu numerycznym?**  
*Odp:* Z przecięcia dwóch funkcji błędu: malejącego błędu metody $O(h)$ i rosnącego błędu zaokrągleń $O(\epsilon / h)$. Minimum wypada w $\sqrt{\text{macheps}} = \sqrt{2^{-52}} = 2^{-26} \approx 1.49 \cdot 10^{-8}$.
