# Autor: Jakub Retmańczyk
# Zadanie 4

# Dane wejściowe z treści zadania
x = [2.718281828, -3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
y = [1486.2497, 878366.9879, -22.37492, 4773714.647, 0.000185049]
dokladna = -1.00657107000000e-11  # wartość analityczna iloczynu skalarnego

for T in (Float32, Float64)
    xt, yt = T.(x), T.(y)
    println("=== ", T, " ===")

    # a) w przód
    sa = zero(T)
    for i in 1:5; sa += xt[i] * yt[i]; end
    println("a) w przód:       ", sa, " | błąd: ", abs(Float64(sa) - dokladna))

    # b) w tył
    sb = zero(T)
    for i in 5:-1:1; sb += xt[i] * yt[i]; end
    println("b) w tył:         ", sb, " | błąd: ", abs(Float64(sb) - dokladna))

    # c) od największych do najmniejszych według modułu
    p = xt .* yt
    sc = sum(sort(p[p .> 0], rev = true)) + sum(sort(p[p .< 0]))
    println("c) od największ.: ", sc, " | błąd: ", abs(Float64(sc) - dokladna))

    # d) od najmniejszych do największych według modułu
    sd = sum(sort(p[p .> 0])) + sum(sort(p[p .< 0], rev = true))
    println("d) od najmniejsz.:", sd, " | błąd: ", abs(Float64(sd) - dokladna))
    println()
end
