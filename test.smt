(declare-fun |1 Fill 10| () String)
(declare-fun |1 Fill 1| () String)
(declare-fun |1 Fill 0| () String)
(declare-fun |1 Fill 11| () String)
(declare-fun HI () String)
(declare-fun |1 Fill 5| () String)
(declare-fun |1 Fill 4| () String)
(declare-fun |1 Fill 3| () String)
(declare-fun |1 Fill 2| () String)
(declare-fun |1 Fill 8| () String)
(declare-fun |1 Fill 7| () String)

(assert (= (str.++ |1 Fill 0| (str.++ |1 Fill 1| |1 Fill 10|)) "y"))
(assert (= |1 Fill 11| "x"))
(assert (str.in.re HI (re.union (str.to.re "x") (str.to.re "y"))))

(assert (=> (str.in.re HI (re.union (str.to.re "x") (str.to.re "y")))
    (= HI (str.++ |1 Fill 0| (str.++ |1 Fill 1| |1 Fill 10|)))))
(assert (= (str.++ |1 Fill 4| |1 Fill 5|) (seq.unit #x78)))
(assert (= (not (= |1 Fill 10| (str.++ |1 Fill 4| |1 Fill 5|))) (= |1 Fill 10| "")))
(assert (let ((a!1 (= |1 Fill 11|
              (str.++ |1 Fill 2|
                      (str.++ |1 Fill 3| (str.++ |1 Fill 4| |1 Fill 5|))))))
  (or (not a!1) (= |1 Fill 10| (str.++ |1 Fill 4| |1 Fill 5|)))))
(assert (let ((a!1 (not (= |1 Fill 11|
                   (str.++ |1 Fill 2| (str.++ |1 Fill 7| |1 Fill 8|))))))
  (or a!1 (= |1 Fill 10| ""))))
(assert (let ((a!1 (= |1 Fill 11|
              (str.++ |1 Fill 2|
                      (str.++ |1 Fill 3| (str.++ |1 Fill 4| |1 Fill 5|))))))
  (= (not a!1)
     (= |1 Fill 11| (str.++ |1 Fill 2| (str.++ |1 Fill 7| |1 Fill 8|))))))
(assert (str.in.re |1 Fill 11| (re.union (str.to.re "x") (str.to.re "y"))))
(assert (str.in.re (str.++ |1 Fill 0| (str.++ |1 Fill 1| |1 Fill 10|))
           (re.union (str.to.re "x") (str.to.re "y"))))



(check-sat)
(get-model)
(get-info :reason-unknown)