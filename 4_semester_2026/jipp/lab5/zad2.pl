
mergesort([], []).
mergesort([A], [A]).

mergesort([A, B | Rest], Sorted) :-
    split([A, B | Rest], L1, L2),
    mergesort(L1, S1),
    mergesort(L2, S2),
    merge_lists(S1, S2, Sorted).

split([], [], []).
split([A], [A], []).
split([A, B | Tail], [A | L1], [B | L2]) :-
    split(Tail, L1, L2).

merge_lists([], L, L).
merge_lists(L, [], L).
merge_lists([H1|T1], [H2|T2], [H1|T]) :-
    H1 =< H2, !,
    merge_lists(T1, [H2|T2], T).
merge_lists([H1|T1], [H2|T2], [H2|T]) :-
    H1 > H2,
    merge_lists([H1|T1], T2, T).


de(A, 0, 1, 0, A) :- !.
de(A, B, X, Y, Z) :-
    B > 0,
    R is A mod B,
    de(B, R, X1, Y1, Z),
    Q is A // B,
    X is Y1,
    Y is X1 - Q * Y1.


prime_factors(1, []) :- !.
prime_factors(N, X) :- 
    N > 1, 
    pf(N, 2, X).

pf(1, _, []) :- !.
pf(N, F, [F | Rest]) :-
    0 is N mod F, !,
    N1 is N // F,
    pf(N1, F, Rest).
pf(N, F, Rest) :-
    F * F =< N, !,
    next_factor(F, F1),
    pf(N, F1, Rest).
pf(N, _, [N]). 

next_factor(2, 3) :- !.
next_factor(F, F1) :- F1 is F + 2.

totient(N, T) :- 
    count_coprimes(N, N, T).

count_coprimes(_, 0, 0) :- !.
count_coprimes(N, C, T) :-
    C > 0,
    gcd(N, C, 1), !,
    C1 is C - 1,
    count_coprimes(N, C1, T1),
    T is T1 + 1.
count_coprimes(N, C, T) :-
    C > 0,
    C1 is C - 1,
    count_coprimes(N, C1, T).

gcd(A, 0, A) :- !.
gcd(A, B, G) :-
    B > 0,
    R is A mod B,
    gcd(B, R, G).


main :-
    nl,
    writeln('mergesort:'),
    write('Przed: '), writeln([5, 1, 9, 3, 7, 2, 8, 4, 6]),
    mergesort([5, 1, 9, 3, 7, 2, 8, 4, 6], Sorted),
    write('Po:    '), writeln(Sorted),

    nl,
    writeln('Rownanie diofantyczne:'),
    de(12, 18, X, Y, Z),
    format('de 12 18 -> x=~w, y=~w, nwd=~w~n', [X, Y, Z]),

    nl,
    writeln('prime_factors:'),
    prime_factors(60, PF60),
    format('prime_factors 60 = ~w~n', [PF60]),

    nl,
    writeln('totient1:'),
    totient(10, T10),
    format('totient 10 = ~w~n', [T10]).

:- initialization(main).


