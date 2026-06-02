let string_of_int_list lst =
  "[" ^ String.concat "," (List.map string_of_int lst) ^ "]"

let rec binomial n k =
  if k = 0 || n = k then 1
  else if k > n || k < 0 then 0
  else binomial (n - 1) k + binomial (n - 1) (k - 1)

let next_row row =
  let rec aux acc lst1 lst2 =
    match lst1, lst2 with
    | [], [] -> List.rev acc
    | h1::t1, h2::t2 -> aux ((h1 + h2) :: acc) t1 t2
    | _, _ -> failwith "Nieosiągalne"
  in
  aux [] (0 :: row) (row @ [0])

let binomial2 n k =
  let rec get_row curr_n row =
    if curr_n = 0 then row
    else get_row (curr_n - 1) (next_row row)
  in
  let nth_row = get_row n [1] in
  List.nth nth_row k

let rec split_half lst =
  let rec aux acc1 acc2 toggle = function
    | [] -> (acc1, acc2)
    | h::t -> if toggle then aux (h::acc1) acc2 false t
              else aux acc1 (h::acc2) true t
  in 
  aux [] [] true lst

let rec merge l1 l2 =
  match l1, l2 with
  | [], l | l, [] -> l
  | h1::t1, h2::t2 ->
      if h1 <= h2 then h1 :: merge t1 l2
      else h2 :: merge l1 t2

let rec mergesort = function
  | [] -> []
  | [x] -> [x]
  | lst ->
      let (left, right) = split_half lst in
      merge (mergesort left) (mergesort right)

let rec de a b =
  if b = 0 then (1, 0, a)
  else
    let (x1, y1, z) = de b (a mod b) in
    (y1, x1 - (a / b) * y1, z)

let prime_factors n =
  let rec aux m f =
    if m = 1 then []
    else if m mod f = 0 then f :: aux (m / f) f
    else if f * f > m then [m]
    else aux m (f + 1)
  in 
  aux n 2

let rec gcd a b =
  if b = 0 then a else gcd b (a mod b)

let totient n =
  let rec count current acc =
    if current = 0 then acc
    else if gcd n current = 1 then count (current - 1) (acc + 1)
    else count (current - 1) acc
  in 
  count n 0

let unique_factors lst =
  let rec aux acc = function
    | [] -> List.rev acc
    | h::t -> if List.mem h acc then aux acc t else aux (h::acc) t
  in aux [] lst

let totient2 n =
  let primes = unique_factors (prime_factors n) in
  List.fold_left (fun acc p -> acc * (p - 1) / p) n primes

let primes n =
  let rec range a b =
    if a > b then [] else a :: range (a + 1) b
  in
  let rec sieve = function
    | [] -> []
    | p::xs -> p :: sieve (List.filter (fun x -> x mod p <> 0) xs)
  in
  sieve (range 2 n)

let () =
  print_endline "\nbinomial rekurencyjny:";
  Printf.printf "binomial 5 2 = %d\n" (binomial 5 2);

  print_endline "\nbinomial trojkatem paskala:";
  Printf.printf "binomial2 5 2 = %d\n" (binomial2 5 2);
  Printf.printf "binomial2 50 25 = %d\n" (binomial2 50 25);

  print_endline "\nmergesort:";
  let lst = [5; 1; 9; 3; 7; 2; 8; 4; 6] in
  Printf.printf "Przed: %s\n" (string_of_int_list lst);
  Printf.printf "Po:    %s\n" (string_of_int_list (mergesort lst));

  print_endline "\nRownanie diofantyczne:";
  let (x, y, z) = de 12 18 in
  Printf.printf "de 12 18 -> x=%d, y=%d, nwd=%d\n" x y z;

  print_endline "\nprime_factors:";
  Printf.printf "prime_factors 60 = %s\n" (string_of_int_list (prime_factors 60));

  print_endline "\ntotient1:";
  Printf.printf "totient 10 = %d\n" (totient 10);

  print_endline "\ntotient2:";
  Printf.printf "totient2 10 = %d\n" (totient2 10);
  Printf.printf "totient2 123456789 = %d\n" (totient2 123456789);

  print_endline "\nprimes:";
  Printf.printf "primes 50 = %s\n" (string_of_int_list (primes 50))