
function iloczyn_skalarny_w_przod(x::Vector{T}, y::Vector{T}) where {T<:AbstractFloat}
    # S  wartosc sumy częściowej
    S = zero(T)
    n = length(x)
    for i in 1:n
        S += x[i] * y[i]
    end
    return S
end

ś
function iloczyn_skalarny_w_tyl(x::Vector{T}, y::Vector{T}) where {T<:AbstractFloat}
    # S  wartosc sumy częściowej
    S = zero(T)
    n = length(x)
    for i in n:-1:1
        S += x[i] * y[i]
    end
    return S
end


function iloczyn_skalarny_od_najwiekszego(x::Vector{T}, y::Vector{T}) where {T<:AbstractFloat}
    # p  wektor iloczynów cząstkowych x_i * y_i
    p = x .* y
    
    # S_pos - suma składników dodatnich posortowanych malejąco
    S_pos = zero(T)
    for val in sort(p[p .> zero(T)], rev = true)
        S_pos += val
    end
    
    # S_neg - suma składników ujemnych posortowanych rosnąco (najbardziej ujemne najpierw)
    S_neg = zero(T)
    for val in sort(p[p .< zero(T)])
        S_neg += val
    end
    
    return S_pos + S_neg
end

function iloczyn_skalarny_od_najmniejszego(x::Vector{T}, y::Vector{T}) where {T<:AbstractFloat}
    # p - wektor iloczynów cząstkowych x_i * y_i
    p = x .* y
    
    # S_pos - suma składników dodatnich posortowanych rosnąco
    S_pos = zero(T)
    for val in sort(p[p .> zero(T)])
        S_pos += val
    end
    
    # S_neg - suma składników ujemnych posortowanych malejąco (najmniej ujemne najpierw)
    S_neg = zero(T)
    for val in sort(p[p .< zero(T)], rev = true)
        S_neg += val
    end
    
    return S_pos + S_neg
end

X_dane = [2.718281828, -3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
Y_dane = [1486.2497, 878366.9879, -22.37492, 4773714.647, 0.000185049]
dokladna_wartosc = -1.00657107000000e-11

for T in (Float32, Float64)
    x = T.(X_dane)
    y = T.(Y_dane)
    
    sa = iloczyn_skalarny_w_przod(x, y)
    sb = iloczyn_skalarny_w_tyl(x, y)
    sc = iloczyn_skalarny_od_najwiekszego(x, y)
    sd = iloczyn_skalarny_od_najmniejszego(x, y)
    
    println("a) w przód                   : ", sa, " | błąd: ", abs(Float64(sa) - dokladna_wartosc))
    println("b) w tył                     : ", sb, " | błąd: ", abs(Float64(sb) - dokladna_wartosc))
    println("c) od największego do najmn. : ", sc, " | błąd: ", abs(Float64(sc) - dokladna_wartosc))
    println("d) od najmniejszego do najw. : ", sd, " | błąd: ", abs(Float64(sd) - dokladna_wartosc))
    println()
end
println("Prawidłowa wartość odniesienia: ", dokladna_wartosc)
