# Autor: Jakub Retmańczyk
# Lista 2, zadanie 4
using Polynomials, Printf

# współczynniki P od x^20 do x^0
coef_desc = BigInt[1, -210, 20615, -1256850, 53327946, -1672280820, 40171771630,
    -756111184500, 11310276995381, -135585182899530, 1307535010540395,
    -10142299865511450, 63030812099294896, -311333643161390640,
    1206647803780373360, -3599979517947607200, 8037811822645051776,
    -12870931245150988800, 13803759753640704000, -8752948036761600000,
    2432902008176640000]

# p(x) = (x-1)...(x-20) w postaci iloczynowej
p_prod(x) = prod(x - k for k in 1:20)

# eksperyment(c, tytul): pierwiastki P o współczynnikach c (od x^20) i ich błędy
function eksperyment(c, tytul)
    P = Polynomial(reverse(c))
    z = roots(P)
    z = z[sortperm(real.(z))]
    println("== ", tytul, " ==")
    @printf("%3s %30s %10s %10s %10s\n", "k", "z_k", "|P(z)|", "|p(z)|", "|z-k|")
    for k in 1:20
        zk = z[k]
        @printf("%3d %30s %10.2e %10.2e %10.2e\n", k, string(round(zk, digits = 5)),
                abs(P(zk)), abs(p_prod(zk)), abs(zk - k))
    end
    return z
end

c1 = Float64.(coef_desc)
z1 = eksperyment(c1, "(a) współczynniki oryginalne")
c2 = copy(c1)
c2[2] = -210.0 - 2.0^-23
eksperyment(c2, "(b) -210 -> -210-2^-23")

setprecision(BigFloat, 512)

# horner(c, x): P(x) dla współczynników c (od najwyższej potęgi)
horner(c, x) = foldl((s, a) -> s * x + a, c; init = zero(x))
# pochodna(c, x): P'(x)
function pochodna(c, x)
    n = length(c) - 1
    return foldl((s, i) -> s * x + c[i] * (n - i + 1), 1:n; init = zero(x))
end

bledy = [abs(BigInt(c1[i]) - coef_desc[i]) for i in 1:21]   # błędy zapisu współczynników
println("\nBłędy zapisu współczynników w Float64: ", filter(>(0), bledy))

c_fl = BigFloat.(c1)  # współczynniki zaokrąglone do Float64
c_ex = BigFloat.(coef_desc) # współczynniki dokładne
@printf("%3s %10s %10s %12s %14s\n", "k", "|z-k|", "|r-k|", "|P(z)| dokł.", "|p'(k)|*|z-k|")
for k in 1:20
    r = BigFloat(k)  # pierwiastek P z zaokrąglonymi współczynnikami
    for _ in 1:100
        r -= horner(c_fl, r) / pochodna(c_fl, r)
    end
    zk = real(z1[k])
    dp = abs(prod(BigFloat(k - j) for j in 1:20 if j != k))   # |p'(k)|
    @printf("%3d %10.2e %10.2e %12.2e %14.2e\n", k, abs(zk - k), Float64(abs(r - k)),
            Float64(abs(horner(c_ex, BigFloat(zk)))), Float64(dp * abs(zk - k)))
end
