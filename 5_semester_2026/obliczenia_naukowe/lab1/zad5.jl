# Autor: Jakub Retmańczyk
# Zadanie 5

using Plots

# Wzór wyjściowy oraz postać przekształcona bez odejmowania bliskich liczb
f(x) = sqrt(x^2 + 1.0) - 1.0
g(x) = x^2 / (sqrt(x^2 + 1.0) + 1.0)

K = 1:25
for k in K
    x = 8.0^-k
    println("k = ", lpad(k, 2), " | f(x) = ", rpad(f(x), 24), " | g(x) = ", g(x))
end

plt = plot(K, [max(f(8.0^-k), eps(Float64)) for k in K], yscale=:log10, marker=:circle,
           label="f(x)", xlabel="k (x = 8^-k)", ylabel="Wartość funkcji", legend=:bottomleft)
plot!(plt, K, [g(8.0^-k) for k in K], marker=:circle, label="g(x)")
savefig("zad5.png")
