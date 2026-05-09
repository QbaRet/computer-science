from __future__ import annotations
from typing import Optional, TypeVar, Generic, Type
import random
import math

import sys

class Ring:
    MODULUS: Optional[int] = None

    def __class_getitem__(cls, item: int) -> type:
        if not isinstance(item, int):
            raise TypeError("Modulus musi byc int")
        if item <= 0:
            raise ValueError("modulus musi byc > 0")
        cache = getattr(cls, "_param_cache", None)
        if cache is None:
            cache = {}
            setattr(cls, "_param_cache", cache)
        if item in cache:
            return cache[item]
        name = f"Ring_{item}"
        sub = type(name, (cls,), {"MODULUS": item})
        cache[item] = sub
        return sub

    def __init__(self, value: int) -> None:
        modulus = self.__class__.MODULUS
        if modulus is None:
            raise TypeError("Nalezy uzyc parametryzacji Ring[n]")
        self.modulus = modulus
        self.value = self._scale(value, self.modulus)

    @staticmethod
    def _scale(x: int, n: int) -> int:
        return ((x % n) + n) % n

    @staticmethod
    def _ext_euclid(a: int, b: int) -> tuple[int, int, int]:
        if a == 0:
            return b, 0, 1
        g, x1, y1 = Ring._ext_euclid(b % a, a)
        x = y1 - (b // a) * x1
        y = x1
        return g, x, y

    def _inverse(self, a: int) -> int:
        g, x, _ = Ring._ext_euclid(a, self.modulus)
        if g != 1:
            raise ValueError("Brak odwrotnosci")
        return self._scale(x, self.modulus)

    def _coerce_other(self, other: object) -> "Ring":
        if isinstance(other, Ring):
            if type(other) is not type(self):
                raise ValueError("Rozne typy Ring")
            return other
        if isinstance(other, int):
            return self.__class__(other)
        raise TypeError("Dozwolone: Ring o tym samym modulus albo int")

    def __add__(self, other: object) -> "Ring": return self.__class__(self.value + self._coerce_other(other).value)
    def __sub__(self, other: object) -> "Ring": return self.__class__(self.value - self._coerce_other(other).value)
    def __mul__(self, other: object) -> "Ring": return self.__class__(self.value * self._coerce_other(other).value)
    def __truediv__(self, other: object) -> "Ring":
        i = self._inverse(self._coerce_other(other).value)
        return self * i

    def __iadd__(self, other: object) -> "Ring":
        self.value = self._scale(self.value + self._coerce_other(other).value, self.modulus)
        return self
    def __isub__(self, other: object) -> "Ring":
        self.value = self._scale(self.value - self._coerce_other(other).value, self.modulus)
        return self
    def __imul__(self, other: object) -> "Ring":
        self.value = self._scale(self.value * self._coerce_other(other).value, self.modulus)
        return self
    def __itruediv__(self, other: object) -> "Ring":
        inv = self._inverse(self._coerce_other(other).value)
        self.value = self._scale(self.value * inv, self.modulus)
        return self

    def __eq__(self, other: object) -> bool: return self.value == self._coerce_other(other).value
    def __lt__(self, other: object) -> bool: return self.value < self._coerce_other(other).value
    def __le__(self, other: object) -> bool: return self.value <= self._coerce_other(other).value
    def __gt__(self, other: object) -> bool: return self.value > self._coerce_other(other).value
    def __ge__(self, other: object) -> bool: return self.value >= self._coerce_other(other).value
    def __int__(self) -> int: return int(self.value)
    def __str__(self) -> str: return str(self.value)


T = TypeVar('T', bound=Ring)

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
    p, q = 10007, 10009
    n = p * q
    
    RingN = Ring[n]
    rsa = RSA[RingN](p, q)
    
    print(f"Modulo: {rsa.getModulo()}")
    print(f"Klucz publiczny: {rsa.getPublicKey(RingN)}")
    
    m = RingN(1234567) 
    print(f"Wiadomosc: {m}")
    
    c = rsa.encrypt(m)
    print(f"Szyfrogram: {c}")
    
    decoded = rsa.decrypt(c)
    print(f"Po deszyfrowaniu: {decoded}")
    