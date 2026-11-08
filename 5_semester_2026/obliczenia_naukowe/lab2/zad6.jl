# Autor: Jakub Retmańczyk
# Lista 2, zadanie 6: x_{n+1} = x_n^2 + c w Float64, 40 iteracji
using Printf

# iteruj(x0, c, n, T): zwraca x_0..x_n dla x_{k+1} = x_k^2 + c w arytmetyce T
#   x0 - wartość początkowa; c - stała; n - liczba iteracji; T - typ (domyślnie Float64)
function iteruj(x0, c, n, T = Float64)
    x = Vector{T}(undef, n + 1)
    x[1] = x0
    for k in 1:n
        x[k+1] = x[k]^2 + c
    end
    return x
end

przyp = [(-2.0, 1.0), (-2.0, 2.0), (-2.0, 1.99999999999999),
         (-1.0, 1.0), (-1.0, -1.0), (-1.0, 0.75), (-1.0, 0.25)]

for (i, (c, x0)) in enumerate(przyp)
    x = iteruj(x0, c, 40)
    println("$i) c=$c, x0=$x0")
    println("   n=0..5:   ", join((@sprintf("%.6g", v) for v in x[1:6]), ", "))
    println("   n=36..40: ", join((@sprintf("%.6g", v) for v in x[37:41]), ", "))
end

# wykaz iteracji dla 3 przypadku: rozbieganie od punktu stałego 2
x = iteruj(1.99999999999999, -2.0, 40)
println("\nprzypadek 3, kolejne n:")
for n in 0:2:40; @printf("n=%2d  %.10g\n", n, x[n+1]); end

# przypadek 3 w BigFloat z tym samym x0: czy to, co widać, to zaokrąglenia?
setprecision(BigFloat, 512)
xb = iteruj(BigFloat(1.99999999999999), BigFloat(-2), 40, BigFloat)
println("\nprzypadek 3, Float64 vs BigFloat:")
for n in (20, 30, 40)
    @printf("n=%2d  Float64 %.6f  BigFloat %.6f  różnica %.1e\n", n, x[n+1], Float64(xb[n+1]), Float64(abs(x[n+1] - xb[n+1])))
end

# przypadki 6 i 7: kiedy Float64 trafia dokładnie w cykl {0, -1}
for x0 in (0.75, 0.25)
    n = findfirst(v -> v == 0.0 || v == -1.0, iteruj(x0, -1.0, 40)) - 1
    println("x0=", x0, ": pierwsza wartość dokładnie w {0,-1} dla n=", n)
end

# iteracja graficzna (pajęczyna) -> zad6_cobweb.png
# Wymaga: import Pkg; Pkg.add("Plots")
using Plots
# cobweb(x0, c, n): zwraca ścieżkę punktów iteracji graficznej
function cobweb(x0, c, n)
    xs = [x0]; ys = [0.0]; x = x0
    for _ in 1:n
        y = x^2 + c
        push!(xs, x); push!(ys, y)
        push!(xs, y); push!(ys, y)
        x = y
    end
    return xs, ys
end
# punkty stałe x = x^2 + c, czyli x = (1 ± sqrt(1 - 4c)) / 2
punkty_stale(c) = [(1 - sqrt(1 - 4c)) / 2, (1 + sqrt(1 - 4c)) / 2]

wybrane = [2, 3, 6, 7]   # przypadki na rysunku; 1, 4 i 5 widać w tabeli
plots = []
for i in wybrane
    c, x0 = przyp[i]
    t = range(-2.2, 2.2, length = 400)
    p = plot(t, t .^ 2 .+ c, label = "x² + c", color = :steelblue, lw = 1.5,
             xlims = (-2.2, 2.2), ylims = (-2.5, 2.5), aspect_ratio = :equal,
             title = "przypadek $i: c = $(Int(c)), x₀ = $x0", titlefontsize = 10,
             tickfontsize = 8, legend = i == 7 ? :bottomright : false)
    plot!(p, t, t, label = "y = x", color = :black, ls = :dash)
    xs, ys = cobweb(x0, c, 30)
    plot!(p, xs, ys, label = "iteracja", color = :red, lw = 0.8)
    ps = punkty_stale(c)
    scatter!(p, ps, ps, label = "punkty stałe", color = :black, ms = 4)
    push!(plots, p)
end
savefig(plot(plots..., layout = (2, 2), size = (800, 800)), joinpath(@__DIR__, "zad6_cobweb.png"))

# przebieg ciągu x_n w zależności od n -> zad6_przebieg.png
ns = 0:40   # numery iteracji
p23 = plot(ns, iteruj(2.0, -2.0, 40), label = "przypadek 2", color = :black, ls = :dash, lw = 1.5,
           title = "przypadki 2 i 3: c = -2", titlefontsize = 10, xlabel = "n", ylabel = "xₙ", ylims = (-2.2, 2.2),
           legend = :bottomleft)
plot!(p23, ns, iteruj(1.99999999999999, -2.0, 40), label = "przypadek 3", color = :red,
      marker = :circle, ms = 2.5, lw = 1)
p6 = plot(ns, iteruj(0.75, -1.0, 40), label = false, color = :steelblue, marker = :circle, ms = 2.5,
          title = "przypadek 6: c = -1, x₀ = 0.75", titlefontsize = 10, xlabel = "n", ylims = (-1.1, 0.9))
p7 = plot(ns, iteruj(0.25, -1.0, 40), label = false, color = :steelblue, marker = :circle, ms = 2.5,
          title = "przypadek 7: c = -1, x₀ = 0.25", titlefontsize = 10, xlabel = "n", ylims = (-1.1, 0.9))
savefig(plot(p23, p6, p7, layout = (1, 3), size = (1050, 330), left_margin = 4Plots.mm,
             bottom_margin = 5Plots.mm), joinpath(@__DIR__, "zad6_przebieg.png"))
