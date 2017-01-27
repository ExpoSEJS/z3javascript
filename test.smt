(declare-fun HI () String)
(declare-fun |^z(x\|y)$1 Fill 0| () String)
(declare-fun |^z(x\|y)$1 Fill 1| () String)

(assert (str.in.re HI
           (re.++ (str.to.re "z") (re.union (str.to.re "x") (str.to.re "y")))))

(assert (let ((a!1 (str.in.re HI
                      (re.++ (str.to.re "z")
                             (re.union (str.to.re "x") (str.to.re "y"))))))
                           
  (=> a!1 (= HI |^z(x\|y)$1 Fill 0|))))

(assert (str.in.re |^z(x\|y)$1 Fill 1| (re.union (str.to.re "x") (str.to.re "y"))))
(assert (str.in.re |^z(x\|y)$1 Fill 0|
           (re.++ (str.to.re "z") (re.union (str.to.re "x") (str.to.re "y")))))


(check-sat-using (then qe smt))
(get-model)
(get-info :reason-unknown)