;;; INTERCAL operators for the GNU Emacs Calculator.
;;; Copyright (C) 1993 Jon Ferro <jferro@ece.cmu.edu>

;;; This file is not part of GNU, and it's doubtful if they 
;;; would even admit to its existence.

;;; This file is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY.  No author or distributor
;;; accepts responsibility to anyone for the consequences of using it
;;; or for whether it serves any particular purpose or works at all,
;;; unless he says so in writing.  Refer to the GNU Emacs General Public
;;; License for full details.

;;; Everyone is granted permission to copy, modify and redistribute
;;; this file, but only under the conditions described in the
;;; GNU Emacs General Public License.   A copy of this license is
;;; supposed to have been given to you along with GNU Emacs so you
;;; can know your rights and responsibilities.  It should be in a
;;; file named COPYING.  Among other things, the copyright notice
;;; and this notice must be preserved on all copies.

;; "YIX" sets the base for future operations, defaults to 2 (72-standard).
;; "YI" is prefix for binary ops $ and ~, and unary ops &, V, @, ?, and ^.
;; See the INTERCAL manual for explanation of these operators.

;; A prefix argument will cause the following operation to treat its
;; operands as large numbers, for whatever suffices as "large" in the
;; current radix.  Otherwise, the "small" version of the operation will be
;; performed if all appropriate operands fit in small variables.  Higher
;; order whirls are specified by setting the Hyperbolic flag, in which case
;; the order is the first value from the stack and the operand is second.
;; (E.g., in quinary, say, #3@500 is calculated as a 12-quint value by
;; "500 RET 3 H C-u Y I @".  No, I have no clue what that comes out to.)

(require 'calc-ext)
(require 'calc-macs)
(defun calc-Need-calc-intercal () 'shyeah-right-as-if nil)

;; Basic key (after the "third-party enhancement" prefix, "Y")
;; for Intercal commands.
(defvar calc-intercal-key "I")

;; Initial radix for Intercal calculations.
(defvar calc-intercal-radix 2)

;; Number of digits in a small number, by radix.  (Double for large.)
(defconst calc-intercal-sizes-by-radix '(0 0 16 10 8 6 6 5))
;; Name for a digit in each radix.  (Null for the default, 2).
(defconst calc-intercal-names-by-radix
  '("" "" "" "Trits " "Quarts " "Quints " "Sexts " "Septs "))
;; Other counts of the current radix size, set for the default, 2.
(defvar calc-intercal-digits 16)
(defvar calc-intercal-small-size (calc-eval "2^16" 'raw))
(defvar calc-intercal-large-size (calc-eval "2^32" 'raw))
;; A further assumption of the default radix being 2 is that
;; calc-intercal-update-radix-display is not called until the
;; radix is actually changed.


;;; Package setup

(define-key calc-mode-map (format "Y%s$" calc-intercal-key)
  'calc-intercal-mingle)
(define-key calc-mode-map (format "Y%s~" calc-intercal-key)
  'calc-intercal-select)
(define-key calc-mode-map (format "Y%s&" calc-intercal-key)
  'calc-intercal-ampersand)
(define-key calc-mode-map (format "Y%sV" calc-intercal-key)
  'calc-intercal-book)
(define-key calc-mode-map (format "Y%s@" calc-intercal-key)
  'calc-intercal-whirl)
(define-key calc-mode-map (format "Y%s?" calc-intercal-key)
  'calc-intercal-what)
(define-key calc-mode-map (format "Y%s^" calc-intercal-key)
  'calc-intercal-shark)
(define-key calc-mode-map (format "Y%sX" calc-intercal-key)
  'calc-intercal-set-radix)

(setq calc-Y-help-msgs
      (append (list (format "Intercal: %s + set-radiX, $ (mingle), ~ (select)" calc-intercal-key)
                    (format "Intercal: %s + & (and), V (or), @ (but)" calc-intercal-key)
                    (format "Intercal: %s + ^ (add-w/o-carry), ? (subtract-w/o-borrow)" calc-intercal-key))
              calc-Y-help-msgs))


;;; Wrappers for functions accessible in the calculator.

;; All of these wrappers are as identical as possible, to minimize fuss, 
;; but some of the calcFunc-intercal-* ignore their prefix argument.

(defun calc-intercal-mingle (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 2 "ick$"
                      (append '(calcFunc-intercal-mingle)
                              (calc-top-list-n 2)
                              (and n (list (prefix-numeric-value n)))))))

(defun calc-intercal-select (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 2 "ick~"
                      (append '(calcFunc-intercal-select)
                              (calc-top-list-n 2)
                              (and n (list (prefix-numeric-value n)))))))

(defun calc-intercal-ampersand (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 1 "ick&"
                      (append '(calcFunc-intercal-ampersand)
                              (calc-top-list-n 1)
                              (and n (list (prefix-numeric-value n)))))))

(defun calc-intercal-book (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 1 "ickV"
                      (append '(calcFunc-intercal-book)
                              (calc-top-list-n 1)
                              (and n (list (prefix-numeric-value n)))))))

(defun calc-intercal-whirl (n)
  (interactive "P")
  (calc-wrapper
   (let ((hyp (if (calc-is-hyperbolic) 2 1)))
     (calc-enter-result hyp "ick@"
                        (append '(calcFunc-intercal-whirl)
                                (calc-top-list-n hyp)
                                (if (calc-is-hyperbolic) nil '(1))
                                (and n (list (prefix-numeric-value n))))))))

(defun calc-intercal-what (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 1 "ick?"
                      (append '(calcFunc-intercal-what)
                              (calc-top-list-n 1)
                              (and n (list (prefix-numeric-value n)))))))

(defun calc-intercal-shark (n)
  (interactive "P")
  (calc-wrapper
   (calc-enter-result 1 "ick^"
                      (append '(calcFunc-intercal-shark)
                              (calc-top-list-n 1)
                              (and n (list (prefix-numeric-value n)))))))

;;; Radix operations

(defun calc-intercal-set-radix (n)
  (interactive "NIntercal calculation radix (2-7): ")
  (calc-wrapper
   (if (and (>= n 2) (<= n 7))
       (progn
         (setq calc-intercal-radix n)
         (setq calc-intercal-digits 
               (nth calc-intercal-radix calc-intercal-sizes-by-radix))
         (setq calc-intercal-small-size 
               (math-pow calc-intercal-radix calc-intercal-digits))
         (setq calc-intercal-large-size
               (math-pow calc-intercal-radix (* 2 calc-intercal-digits)))
         (calc-intercal-update-radix-display))
     (setq n calc-intercal-radix))
   (message "Intercal radix is %d." n)))

(defun calc-intercal-update-radix-display ()
  (let ((names calc-intercal-names-by-radix)
        (end nil))
    ;; Look for one of our modes, and replace if found.
    (while (and names (not end))
      (let ((top (member (car names) calc-other-modes)))
        (if top
            (progn
              (rplaca top (nth calc-intercal-radix
                               calc-intercal-names-by-radix))
              (setq end t))))
      (setq names (cdr names)))
    ;; If loop was exited because no radix name was found, add one.
    (or end (setq calc-other-modes
                  (cons (nth calc-intercal-radix 
                             calc-intercal-names-by-radix)
                        calc-other-modes)))))

;;; Binary operators

(defun calcFunc-intercal-mingle (a b &optional n)
  ;; Check args.
  (or (math-natnump a)
      (math-reject-arg a 'natnump))
  (or (math-natnump b)
      (math-reject-arg b 'natnump))
  (let ((r (reverse (calc-intercal-mince a)))
        (s (reverse (calc-intercal-mince b))))
    ;; Check Intercal variable sizing.
    (or (<= (length r) calc-intercal-digits)
        (math-reject-arg a calc-intercal-size-err-msg))
    (or (<= (length s) calc-intercal-digits)
        (math-reject-arg b calc-intercal-size-err-msg))
    (let ((result nil))
      ;; Alternate digits from each arg, until no more.
      (while (or r s)
        (setq result (cons (if (null s) 0 (car s)) result))
        (setq s (cdr s))
        (setq result (cons (if (null r) 0 (car r)) result))
        (setq r (cdr r)))
      (calc-intercal-liquefy result))))

(defun calcFunc-intercal-select (a b &optional n)
  ;; Check args.
  (or (math-natnump a)
      (math-reject-arg a 'natnump))
  (or (math-natnump b)
      (math-reject-arg b 'natnump))
  (let ((r (reverse (calc-intercal-mince a)))
        (s (reverse (calc-intercal-mince b))))
    ;; Check Intercal variable sizing.
    (or (<= (length r) (* 2 calc-intercal-digits))
        (math-reject-arg a calc-intercal-size-err-msg))
    (or (<= (length s) (* 2 calc-intercal-digits))
        (math-reject-arg b calc-intercal-size-err-msg))
    (let ((result nil)
          (rt r)
          (st s)
          (i (- calc-intercal-radix 1)))
      ;; Iterate downward through selector bits...
      (while (> i 0)
        (setq r rt)
        (setq s st)
        ;; ... selecting and packing down the ones that match each iteration.
        (while s
          (if (= (car s) i)
              (setq result (cons (calc-intercal-make-julienne-fries
                                  (car r) (car s) 0)
                                 result)))
          (setq s (cdr s))
          (setq r (cdr r)))
        (setq i (1- i)))
      (calc-intercal-liquefy result))))


;;; Unary operators

(defun calcFunc-intercal-whirl (a o &optional n)
  ;; Check args.
  (or (math-natnump a)
      (math-reject-arg a 'natnump))
  ;; Check intercal variable sizing.
  ;; Note that O is technically not a second arg, but rather
  ;; a specification of the ordinality of the whirl, which makes this
  ;; function capable of handling 7 different unary operations.
  (or (and (math-fixnatnump o) (<= o (- calc-intercal-radix 1)))
      (math-reject-arg o calc-intercal-arg-err-msg))
  (let ((r (calc-intercal-mince a))
        (s nil))
    (or (<= (length r) (* 2 calc-intercal-digits))
        (math-reject-arg a calc-intercal-size-err-msg))
    ;; Prepare args for merging.
    (setq r (calc-intercal-whip r n))
    (setq s (calc-intercal-puree r))
    (setq r (nreverse r))
    (setq s (nreverse s))
    (let* ((result nil))
      (while r
        (setq result (cons (calc-intercal-make-julienne-fries
                            (car r) (car s) o)
                           result))
        (setq r (cdr r))
        (setq s (cdr s)))
      (calc-intercal-liquefy result))))

(defun calcFunc-intercal-ampersand (a &optional n)
  (calcFunc-intercal-whirl a 0 n))

(defun calcFunc-intercal-book (a &optional n)
  (calcFunc-intercal-whirl a (- calc-intercal-radix 1) n))


;;; More unary operators

(defun calcFunc-intercal-shark (a &optional n)
  ;; Check args.
  (or (math-natnump a)
      (math-reject-arg a 'natnump))
  ;; Check intercal variable sizing.
  (let ((r (calc-intercal-mince a))
        (s nil))
    (or (<= (length r) (* 2 calc-intercal-digits))
        (math-reject-arg a calc-intercal-size-err-msg))
    ;; Prepare args for merging.
    (setq r (calc-intercal-whip r n))
    (setq s (calc-intercal-puree r))
    (setq r (nreverse r))
    (setq s (nreverse s))
    (let* ((result nil))
      (while r
        (setq result (cons (calcFunc-mod (+ (car s) (car r))
                                         calc-intercal-radix)
                           result))
        (setq r (cdr r))
        (setq s (cdr s)))
      (calc-intercal-liquefy result))))

(defun calcFunc-intercal-what (a &optional n)
  ;; Check args.
  (or (math-natnump a)
      (math-reject-arg a 'natnump))
  ;; Check intercal variable sizing.
  (let ((r (calc-intercal-mince a))
        (s nil))
    (or (<= (length r) (* 2 calc-intercal-digits))
        (math-reject-arg a calc-intercal-size-err-msg))
    ;; Prepare args for merging.
    (setq r (calc-intercal-whip r n))
    (setq s (calc-intercal-puree r))
    (setq r (nreverse r))
    (setq s (nreverse s))
    (let* ((result nil))
      (while r
        (setq result (cons (calcFunc-mod (- (car s) (car r))
                                         calc-intercal-radix)
                           result))
        (setq r (cdr r))
        (setq s (cdr s)))
      (calc-intercal-liquefy result))))

;;; Internals

;; Just what Intercal needs: more internal operations 
;; with gratuitous names!

(defun calc-intercal-mince (a)
  ;; Turn a calculator number into a list of digits in the current radix.
  (let ((digits nil))
    (while (not (math-zerop a))
      (let ((val (math-idivmod a calc-intercal-radix)))
        (setq digits (cons (cdr val) digits))
        (setq a (car val))))
    digits))

(defun calc-intercal-liquefy (l)
  ;; Turn a list of digits into a number for the calculator.
  (let ((a 0))
    (while l
      (setq a (math-add (car l) (math-mul a calc-intercal-radix)))
      (setq l (cdr l)))
    a))

(defun calc-intercal-whip (l d)
  ;; "Sign extend" a list of digits by padding out to the appropriate
  ;; size with extra zeros.  Extend to the small size unless the list
  ;; is already too long or the second arg D is non-nil.
  (let ((len (length l))
        (goal (nth calc-intercal-radix calc-intercal-sizes-by-radix)))
    (if (or d (> len goal))
        (setq goal (* 2 goal)))
    (while (< (length l) goal)
      (setq l (cons 0 l)))
    l))

(defun calc-intercal-puree (l)
  ;; Rotate the argument one to the right.
  (let (result)
    (while (cdr l)
      (setq result (cons (car l) result))
      (setq l (cdr l)))
    (cons (car l) (nreverse result))))    

(defun calc-intercal-make-julienne-fries (a b m)
  ;; Closest to the actual retail price without going over...
  (cond ((and (<= a m) (<= b m)) (max a b))
        ((<= a m) a)
        ((<= b m) b)
        (t (max a b))))

;; Still not used:
;; blend fold beat mix stir fluff
;; grate dice shred shave tenderize grind chop slice
;; chew swallow digest ingest masticate gnaw lick donotlick suck

;; Error messages.
(defconst calc-intercal-size-err-msg
  "533 YOU WANT MAYBE WE SHOULD IMPLEMENT 64-BIT VARIABLES?")
(defconst calc-intercal-arg-err-msg
  "997 ILLEGAL POSSESSION OF A CONTROLLED UNARY OPERATOR")
