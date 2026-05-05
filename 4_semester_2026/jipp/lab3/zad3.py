import sys
import random
import math
from functools import lru_cache
from typing import TypeVar, Generic, Type, Any

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
            try:
                inv = pow(other.a, -1, n)
                return RingN((self.a * inv) % n)
            except ValueError:
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
            try:
                inv = pow(other.a, -1, n)
                self.a = (self.a * inv) % n
                return self
            except ValueError:
                raise ValueError("Brak rozwiazan")
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

T = TypeVar('T') 

class RSA(Generic[T]):
    def __init__(self, p: int, q: int):
        self._n = p * q
        self._fi = (p - 1) * (q - 1)
        while True:
            self._e = random.randint(2, self._fi - 1)
            if math.gcd(self._e, self._fi) == 1:
                break
                
        self._d = pow(self._e, -1, self._fi)

    def getModulo(self) -> int:
        return self._n

    def getPublicKey(self, ring_class: Type[T]) -> T:
        """Zwraca klucz publiczny opakowany w podaną klasę pierścienia."""
        return ring_class(self._e)

    def encrypt(self, m: T) -> T:
        result = type(m)(1) 
        base = m
        exp = self._e

        while exp > 0:
            if exp % 2 == 1:
                result = result * base 
            base = base * base      
            exp //= 2

        return result

    def decrypt(self, c: T) -> T:
        result = type(c)(1)
        base = c
        exp = self._d

        while exp > 0:
            if exp % 2 == 1:
                result = result * base
            base = base * base
            exp //= 2

        return result


if __name__ == "__main__":
    try:
        p = 10007
        q = 10009
        n = p * q

        RingN = Ring[n]
    
        rsa = RSA[RingN](p, q)
        
        print("--- Parametry RSA ---")
        print(f"Modulo (n): {rsa.getModulo()}")
        print(f"Klucz publiczny (e): {rsa.getPublicKey(RingN)}")

        m = RingN(1234567) 
        print("\n--- Szyfrowanie ---")
        print(f"Oryginalna wiadomosc (m): {m}")
        
        c = rsa.encrypt(m)
        print(f"Szyfrogram (c): {c}")
        
        zdekodowana = rsa.decrypt(c)
        print(f"Zdekodowana wiadomosc: {zdekodowana}")

        print("\nCzy zdekodowana == oryginalna? ", end="")
        print("TAK :)" if m == zdekodowana else "NIE :(")

    except Exception as e:
        print(f"wyjatek: {e}", file=sys.stderr)