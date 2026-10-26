# Autor: Jakub Retmańczyk
# Zadanie 3

# a) Dowolne x w (1, 2)
x = 1.5
while x * (1.0 / x) == 1.0
    global x = nextfloat(x)
end
println("(a) Przykładowe x = ", x, ", x * (1/x) = ", x * (1.0 / x))

# b) Najmniejsze x w (1, 2)
x_min = nextfloat(1.0)
k = 1
while x_min * (1.0 / x_min) == 1.0
    global x_min = nextfloat(x_min)
    global k += 1
end
println("(b) Najmniejsze x = ", x_min, " (krok k = ", k, "), x * (1/x) = ", x_min * (1.0 / x_min))
