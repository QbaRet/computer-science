# Autor: Jakub Retmańczyk
# Zadanie 6

using Plots

f(x) = sin(x) + cos(3.0 * x)
dokladna = cos(1.0) - 3.0 * sin(3.0)  # wartość analityczna f'(1)

N = 0:54
hs = [2.0^-n for n in N]
przyblizenia = [(f(1.0 + h) - f(1.0)) / h for h in hs]
bledy = [abs(dokladna - p) for p in przyblizenia]

for n in N
    println("n = ", lpad(n, 2), " | h = ", rpad(hs[n+1], 12), " | 1+h = ", rpad(1.0 + hs[n+1], 20), " | błąd = ", bledy[n+1])
end

plt = plot(hs, max.(bledy, nextfloat(0.0)), xscale=:log10, yscale=:log10, marker=:circle,
           legend=false, xlabel="h = 2^-n", ylabel="|f'(1) - przybliżenie|")
savefig("zad6.png")
