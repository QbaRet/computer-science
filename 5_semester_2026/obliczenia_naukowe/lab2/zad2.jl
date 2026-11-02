# Autor: Jakub Retmańczyk
# Lista 2, zadanie 2: układ Ax=b, macierze Hilberta i losowe o zadanym uwarunkowaniu
using LinearAlgebra, Random, Printf

# hilb(n): macierz Hilberta stopnia n, H[i,j] = 1/(i+j-1)
function hilb(n::Int)
    n < 1 && error("size should be > 0")
    return [1 / (i + j - 1) for i in 1:n, j in 1:n]
end

# matcond(n, c): losowa macierz n×n o wskaźniku uwarunkowania c
#   n - rozmiar (>=2); c - żądany wskaźnik uwarunkowania (>=1)
# Konstrukcja: A = U*Σ*V', U,V losowe ortogonalne, Σ - wartości osobliwe od 1 do c
function matcond(n::Int, c::Float64)
    n < 2 && error("size n should be > 1")
    c < 1.0 && error("condition number  c of a matrix should be >= 1.0")
    (U, S, V) = svd(rand(n, n))
    return U * diagm(0 => [LinRange(1.0, c, n)...]) * V'
end

# relerr(xt): błąd względny ||xt - x||/||x|| dla x = (1,...,1)
relerr(xt) = norm(xt .- 1) / norm(ones(length(xt)))

# test(A): rozwiązuje Ax=b (b=A*1) eliminacją Gaussa i przez inv(A)*b; zwraca (błąd Gaussa, błąd inv)
function test(A)
    b = A * ones(size(A, 1))
    return relerr(A \ b), relerr(inv(A) * b)
end

Random.seed!(1234)
println("== Macierze Hilberta ==")
@printf("%3s %10s %6s %12s %12s\n", "n", "cond", "rank", "Gauss", "inv")
for n in [2:2:20; 25; 30]
    A = hilb(n)
    g, i = test(A)
    @printf("%3d %10.3e %6d %12.3e %12.3e\n", n, cond(A), rank(A), g, i)
end

println("\n== Macierze losowe R_n ==")
@printf("%3s %8s %10s %6s %12s %12s\n", "n", "c", "cond", "rank", "Gauss", "inv")
for n in (5, 10, 20), c in (1.0, 10.0, 1e3, 1e7, 1e12, 1e16)
    A = matcond(n, c)
    g, i = test(A)
    @printf("%3d %8.0e %10.3e %6d %12.3e %12.3e\n", n, c, cond(A), rank(A), g, i)
end
