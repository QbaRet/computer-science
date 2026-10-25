# Ściągawka i przewodnik do obrony - Laboratorium 1 AOD
## Podstawowe algorytmy grafowe

Ten dokument zawiera esencję teoretyczną i praktyczną potrzebną do zaliczenia i obrony Listy 1.

---

## 1. Zagadnienia ogólne i Zadanie 0 (Metodologia badań algorytmów)

### O co chodzi w Zadaniu 0 (David S. Johnson [Joh02], AMO93 rozdz. 18)?
Prowadzący pyta o kulturę eksperymentalną i pułapki implementacyjne przy grafach rzędu $10^6$ wierzchołków:
1. **Separacja I/O od czasu algorytmu**: Nie mierzymy czasu wczytywania danych z dysku (`cin`), tylko czysty czas wykonania algorytmu w pamięci RAM. Używamy `std::chrono::high_resolution_clock`.
2. **Reprezentacja grafu**:
   - Używamy **list sąsiedztwa** (`std::vector<std::vector<int>>`), co daje pamięć $O(|V| + |E|)$ (dla $10^6$ wierzchołków to kilkadziesiąt megabajtów).
   - **Pułapka**: Macierz sąsiedztwa $O(|V|^2)$ dla $n = 10^6$ wymagałaby $10^{12}$ bajtów = **1 Terabajt pamięci RAM**, co natychmiast zawiesiłoby komputer.
3. **Pułapka przepełnienia stosu (Stack Overflow)**:
   - Domyślny rozmiar stosu systemowego wątku to 8 MB.
   - W grafie będącym długą ścieżką o długości $10^6$ wierzchołków (np. `g3-6.txt`), rekurencyjny DFS tworzy $10^6$ ramek stosu i powoduje `Segmentation Fault` (kod błędu 139).
   - **Rozwiązanie**: Zastąpienie rekurencji jawnym stosem na stercie (`std::vector<std::pair<int, size_t>> st`).
4. **Szybkie wejście/wyjście (Fast I/O)**:
   - Wyłączenie synchronizacji strumieni C++ ze standardowym wejściem C: `std::ios_base::sync_with_stdio(false); std::cin.tie(nullptr);`. Skraca czas wczytywania pliku 40 MB z kilku sekund do ułamka sekundy.

---

## 2. Zadanie 1 – Przeszukiwanie grafów (DFS i BFS)

### BFS (Breadth-First Search, przeszukiwanie wszerz)
- **Struktura**: Kolejka FIFO (`std::queue`).
- **Zasada działania**: Odwiedza wierzchołki warstwami według odległości od źródła (falami: najpierw w odległości 1, potem 2, itd.).
- **Główna własność**: W grafach nieważonych BFS znajduje najkrótsze ścieżki (pod względem liczby krawędzi).
- **Drzewo BFS**: Krawędzie $(u, v)$, po których nastąpiło pierwsze dotarcie do nieodwiedzonego wierzchołka $v$ z wierzchołka $u$.
- **Złożoność**: Czas $O(|V| + |E|)$, Pamięć $O(|V| + |E|)$.

### DFS (Depth-First Search, przeszukiwanie wgłąb)
- **Struktura**: Stos LIFO (`std::stack` lub symulacja wektorem).
- **Zasada działania**: Schodzi wzdłuż danej ścieżki tak głęboko, jak to możliwe. W ślepym zaułku cofa się (backtracking) do ostatniego rozgałęzienia.
- **Drzewo DFS**: Krawędzie drzewowe prowadzące do nowo odkrytych wierzchołków.
- **Klasyfikacja krawędzi w DFS**:
  - *Drzewowe (Tree edges)*: wchodzące do nieodwiedzonych wierzchołków.
  - *Powrotne (Back edges)*: prowadzące do przodka w drzewie DFS $\to$ **świadczą o istnieniu cyklu!**
  - *W przód (Forward edges)*: prowadzące do potomka w drzewie DFS.
  - *Poprzeczne (Cross edges)*: łączące gałęzie bez relacji przodek-potomek.
- **Złożoność**: Czas $O(|V| + |E|)$, Pamięć $O(|V| + |E|)$.

---

## 3. Zadanie 2 – Sortowanie topologiczne i detekcja cyklu

### Podstawy teoretyczne
- **DAG (Directed Acyclic Graph)**: Graf skierowany bez cykli skierowanych.
- **Porządek topologiczny**: Liniowe uporządkowanie wierzchołków $v_1, v_2, \ldots, v_n$, w którym dla każdej krawędzi skierowanej $u \to v$, wierzchołek $u$ pojawia się przed $v$.
- **Warunek konieczny i wystarczający**: Graf skierowany ma porządek topologiczny **wtedy i tylko wtedy, gdy jest acykliczny (jest DAG-iem)**.

### Algorytm Kahna (zastosowany w kodzie)
1. Oblicz stopień wejściowy (`in_degree[u]`) każdego wierzchołka (ile krawędzi wchodzi do $u$).
2. Wszystkie wierzchołki o `in_degree[u] == 0` wrzuć do kolejki FIFO.
3. W pętli pobieraj wierzchołek $u$ z kolejki, dodaj do porządku topologicznego i dla każdego sąsiada $v$ zmniejsz `in_degree[v]--`.
4. Jeśli `in_degree[v] == 0`, wrzuć $v$ do kolejki.
5. **Wykrywanie cyklu**:
   - Jeśli liczba zdjętych wierzchołków równa się $|V|$ $\implies$ graf jest **acykliczny (DAG)**.
   - Jeśli liczba zdjętych wierzchołków jest mniejsza niż $|V|$ $\implies$ wierzchołki o `in_degree > 0` tworzą **skierowany cykl** (nie można ich usunąć, bo zależą od siebie nawzajem).
- **Złożoność**: Czas $O(|V| + |E|)$, Pamięć $O(|V| + |E|)$.

---

## 4. Zadanie 3 – Silnie spójne składowe (SCC)

### Co to jest silnie spójna składowa (SCC)?
- Podzbiór wierzchołków $C \subseteq V$ w grafie skierowanym, w którym **z każdego wierzchołka da się dojść do każdego innego** ścieżką skierowaną (wzajemna osiągalność: $u \rightsquigarrow v$ i $v \rightsquigarrow u$).
- **Graf kondensacji**: Graf, w którym każda składowa SCC jest zwinięta w jeden super-wierzchołek. Graf kondensacji jest **zawsze DAG-iem**!

### Algorytm Kosaraju-Sharir (zastosowany w kodzie)
Działa w dwóch przejściach DFS:
1. **Faza 1 (DFS na $G$)**: Uruchamiamy DFS na oryginalnym grafie. Wierzchołki wrzucamy na stos w kolejności **zakończenia ich przetwarzania** (post-order finish time).
2. **Odwrócenie grafu**: Tworzymy graf transponowany $G^T$ (wszystkie krawędzie $u \to v$ zamienione na $v \to u$).
3. **Faza 2 (DFS na $G^T$)**: Zdejmujemy wierzchołki ze stosu z Fazy 1. Jeśli dany wierzchołek nie był jeszcze odwiedzony w $G^T$, uruchamiamy z niego DFS. Wierzchołki odwiedzone w tym jednym wywołaniu tworzą **dokładnie jedną silnie spójną składową**.

### Dlaczego to działa?
Wierzchołek o najwyższym czasie zakończenia w Fazie 1 należy do źródłowej składowej SCC w grafie kondensacji. W odwróconym grafie $G^T$ źródłowa składowa staje się ujściem (sink). Wykonując DFS w $G^T$, nie możemy "uciec" do innych składowych, więc odwiedzamy wyłącznie wierzchołki tej jednej składowej.
- **Złożoność**: Czas $O(|V| + |E|)$ (dwa przejścia DFS), Pamięć $O(|V| + |E|)$.

---

## 5. Zadanie 4 – Badanie dwudzielności grafu

### Definicja grafu dwudzielnego
Graf $G = (V, E)$ jest dwudzielny, jeśli zbiór wierzchołków $V$ można podzielić na dwa rozłączne podzbiory $V_0$ i $V_1$ ($V = V_0 \cup V_1, V_0 \cap V_1 = \emptyset$) takie, że każda krawędź łączy wierzchołek z $V_0$ z wierzchołkiem z $V_1$. Żadna krawędź nie może łączyć dwóch wierzchołków z tego samego podzbioru.

### Kluczowe twierdzenie
> **Graf jest dwudzielny wtedy i tylko wtedy, gdy NIE zawiera cyklu o nieparzystej długości.**

### Dlaczego kierunek krawędzi nie ma znaczenia w Zadaniu 4?
Treść zadania definiuje warunek: jeśli $(u, v) \in E$, to $u \in V_i$ i $v \in V_{1-i}$. Oznacza to, że istnienie krawędzi wymusza różny kolor wierzchołków $u$ i $v$ ($color(u) \neq color(v)$), niezależnie od jej zwrotu. Krawędź skierowana $u \to v$ narzuca dokładnie takie samo ograniczenie na podział jak krawędź nieskierowana $\{u, v\}$. Dlatego graf skierowany badamy na jego nieskierowanym grafie ograniczeń.

### Algorytm (2-kolorowanie BFS)
1. Każdemu wierzchołkowi przypisujemy kolor $-1$ (nieodwiedzony).
2. Dla każdej niespójnej składowej wybieramy startowy wierzchołek $s$ i ustawiamy `color[s] = 0`.
3. Przechodzimy graf algorytmem BFS:
   - Dla każdego sąsiada $v$ wierzchołka $u$:
     - Jeśli `color[v] == -1`, nadajemy mu kolor przeciwny: `color[v] = 1 - color[u]` i wrzucamy do kolejki.
     - Jeśli `color[v] == color[u]`, to wykryliśmy krawędź wewnątrz tej samej części $\implies$ istnieje cykl nieparzysty $\implies$ **graf NIE jest dwudzielny**.
4. Jeśli cały graf uda się pokolorować bez konfliktów $\implies$ **graf JEST dwudzielny**, a podział to $V_0 = \{u : color[u] == 0\}$, $V_1 = \{u : color[u] == 1\}$.

### Pytanie z listy: „Jaką złożoność ma zaimplementowany algorytm?”
- **Złożoność czasowa: $O(|V| + |E|)$**
  *Uzasadnienie*: Zbudowanie symetrycznego grafu krawędzi trwa $O(|E|)$. Algorytm BFS odwiedza każdy wierzchołek dokładnie jeden raz (wstawienie i zdjęcie z kolejki) oraz przegląda każdą krawędź stałą liczbę razy (dwukrotnie w grafie nieskierowanym).
- **Złożoność pamięciowa: $O(|V| + |E|)$**
  *Uzasadnienie*: Listy sąsiedztwa zajmują pamięć proporcjonalną do liczby wierzchołków i krawędzi, a tablica kolorów i kolejka BFS zajmują $O(|V|)$.

---

## 6. Szybkie Q&A na obronę laboratorium

1. **Dlaczego w Zadaniu 1 w drzewie przeszukiwania jest $n-1$ krawędzi?**
   Drzewo rozpinające grafu spójnego o $n$ wierzchołkach zawsze posiada dokładnie $n - 1$ krawędzi bez cykli. Jeśli graf ma $k$ składowych, las przeszukiwania ma $n - k$ krawędzi.
2. **Czym różni się drzewo DFS od drzewa BFS?**
   Drzewo BFS jest zazwyczaj "płaskie i szerokie" (minimalizuje głębokość, wyznacza najkrótsze ścieżki). Drzewo DFS jest "wąskie i głębokie" (idzie jak najdalej w głąb).
3. **Czy każdy DAG ma dokładnie jeden porządek topologiczny?**
   Nie. Graf może mieć wiele poprawnych porządków topologicznych. Dokładnie jeden istnieje tylko wtedy, gdy w grafie istnieje ścieżka Hamiltona.
4. **Co oznacza, że graf ma tylko 1 silnie spójną składową o rozmiarze $n$?**
   Oznacza to, że graf jest silnie spójny (z każdego wierzchołka da się dojść do każdego innego).
5. **Jak szybko sprawdzić, czy graf ma cykl nieparzysty?**
   Uruchomić 2-kolorowanie (BFS lub DFS). Jeśli napotkamy krawędź między wierzchołkami o tym samym kolorze, graf ma cykl nieparzysty.
