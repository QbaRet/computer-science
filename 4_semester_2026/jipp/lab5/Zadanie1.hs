import Data.List (nub)

binomial :: Integer -> Integer -> Integer
binomial n 0 = 1
binomial n k | n == k = 1
             | k > n || k < 0 = 0
             | otherwise = binomial (n - 1) k + binomial (n - 1) (k - 1)

binomial2 :: Int -> Int -> Integer
binomial2 n k = (pascal !! n) !! k
  where
    pascal = iterate nextRow [1]
    nextRow row = zipWith (+) (0 : row) (row ++ [0])

mergesort :: Ord a => [a] -> [a]
mergesort [] = []
mergesort [x] = [x]
mergesort xs = merge (mergesort left) (mergesort right)
  where
    (left, right) = splitAt (length xs `div` 2) xs
    merge [] ys = ys
    merge xs [] = xs
    merge (x:xs') (y:ys')
      | x <= y    = x : merge xs' (y:ys')
      | otherwise = y : merge (x:xs') ys'

de :: Integer -> Integer -> (Integer, Integer, Integer)
de a 0 = (1, 0, a)
de a b = let (x1, y1, z) = de b (a `mod` b)
         in (y1, x1 - (a `div` b) * y1, z)

prime_factors :: Integer -> [Integer]
prime_factors n = factors n 2
  where
    factors 1 _ = []
    factors m f
      | m `mod` f == 0 = f : factors (m `div` f) f
      | f * f > m      = [m]
      | otherwise      = factors m (f + 1)

totient :: Integer -> Int
totient n = length [x | x <- [1..n], gcd n x == 1]

totient2 :: Integer -> Integer
totient2 n = foldl (\acc p -> acc * (p - 1) `div` p) n uniquePrimes
  where
    uniquePrimes = nub (prime_factors n)

primes :: Integer -> [Integer]
primes n = sieve [2..n]
  where
    sieve [] = []
    sieve (p:xs) = p : sieve [x | x <- xs, x `mod` p /= 0]

main :: IO ()
main = do
    
    putStrLn "\nbinomial rekurencyjny:"
    putStrLn $ "binomial 5 2 = " ++ show (binomial 5 2)
    
    putStrLn "\nbinomial trojkatem paskala:"
    putStrLn $ "binomial2 5 2 = " ++ show (binomial2 5 2)
    putStrLn $ "binomial2 50 25 = " ++ show (binomial2 50 25)
    
    putStrLn "\nmergesort:"
    let lista = [5, 1, 9, 3, 7, 2, 8, 4, 6]
    putStrLn $ "Przed: " ++ show lista
    putStrLn $ "Po:    " ++ show (mergesort lista)
    
    putStrLn "\nRównanie diofantyczne:"
    let (x, y, z) = de 12 18
    putStrLn $ "de 12 18 -> x=" ++ show x ++ ", y=" ++ show y ++ ", nwd=" ++ show z
    putStrLn $ "12x+18y: " ++ show (12*x + 18*y)
    
    putStrLn "\nprime_factors:"
    putStrLn $ "prime_factors 60 = " ++ show (prime_factors 60)
    putStrLn $ "prime_factors 123456 = " ++ show (prime_factors 123456)
    
    putStrLn "\ntotient1:"
    putStrLn $ "totient 10 = " ++ show (totient 10)
    
    putStrLn "\ntotient2:"
    putStrLn $ "totient2 10 = " ++ show (totient2 10)
    putStrLn $ "totient2 123456789 = " ++ show (totient2 123456789)
    
    putStrLn "\nprimes:"
    putStrLn $ "primes 50 = " ++ show (primes 50)