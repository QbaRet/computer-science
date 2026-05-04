import sys
from functools import lru_cache

@lru_cache(maxsize=None)
def _ring_class(n: int):
    class RingN:
        __slots__ = ("a",)

        def __init__(self, val: int = 0):
            self.a = val % n
        def __add__(self, other: "RingN") -> "RingN":
            return RingN((self.a + other.a) % n)

        def __sub__(self, other: "RingN") -> "RingN":
            return RingN((n + self.a - other.a) % n)

        def __mul__(self, other: "RingN") -> "RingN":
            return RingN((self.a * other.a) % n)

        def __truediv__(self, other: "RingN") -> "RingN":
            for i in range(1, n):
                if (other.a * i) % n == 1:
                    return RingN((self.a * i) % n)
            raise ValueError("Brak rozwiazan")

        def __iadd__(self, other: "RingN") -> "RingN":
            self.a = (self.a + other.a) % n
            return self

        def __isub__(self, other: "RingN") -> "RingN":
            self.a = (n + self.a - other.a) % n
            return self

        def __imul__(self, other: "RingN") -> "RingN":
            self.a = (self.a * other.a) % n
            return self

        def __itruediv__(self, other: "RingN") -> "RingN":
            for i in range(1, n):
                if (other.a * i) % n == 1:
                    self.a = (self.a * i) % n
                    return self
            raise ValueError("Brak rozwiazan")

        # -------------------- porównania --------------------
        def __eq__(self, other: object) -> bool:
            return isinstance(other, RingN) and self.a == other.a

        def __ne__(self, other: object) -> bool:
            return not self.__eq__(other)

        def __lt__(self, other: "RingN") -> bool:
            return self.a < other.a

        def __le__(self, other: "RingN") -> bool:
            return self.a <= other.a

        def __gt__(self, other: "RingN") -> bool:
            return self.a > other.a

        def __ge__(self, other: "RingN") -> bool:
            return self.a >= other.a

        def __str__(self) -> str:
            return str(self.a)

        def __repr__(self) -> str:
            return f"Ring[{n}]({self.a})"

    RingN.__name__ = f"Ring[{n}]"
    RingN.__qualname__ = f"Ring[{n}]"
    return RingN


class RingMeta(type):

    def __getitem__(cls, n: int):
        if not isinstance(n, int) or n <= 0:
            raise TypeError("n musi byc dodatnia liczba calkowita")
        return _ring_class(n)


class Ring(metaclass=RingMeta):
    pass


if __name__ == "__main__":
    try:
        Ring7 = Ring[7] 

        a = Ring7(10)
        b = Ring7(5)
        c = Ring7()

        print(f"a: {a}")
        print(f"b: {b}")
        print(f"a + b = {a + b}")
        print(f"a - b = {a - b}")
        print(f"a * b = {a * b}")
        print(f"a / b = {a / b}")

        print(f"a == b: {str(a == b).lower()}")
        print(f"a != b: {str(a != b).lower()}")
        print(f"a < b:  {str(a < b).lower()}")
        print(f"a > b:  {str(a > b).lower()}")
        print(f"a <= 3: {str(a <= Ring7(3)).lower()}")

        c = Ring7(a.a)  
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
        if inp.strip():
            c = Ring7(int(inp))
            print(f"Wprowadzono {c}")

        zero = Ring7(0)
        print("Proba dzielenia przez zero: ", end="")
        print(a / zero)

    except Exception as e:
        print(f"wyjatek:{e}", file=sys.stderr)
