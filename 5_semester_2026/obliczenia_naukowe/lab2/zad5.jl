# Autor: Jakub Retmańczyk
# Lista 2, zadanie 5: model logistyczny p_{n+1} = p_n + r p_n (1 - p_n)
using Printf

# logistyczny(p0, r, n, T): zwraca wektor p_0..p_n w arytmetyce T
#   p0 - populacja początkowa; r - stała wzrostu; n - liczba iteracji; T - typ
function logistyczny(p0, r, n, T)
    p = Vector{T}(undef, n + 1)
    p[1] = T(p0); rr = T(r)
    for i in 1:n
        p[i+1] = p[i] + rr * p[i] * (one(T) - p[i])
    end
    return p
end

# 1) Float32: 40 iteracji vs obcięcie po 10. iteracji do 3 miejsc po przecinku
a = logistyczny(0.01, 3, 40, Float32)
b = logistyczny(0.01, 3, 10, Float32)
b[end] = trunc(b[end], digits = 3)        # 0.722
reszta = logistyczny(b[end], 3, 30, Float32)
bb = vcat(b[1:end-1], reszta)             # 41 elementów: p0..p40
println("1) Float32: bez obcięcia vs z obcięciem po 10. iteracji (p10 = ", b[end], ")")
@printf("%3s %14s %14s\n", "n", "bez", "obcięcie")
for n in [0, 5, 10, 11, 12, 15, 20, 25, 30, 35, 40]
    @printf("%3d %14.8f %14.8f\n", n, a[n+1], bb[n+1])
end

# 2) Float32 vs Float64
f32 = logistyczny(0.01, 3, 40, Float32)
f64 = logistyczny(0.01, 3, 40, Float64)
println("\n2) Float32 vs Float64")
@printf("%3s %14s %18s %10s\n", "n", "Float32", "Float64", "|różn.|")
for n in [0, 5, 10, 15, 20, 25, 30, 35, 40]
    @printf("%3d %14.8f %18.14f %10.2e\n", n, f32[n+1], f64[n+1], abs(f32[n+1] - f64[n+1]))
end
