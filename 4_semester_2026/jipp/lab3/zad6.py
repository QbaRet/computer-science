from __future__ import annotations

import random
from typing import Optional, Generic, TypeVar

class Ring:

    _modulo: Optional[int] = None

    def __class_getitem__(cls, item: int) -> type:
        if not isinstance(item, int):
            raise TypeError("Modulus musi byc int")
        if item <= 0:
            raise ValueError("modulus musi byc > 0")
        cache = getattr(cls, "_type_cache", None)
        if cache is None:
            cache = {}
            setattr(cls, "_type_cache", cache)
        if item in cache:
            return cache[item]
        type_name = f"RingMod{item}"
        ring_type = type(type_name, (cls,), {"_modulo": item})
        cache[item] = ring_type
        return ring_type

    def __init__(self, number: int) -> None:
        modulo = self.__class__._modulo
        if modulo is None:
            raise TypeError("Nalezy uzyc parametryzacji Ring[n]")

        self._modulo_value = modulo
        self._value = self._normalize(number, self._modulo_value)

    @staticmethod
    def _normalize(value: int, modulo: int) -> int:
        return ((value % modulo) + modulo) % modulo

    @staticmethod
    def _extended_gcd(left: int, right: int) -> tuple[int, int, int]:
        if left == 0:
            return right, 0, 1
        gcd_value, x_prev, y_prev = Ring._extended_gcd(right % left, left)
        x_value = y_prev - (right // left) * x_prev
        y_value = x_prev
        return gcd_value, x_value, y_value

    def _inverse(self, value: int) -> int:
        gcd_value, coefficient, _ = Ring._extended_gcd(value, self._modulo_value)
        if gcd_value != 1:
            raise ValueError("Brak odwrotnosci")
        return self._normalize(coefficient, self._modulo_value)

    def _ensure_same_ring(self, other: object) -> "Ring":
        if isinstance(other, Ring):
            if type(other) is not type(self):
                raise ValueError("Rozne typy Ring")
            return other
        if isinstance(other, int):
            return self.__class__(other)
        raise TypeError("Dozwolone sa tylko Ring z tym samym modulus albo int")

    def __add__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        return self.__class__(self._value + other_ring._value)

    def __sub__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        return self.__class__(self._value - other_ring._value)

    def __mul__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        return self.__class__(self._value * other_ring._value)

    def __truediv__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        inverse_value = self._inverse(other_ring._value)
        return self * inverse_value

    def __iadd__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        self._value = self._normalize(self._value + other_ring._value, self._modulo_value)
        return self

    def __isub__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        self._value = self._normalize(self._value - other_ring._value, self._modulo_value)
        return self

    def __imul__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        self._value = self._normalize(self._value * other_ring._value, self._modulo_value)
        return self

    def __itruediv__(self, other: object) -> "Ring":
        other_ring = self._ensure_same_ring(other)
        inverse_value = self._inverse(other_ring._value)
        self._value = self._normalize(self._value * inverse_value, self._modulo_value)
        return self

    def __eq__(self, other: object) -> bool:
        other_ring = self._ensure_same_ring(other)
        return self._value == other_ring._value

    def __lt__(self, other: object) -> bool:
        other_ring = self._ensure_same_ring(other)
        return self._value < other_ring._value

    def __le__(self, other: object) -> bool:
        other_ring = self._ensure_same_ring(other)
        return self._value <= other_ring._value

    def __gt__(self, other: object) -> bool:
        other_ring = self._ensure_same_ring(other)
        return self._value > other_ring._value

    def __ge__(self, other: object) -> bool:
        other_ring = self._ensure_same_ring(other)
        return self._value >= other_ring._value

    def __int__(self) -> int:
        return int(self._value)

    def __str__(self) -> str:
        return str(self._value)

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self._value})"


T = TypeVar('T', bound=Ring)

class DHSetup(Generic[T]):
    def __init__(self, field_type: type[T]):
        r = random.randint(2, 1000000000)
        self._generator: T = field_type(r)
        
    def getGenerator(self) -> T:
        return self._generator

    def power(self, base: T, b: int) -> T:
        result: T = type(base)(1) 
        current_base = base
        
        while b > 0:
            if b % 2 == 1:
                result = result * current_base
            current_base = current_base * current_base
            b //= 2
            
        return result

class User(Generic[T]):
    def __init__(self, setup: DHSetup[T]):
        self._setup = setup
        self._secret = random.randint(1, 1000000000)
        self._key: Optional[T] = None

    def getPublicKey(self) -> T:
        generator = self._setup.getGenerator()
        return self._setup.power(generator, self._secret)

    def setKey(self, other_public_key: T) -> None:
        self._key = self._setup.power(other_public_key, self._secret)

    def encrypt(self, m: T) -> T:
        if self._key is None:
            raise RuntimeError("Blad: Proba szyfrowania przed ustawieniem klucza wspolnego")
        return m * self._key

    def decrypt(self, c: T) -> T:
        if self._key is None:
            raise RuntimeError("Blad: Proba deszyfrowania przed ustawieniem klucza wspolnego")
        return c / self._key

if __name__ == "__main__":
    p = 1234567891
    MyField = Ring[p]

    try:

        dh = DHSetup(MyField)
        print(f"Generator: {dh.getGenerator()}")

        user_A = User(dh)
        user_B = User(dh)

        pub_A = user_A.getPublicKey()
        pub_B = user_B.getPublicKey()

        print(f"Klucz publiczny A: {pub_A}")
        print(f"Klucz publiczny B:   {pub_B}")

        user_A.setKey(pub_B)
        user_B.setKey(pub_A)

        message = MyField(987654321)
        print(f"Wiadomosc: {message}")

        cipher = user_A.encrypt(message)
        print(f"Szyfrogram: {cipher}")

        decoded = user_B.decrypt(cipher)
        print(f"Po deszyfrowaniu: {decoded}")
        
    except Exception as e:
        print(f"Wyjatek: {e}")