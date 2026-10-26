# Autor: Jakub Retmańczyk
# Zadanie 1

# Wyznaczanie macheps dla zadanego typu zmiennopozycyjnego T (np. Float64)
function wyznacz_macheps(::Type{T}) where {T<:AbstractFloat}
    m = one(T)
    while one(T) + m / T(2) > one(T)
        m /= T(2)
    end
    return m
end

# Wyznaczanie najmniejszej dodatniej liczby maszynowej (eta) dla typu T
function wyznacz_eta(::Type{T}) where {T<:AbstractFloat}
    e = one(T)
    while e / T(2) > zero(T)
        e /= T(2)
    end
    return e
end

# Wyznaczanie największej skończonej liczby dla typu T
function wyznacz_max(::Type{T}) where {T<:AbstractFloat}
    # 1. Szukanie najwyższej potęgi dwójki
    x = one(T)
    while !isinf(x * T(2))
        x *= T(2)
    end
    # 2. Dopełnianie kolejnych bitów mantysy
    k = x / T(2)
    while k > zero(T)
        if !isinf(x + k)
            x += k
        end
        k /= T(2)
    end
    return x
end

for T in (Float16, Float32, Float64)
    println("=== Typ: ", T, " ===")
    println("macheps:  ", wyznacz_macheps(T), " | eps: ", eps(T))
    println("eta:      ", wyznacz_eta(T), " | nextfloat(0): ", nextfloat(zero(T)))
    println("floatmin: ", floatmin(T))
    println("MAX:      ", wyznacz_max(T), " | floatmax: ", floatmax(T), "\n")
end
