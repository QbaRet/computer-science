import sys
import random
from functools import lru_cache
from typing import TypeVar, Generic, Type

@lru_cache(maxsize=None)
def _ring_class(n: int):
    class RingN:
        __slots__ = ("a",)

        def __init__(self, val: int = 0):
            self.a = val % n
            
        def __add__(self, other: "RingN") -> "RingN": return RingN((self.a + other.a) % n)
        def __sub__(self, other: "RingN") -> "RingN": return RingN((n + self.a - other.a) % n)
        def __mul__(self, other: "RingN") -> "RingN": return RingN((self.a * other.a) % n)
        
        def __truediv__(self, other: "RingN") -> "RingN":
            try:
                inv = pow(other.a, -1, n)
                return RingN((self.a * inv) % n)
            except ValueError:
                raise ValueError("Brak odwrotnosci modularnej!")

        def __eq__(self, other: object) -> bool:
            return isinstance(other, RingN) and self.a == other.a

        def __str__(self) -> str: return str(self.a)
        def __repr__(self) -> str: return f"Ring[{n}]({self.a})"

    RingN.__name__ = f"Ring[{n}]"
    return RingN

class RingMeta(type):
    def __getitem__(cls, n: int): return _ring_class(n)

class Ring(metaclass=RingMeta): pass


T = TypeVar('T')

class DHSetup(Generic[T]):
    def __init__(self, ring_class: Type[T]):
        self._ring_class = ring_class
        r = random.randint(2, 1000000000)
        self._generator = self._ring_class(r)

    def getGenerator(self) -> T:
        return self._generator

    def power(self, base: T, b: int) -> T:
        result = self._ring_class(1)
        exp = b
        curr_base = base
        
        while exp > 0:
            if exp % 2 == 1:
                result = result * curr_base
            curr_base = curr_base * curr_base
            exp //= 2
            
        return result

class User(Generic[T]):
    def __init__(self, setup: DHSetup[T]):
        self._setup = setup
        self._secret = random.randint(1, 1000000000)
        self._key = None
        self._is_key_set = False

    def getPublicKey(self) -> T:
        return self._setup.power(self._setup.getGenerator(), self._secret)

    def setKey(self, other_public_key: T) -> None:
        self._key = self._setup.power(other_public_key, self._secret)
        self._is_key_set = True

    def encrypt(self, m: T) -> T:
        if not self._is_key_set:
            raise RuntimeError("Blad: Proba szyfrowania przed ustawieniem klucza wspolnego!")
        return m * self._key

    def decrypt(self, c: T) -> T:
        if not self._is_key_set:
            raise RuntimeError("Blad: Proba deszyfrowania przed ustawieniem klucza wspolnego!")
        return c / self._key


if __name__ == "__main__":
    try:
        p = 1234567891
        MyField = Ring[p]
        
        print("--- Inicjalizacja Diffie-Hellman ---")
        dh = DHSetup(MyField)
        print(f"Wylosowany generator ciala: {dh.getGenerator()}\n")

        alice = User(dh)
        bob = User(dh)

        pub_alice = alice.getPublicKey()
        pub_bob = bob.getPublicKey()

        print(f"Klucz publiczny Alice: {pub_alice}")
        print(f"Klucz publiczny Boba:  {pub_bob}\n")

        try:
            print("Proba szyfrowania bez klucza...")
            alice.encrypt(MyField(123))
        except Exception as e:
            print(f"Zlapano wyjatek: {e}\n")

        alice.setKey(pub_bob)
        bob.setKey(pub_alice)

        msg = MyField(987654321)
        print("--- Komunikacja (Alice -> Bob) ---")
        print(f"Wiadomosc Alice: {msg}")

        c = alice.encrypt(msg)
        print(f"Zaszyfrowana wiadomosc: {c}")

        decoded = bob.decrypt(c)
        print(f"Odkodowana przez Boba: {decoded}")

        print("\nSukces!" if msg == decoded else "\nBlad!")

    except Exception as e:
        print(f"Wyjatek: {e}", file=sys.stderr)