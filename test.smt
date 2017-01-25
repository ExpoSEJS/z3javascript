(declare-const S (String))

(declare-const capture0 (String))
(declare-const capture1 (String))

(declare-const binder0 (String))

(define-fun r1 () (RegEx String) (str.to.re "abc"))
(define-fun r0 () (RegEx String) (re.* r1))

(assert (or (str.in.re capture1 r1) (= capture1 "")))
(assert (str.in.re capture0 r0))

(assert (or (and (= capture0 (str.++ binder0 capture1)) (not (= capture1 ""))) (and (= capture0 capture1) (= capture1 ""))))

(assert (=> (str.in.re S r0) (= S capture0)))
(assert (str.in.re S r0))
(assert (= S "abcabc"))

(check-sat-using (then qe smt))
(get-model)
(get-info :reason-unknown)