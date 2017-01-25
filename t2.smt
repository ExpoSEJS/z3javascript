(declare-fun greaterThan10 (Int) Bool)

(assert (forall ((i Int)) (= (greaterThan10 i) (> i 10))))

(declare-const x (Int))
(declare-const y (Int))

(assert (greaterThan10 x))
(assert (greaterThan10 y))

(check-sat)
(get-model)