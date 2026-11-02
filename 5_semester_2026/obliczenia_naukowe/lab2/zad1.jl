# Autor: Jakub Retmańczyk
# Lista 2, zadanie 1: iloczyn skalarny z lab1/zad4, ale z obciętymi x4 i x5

x_old = [2.718281828, -3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
x_new = [2.718281828, -3.141592654, 1.414213562, 0.577215664, 0.301029995]  # bez ostatniej 9 w x4 i 7 w x5
y = [1486.2497, 878366.9879, -22.37492, 4773714.647, 0.000185049]
dokladna = -1.00657107000000e-11  # wartość z listy 1 (dla pierwotnych danych)

# dot_methods(x, y, T): sumuje x[i]*y[i] w arytmetyce T czterema metodami
#   x, y - wektory danych; T - typ zmiennoprzecinkowy (Float32/Float64)
# zwraca krotkę (w przód, w tył, od największych, od najmniejszych)
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
# dokładna wartość dla zmienionych danych (BigFloat)
setprecision(BigFloat, 256)
ex_new = sum(BigFloat(string(x_new[i])) * BigFloat(string(y[i])) for i in 1:5)
println("Dokładnie (dane zmienione, BigFloat): ", Float64(ex_new))
println("Dokładnie (dane pierwotne, lista 1):  ", dokladna)
