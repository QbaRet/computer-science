(defun binomial (n k)
  (cond ((= k 0) 1)
        ((= n k) 1)
        ((or (> k n) (< k 0)) 0)
        (t (+ (binomial (- n 1) k)
              (binomial (- n 1) (- k 1))))))

(defun next-row (row)
  (mapcar #'+ (append '(0) row) (append row '(0))))

(defun pascal-row (n current-row)
  (if (= n 0)
      current-row
      (pascal-row (- n 1) (next-row current-row))))

(defun binomial2 (n k)
  (nth k (pascal-row n '(1))))

(defun split-half (lst)
  (let ((mid (floor (length lst) 2)))
    (list (subseq lst 0 mid) (subseq lst mid))))

(defun merge-lists (l1 l2)
  (cond ((null l1) l2)
        ((null l2) l1)
        ((<= (car l1) (car l2))
         (cons (car l1) (merge-lists (cdr l1) l2)))
        (t (cons (car l2) (merge-lists l1 (cdr l2))))))

(defun mergesort (lst)
  (if (or (null lst) (null (cdr lst)))
      lst
      (let* ((halves (split-half lst))
             (left (car halves))
             (right (cadr halves)))
        (merge-lists (mergesort left) (mergesort right)))))

(defun de (a b)
  (if (= b 0)
      (list 1 0 a)
      (let* ((res (de b (mod a b)))
             (x1 (car res))
             (y1 (cadr res))
             (z (caddr res)))
        (list y1 (- x1 (* (floor a b) y1)) z))))

(defun get-factors (m f)
  (cond ((= m 1) nil)
        ((= (mod m f) 0) (cons f (get-factors (floor m f) f)))
        ((> (* f f) m) (list m))
        (t (get-factors m (+ f 1)))))

(defun prime-factors (n)
  (get-factors n 2))

(defun count-coprimes (n current)
  (cond ((= current 0) 0)
        ((= (gcd n current) 1) (1+ (count-coprimes n (- current 1))))
        (t (count-coprimes n (- current 1)))))

(defun totient (n)
  (count-coprimes n n))

(defun unique (lst)
  (cond ((null lst) nil)
        ((member (car lst) (cdr lst)) (unique (cdr lst)))
        (t (cons (car lst) (unique (cdr lst))))))

(defun totient2-helper (acc primes)
  (if (null primes)
      acc
      (totient2-helper (floor (* acc (- (car primes) 1)) (car primes))
                       (cdr primes))))

(defun totient2 (n)
  (totient2-helper n (unique (prime-factors n))))

(defun range (start end)
  (if (> start end)
      nil
      (cons start (range (1+ start) end))))

(defun filter-multiples (p lst)
  (cond ((null lst) nil)
        ((= (mod (car lst) p) 0) (filter-multiples p (cdr lst)))
        (t (cons (car lst) (filter-multiples p (cdr lst))))))

(defun sieve (lst)
  (if (null lst)
      nil
      (cons (car lst) (sieve (filter-multiples (car lst) (cdr lst))))))

(defun primes (n)
  (sieve (range 2 n)))

(defun list->string (lst)
  (format nil "[~{~a~^,~}]" lst))

(defun main ()
  (format t "~%binomial rekurencyjny:~%")
  (format t "binomial 5 2 = ~a~%" (binomial 5 2))

  (format t "~%binomial trojkatem paskala:~%")
  (format t "binomial2 5 2 = ~a~%" (binomial2 5 2))
  (format t "binomial2 50 25 = ~a~%" (binomial2 50 25))

  (format t "~%mergesort:~%")
  (format t "Przed: ~a~%" (list->string '(5 1 9 3 7 2 8 4 6)))
  (format t "Po:    ~a~%" (list->string (mergesort '(5 1 9 3 7 2 8 4 6))))

  (format t "~%Rownanie diofantyczne:~%")
  (let* ((res (de 12 18))
         (x (first res))
         (y (second res))
         (z (third res)))
    (format t "de 12 18 -> x=~a, y=~a, nwd=~a~%" x y z))

  (format t "~%prime_factors:~%")
  (format t "prime_factors 60 = ~a~%" (list->string (prime_factors 60)))

  (format t "~%totient1:~%")
  (format t "totient 10 = ~a~%" (totient 10))

  (format t "~%totient2:~%")
  (format t "totient2 10 = ~a~%" (totient2 10))
  (format t "totient2 123456789 = ~a~%" (totient2 123456789))

  (format t "~%primes:~%")
  (format t "primes 50 = ~a~%" (list->string (primes 50))))

(main)