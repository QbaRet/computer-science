import sys

class Ring:
    def __init__(self, n: int, a: int = 0):
        self.n = n
        self.a = a % n

    def _sprawdz_zgodnosc(self, other):
        if self.n != other.n:
            raise ValueError("Rozne podstawy")

    def __add__(self, other):
        self._sprawdz_zgodnosc(other)
        return Ring(self.n, self.a + other.a)

    def __sub__(self, other):
        self._sprawdz_zgodnosc(other)
        return Ring(self.n, self.a - other.a)

    def __mul__(self, other):
        self._sprawdz_zgodnosc(other)
        return Ring(self.n, self.a * other.a)

    def __truediv__(self, other):
        self._sprawdz_zgodnosc(other)
        for i in range(1, self.n):
            if (other.a * i) % self.n == 1:
                return Ring(self.n, self.a * i)
        raise ValueError("Brak rozwiazan")
    
    def __iadd__(self, other):
        self._sprawdz_zgodnosc(other)
        self.a = (self.a + other.a) % self.n
        return self

    def __isub__(self, other):
        self._sprawdz_zgodnosc(other)
        self.a = (self.a - other.a) % self.n
        return self

    def __imul__(self, other):
        self._sprawdz_zgodnosc(other)
        self.a = (self.a * other.a) % self.n
        return self

    def __itruediv__(self, other):
        self._sprawdz_zgodnosc(other)
        for i in range(1, self.n):
            if (other.a * i) % self.n == 1:
                self.a = (self.a * i) % self.n
                return self
        raise ValueError("Brak rozwiazan")
    
    def __eq__(self, other):
        if not isinstance(other, Ring): return False
        return self.n == other.n and self.a == other.a

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        self._sprawdz_zgodnosc(other)
        return self.a < other.a

    def __le__(self, other):
        self._sprawdz_zgodnosc(other)
        return self.a <= other.a

    def __gt__(self, other):
        self._sprawdz_zgodnosc(other)
        return self.a > other.a

    def __ge__(self, other):
        self._sprawdz_zgodnosc(other)
        return self.a >= other.a

    def __str__(self):
        return str(self.a)

if __name__ == "__main__":
    try:
        a = Ring(7, 10)
        b = Ring(7, 5)
        c = Ring(7)

        print(f"a: {a}")
        print(f"b: {b}")
        print(f"a + b = {a + b}")
        print(f"a - b = {a - b}")
        print(f"a * b = {a * b}")
        print(f"a / b = {a / b}")

        print(f"a == b: {str(a == b).lower()}")
        print(f"a != b: {str(a != b).lower()}")
        print(f"a < b:  {str(a < b).lower()} ")
        print(f"a > b:  {str(a > b).lower()}")
        print(f"a <= 3: {str(a <= Ring(7, 3)).lower()}")

        c = a
        print(f"c = a:     {c}")
        c += b
        print(f"c += b:    {c}")
        c -= b
        print(f"c -= b:    {c}")
        c *= b
        print(f"c *= b:    {c}")
        c /= b
        print(f"c /= b:    {c}")

        print("Podaj liczbe: ")
        inp = sys.stdin.readline()
        if inp:
            c = Ring(7, int(inp))
            print(f"Wprowadzono {c}")

        zero = Ring(7, 0)
        print("Proba dzielenia przez zero: ", end="")
        print(a / zero)

    except Exception as e:
        print(f"wyjatek:{str(e)}", file=sys.stderr)