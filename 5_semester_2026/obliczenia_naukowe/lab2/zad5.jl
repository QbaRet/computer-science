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

# 3) odniesienie: te same iteracje w BigFloat (512 bitów), błąd Float32 i Float64
setprecision(BigFloat, 512)
N = 80                                      # liczba iteracji odniesienia
ref = logistyczny(big"0.01", 3, N, BigFloat)
d64 = logistyczny(0.01, 3, N, Float64)
d32 = logistyczny(0.01, 3, N, Float32)
println("\n3) błąd względem BigFloat")
@printf("%3s %16s %12s %12s\n", "n", "BigFloat", "błąd F32", "błąd F64")
for n in [10, 20, 30, 40, 50, 60]
    @printf("%3d %16.10f %12.2e %12.2e\n", n, Float64(ref[n+1]),
            Float64(abs(d32[n+1] - ref[n+1])), Float64(abs(d64[n+1] - ref[n+1])))
end
# pierwsze n, dla którego różnica przekracza 0.1
pierwsze(u, v) = findfirst(abs.(Float64.(u) .- Float64.(v)) .> 0.1) - 1
println("różnica > 0.1: obcięcie od n=", pierwsze(a, bb), ", Float32 vs Float64 od n=", pierwsze(f32, f64),
        ", Float64 vs BigFloat od n=", pierwsze(d64, ref))
