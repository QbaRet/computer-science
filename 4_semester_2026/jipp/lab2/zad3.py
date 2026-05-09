from __future__ import annotations

from typing import Optional


class Ring:

    _modulo: Optional[int] = None

    def __class_getitem__(cls, item: int) -> type:
        if not isinstance(item, int):
            raise TypeError("Modulo musi byc int")
        if item <= 0:
            raise ValueError("modulo musi byc > 0")
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


if __name__ == "__main__":
    first = Ring[7](10)
    second = Ring[7](5)
    print("Ring[7]")
    print(f"Start: a={first}, b={second}")

    result = first + second
    print(f"Dodawanie: {first} + {second} = {result}")

    result = first - second
    print(f"Odejmowanie: {first} - {second} = {result}")

    result = first * second
    print(f"Mnozenie: {first} * {second} = {result}")

    result = first / Ring[7](2)
    print(f"Dzielenie: {first} / 2 = {result}")

    first += second
    print(f"Po +=: {first}")

    try:
        result = Ring[7](1) / Ring[7](0)
        print("Dzielenie przez 0")
    except ValueError:
        print("Nie mozna dzielic przez 0")

