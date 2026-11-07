# Autor: Jakub Retmańczyk
# Lista 2, zadanie 3: f(x) = e^x * ln(1 + e^-x); granica przy x->∞ to 1
# Wykres w Julii (Plots/GR) tutaj; drugi program: zad3.py (matplotlib).
# Wymaga: import Pkg; Pkg.add("Plots")
using Plots

# f(x): naiwny wzór w Float64
f(x) = exp(x) * log(1 + exp(-x))
# fstable(x): wersja stabilna, e^x*log1p(e^-x) (log1p liczy ln(1+t) dokładnie dla małych t)
fstable(x) = exp(x) * log1p(exp(-x))

xs = 0:0.05:60
plt = plot(xs, f.(xs), label = "naiwnie: exp(x)*log(1+exp(-x))", lw = 2,
           xlabel = "x", ylabel = "f(x)", title = "f(x) = exp(x) * ln(1 + exp(-x))",
           legend = :bottomleft)
plot!(plt, xs, fstable.(xs), label = "stabilnie: log1p", lw = 2, ls = :dash)
hline!(plt, [1.0], label = "granica = 1", color = :black, ls = :dot)
savefig(plt, joinpath(@__DIR__, "zad3_julia.png"))

for x in (10, 20, 30, 36, 36.7, 37, 38, 40, 50, 100)
    println("x=", x, "  naiwnie=", f(Float64(x)), "  log1p=", fstable(Float64(x)))
end
# Dla x>~36.7 e^-x < macheps/2, więc 1+e^-x == 1, log(1)=0 i f=0.
println("1+exp(-37) == 1 ? ", 1 + exp(-37.0) == 1.0)

# zakres wartości wzoru naiwnego tuż przed spadkiem do zera
println("max f na [30, 37]: ", maximum(f.(30:0.001:37)))
println("min f > 0 na [30, 37]: ", minimum(filter(>(0), f.(30:0.001:37))))
# dla bardzo dużych x psuje się też log1p: exp(x) = Inf
println("x=710: log1p -> ", fstable(710.0), ";  x=746: log1p -> ", fstable(746.0))
