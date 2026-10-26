# Autor: Jakub Retmańczyk
# Zadanie 2

δ = 2.0^-52  # odległość między liczbami w [1, 2]

# Sprawdzenie równomierności siatki w przedziale [a, 2a]
for a in (0.5, 1.0, 2.0)
    krok = a * δ
    ok = true
    x = a
    for k in 1:1_000_000
        x = nextfloat(x)
        if x != a + k * krok
            ok = false
            break
        end
    end
    println("Przedział [", a, ", ", 2a, "] krok = ", krok, " -> zgodność: ", ok)
end

println("\nBity (znak | cecha | mantysa):")
for x in (0.5, nextfloat(0.5), 1.0, nextfloat(1.0), 1.0 + 2δ, 2.0, nextfloat(2.0))
    s = bitstring(x)
    println(rpad(x, 24), s[1], " | ", s[2:12], " | ", s[13:end])
end
