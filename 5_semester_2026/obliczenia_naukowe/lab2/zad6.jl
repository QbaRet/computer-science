# Autor: Jakub Retmańczyk
# Lista 2, zadanie 6: x_{n+1} = x_n^2 + c w Float64, 40 iteracji
using Printf

# iteruj(x0, c, n): zwraca x_0..x_n dla x_{k+1} = x_k^2 + c (Float64)
#   x0 - wartość początkowa; c - stała; n - liczba iteracji
function iteruj(x0, c, n)
    x = Vector{Float64}(undef, n + 1)
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

# iteracja graficzna (pajęczyna) -> zad6_cobweb.png
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
plots = []
for (i, (c, x0)) in enumerate(przyp)
    t = range(-2.2, 2.2, length = 400)
    p = plot(t, t .^ 2 .+ c, label = "x²+$c", ylims = (-3, 3), xlims = (-2.2, 2.2),
             title = "$i) c=$c, x0=$x0", legend = false, titlefontsize = 8)
    plot!(p, t, t, color = :black, ls = :dash)
    xs, ys = cobweb(x0, c, i == 3 ? 40 : 15)
    plot!(p, xs, ys, color = :red, lw = 1)
    push!(plots, p)
end
savefig(plot(plots..., layout = (3, 3), size = (900, 800)), joinpath(@__DIR__, "zad6_cobweb.png"))
