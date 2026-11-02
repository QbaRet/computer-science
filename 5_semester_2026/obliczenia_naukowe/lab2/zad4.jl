# Autor: Jakub Retmańczyk
# Lista 2, zadanie 4: wielomian Wilkinsona
# Wymaga: import Pkg; Pkg.add("Polynomials")
using Polynomials, Printf

# współczynniki P od x^20 do x^0 (z listy / wielomian.txt)
coef_desc = BigInt[1, -210, 20615, -1256850, 53327946, -1672280820, 40171771630,
    -756111184500, 11310276995381, -135585182899530, 1307535010540395,
    -10142299865511450, 63030812099294896, -311333643161390640,
    1206647803780373360, -3599979517947607200, 8037811822645051776,
    -12870931245150988800, 13803759753640704000, -8752948036761600000,
    2432902008176640000]

# p(x) = (x-1)...(x-20) w postaci iloczynowej
p_prod(x) = prod(x - k for k in 1:20)

# eksperyment(c, tytul): liczy pierwiastki P i wypisuje |P(z)|, |p(z)|, |z-k|
#   c - współczynniki (Float64) od najwyższej potęgi; tytul - opis
function eksperyment(c, tytul)
    P = Polynomial(reverse(c))   # Polynomials: rosnące potęgi
    z = roots(P)
    z = z[sortperm(real.(z))]
    println("== ", tytul, " ==")
    @printf("%3s %30s %10s %10s %10s\n", "k", "z_k", "|P(z)|", "|p(z)|", "|z-k|")
    for k in 1:20
        zk = z[k]
        @printf("%3d %30s %10.2e %10.2e %10.2e\n", k, string(round(zk, digits = 5)),
                abs(P(zk)), abs(p_prod(zk)), abs(zk - k))
    end
end

c1 = Float64.(coef_desc)
eksperyment(c1, "(a) współczynniki oryginalne")
c2 = copy(c1)
c2[2] = -210.0 - 2.0^-23        # (b) zaburzenie współczynnika przy x^19
eksperyment(c2, "(b) -210 -> -210-2^-23")
