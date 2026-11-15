# Autor: Jakub Retmańczyk
# Lista 2, zadanie 1

x_old = [2.718281828, -3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
x_new = [2.718281828, -3.141592654, 1.414213562, 0.577215664, 0.301029995]  # obcięte x4, x5
y = [1486.2497, 878366.9879, -22.37492, 4773714.647, 0.000185049]
dokladna = -1.00657107000000e-11  # wynik z listy 1

# dot_methods(x, y, T): iloczyn skalarny w arytmetyce T, zwraca (w przód, w tył, od największych, od najmniejszych)
function dot_methods(x, y, T)
    xt, yt = T.(x), T.(y)
    p = xt .* yt
    a = zero(T); for i in 1:5;    a += p[i]; end
    b = zero(T); for i in 5:-1:1; b += p[i]; end
    c = sum(sort(p[p .> 0], rev = true)) + sum(sort(p[p .< 0]))
    d = sum(sort(p[p .> 0])) + sum(sort(p[p .< 0], rev = true))
    return (a, b, c, d)
end

for T in (Float32, Float64)
    println("=== ", T, " ===")
    r_old = dot_methods(x_old, y, T)
    r_new = dot_methods(x_new, y, T)
    for (i, nazwa) in enumerate(("w przód", "w tył", "największe->najmn.", "najmn.->największe"))
        println(rpad(nazwa, 20), " stare: ", r_old[i], "   nowe: ", r_new[i])
    end
end
setprecision(BigFloat, 256)
ex_new = sum(BigFloat(string(x_new[i])) * BigFloat(string(y[i])) for i in 1:5)
println("Dokładnie (dane zmienione, BigFloat): ", Float64(ex_new))
println("Dokładnie (dane pierwotne, lista 1):  ", dokladna)

println("Float32: x4 bez zmian? ", Float32(x_old[4]) == Float32(x_new[4]),
        ", x5 bez zmian? ", Float32(x_old[5]) == Float32(x_new[5]))
println("y4*dx4 + y5*dx5 = ", y[4] * (x_new[4] - x_old[4]) + y[5] * (x_new[5] - x_old[5]))
println("wskaźnik uwarunkowania: ", sum(abs.(x_old .* y)) / abs(dokladna))
