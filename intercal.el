;;; intercal.el -- mode for editing INTERCAL code

;; This mode was written by Eric S. Raymond <esr@snark.thyrsus.com>
;; for the C-INTERCAL distribution, and is copyrighted by him 1992.  Free
;; redistribution encouraged.  Someday, maybe, this will be made part of GNU.

;; This mode provides abbrevs for C-INTERCAL's statements, including COME FROM.
;; These abbrevs are context-sensitive and will generate either verb or gerund
;; form as appropriate.  The keys RET, ( and * are also bound in useful ways.

;; The intercal-politesse-level adjustment purports to assist the hapless
;; INTERCAL programmer in meeting INTERCAL's Miss Manners requirement.  In
;; INTERCAL-72 and C-INTERCAL releases after 0.7, no fewer than 1/5 and no
;; more than 1/3 of the program statements must contain a PLEASE to gratify
;; the iron whim of the INTERCAL compiler; this mode assists by randomly
;; expanding some fraction of the "do" abbrevs typed to PLEASE DO or DO PLEASE.
;; The intercal-politesse-level constant is the denominator of this fraction.

;; 	$Id: intercal.el,v 1.4 1994/09/22 09:08:00 esr Exp $

(defconst intercal-politesse-level 4
  "Fraction of DOs that are automagically expanded to PLEASE DO, DO PLEASE.")

(defvar intercal-mode-map nil 
  "Keymap for INTERCAL mode.")
(if intercal-mode-map
    nil
  (setq intercal-mode-map (make-sparse-keymap))
  (define-key intercal-mode-map "\t" 'tab-to-tab-stop)
  (define-key intercal-mode-map "\r" 'intercal-return)
  (define-key intercal-mode-map "(" 'intercal-paren)
  (define-key intercal-mode-map "*" 'intercal-splat)
  (define-key intercal-mode-map "\177" 'backward-delete-char-untabify)
  )

(defvar intercal-mode-syntax-table nil
  "Syntax table in use in Intercal-mode buffers.")

(if intercal-mode-syntax-table
    nil
  (let ((table (make-syntax-table)))
    (modify-syntax-entry ?\\ "\\" table)
    (modify-syntax-entry ?+ "." table)
    (modify-syntax-entry ?- "." table)
    (modify-syntax-entry ?= "." table)
    (modify-syntax-entry ?% "." table)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)
    (modify-syntax-entry ?& "." table)
    (modify-syntax-entry ?| "." table)
    (modify-syntax-entry ?\' "\"" table)
    (setq intercal-mode-syntax-table table)))

(defvar intercal-mode-abbrev-table nil
  "*Abbrev table in use in Intercal-mode buffers.")
(if intercal-mode-abbrev-table
    nil
  (define-abbrev-table 'intercal-mode-abbrev-table ())
  (define-abbrev intercal-mode-abbrev-table "pl"  "PLEASE" nil)
  (define-abbrev intercal-mode-abbrev-table "ne"  "" 'intercal-ne-abbrev)
  (define-abbrev intercal-mode-abbrev-table "fo"  "" 'intercal-fo-abbrev)
  (define-abbrev intercal-mode-abbrev-table "res" "" 'intercal-res-abbrev)
  (define-abbrev intercal-mode-abbrev-table "st"  "" 'intercal-st-abbrev)
  (define-abbrev intercal-mode-abbrev-table "ret" "" 'intercal-ret-abbrev)
  (define-abbrev intercal-mode-abbrev-table "ig"  "" 'intercal-ig-abbrev)
  (define-abbrev intercal-mode-abbrev-table "rem" "" 'intercal-rem-abbrev)
  (define-abbrev intercal-mode-abbrev-table "ab"  "" 'intercal-ab-abbrev)
  (define-abbrev intercal-mode-abbrev-table "rei" "" 'intercal-rei-abbrev)
  (define-abbrev intercal-mode-abbrev-table "gi"  "" 'intercal-gi-abbrev)
  (define-abbrev intercal-mode-abbrev-table "rea" "" 'intercal-rea-abbrev)
  (define-abbrev intercal-mode-abbrev-table "wr"  "" 'intercal-wr-abbrev)
  (define-abbrev intercal-mode-abbrev-table "co"  "" 'intercal-co-abbrev)
  (define-abbrev intercal-mode-abbrev-table "do"  "" 'intercal-do-abbrev)
 )

(defun use-gerund ()
  (save-excursion
    (beginning-of-line)
    (or (looking-at ".*ABSTAIN") (looking-at ".*REINSTATE"))))

(defmacro make-intercal-abbrev (sym gerund verb)
  (list 'defun sym '() (list 'insert (list 'if '(use-gerund) gerund verb))))

(make-intercal-abbrev intercal-ne-abbrev "NEXTING" "NEXT")
(make-intercal-abbrev intercal-fo-abbrev "FORGETTING" "FORGET")
(make-intercal-abbrev intercal-res-abbrev "RESUMING" "RESUME")
(make-intercal-abbrev intercal-st-abbrev "STASHING" "STASH")
(make-intercal-abbrev intercal-ret-abbrev "RETRIEVING" "RETRIEVE")
(make-intercal-abbrev intercal-ig-abbrev "IGNORING" "IGNORE")
(make-intercal-abbrev intercal-rem-abbrev "REMEMBERING" "REMEMBER")
(make-intercal-abbrev intercal-ab-abbrev "ABSTAINING" "ABSTAIN FROM")
(make-intercal-abbrev intercal-rei-abbrev "REINSTATING" "REINSTATE")
(make-intercal-abbrev intercal-gi-abbrev "GIVING UP" "GIVE UP")
(make-intercal-abbrev intercal-rea-abbrev "READING IN" "READ IN")
(make-intercal-abbrev intercal-wr-abbrev "WRITING OUT" "WRITE OUT")
(make-intercal-abbrev intercal-co-abbrev "COMING FROM" "COME FROM")

(defun intercal-do-abbrev ()
  "Emit a DO (usually).  Occasionally, emit DO PLEASE or PLEASE DO."
  (insert
   (if (zerop (% (random) intercal-politesse-level))
       (if (% (random) 2) "PLEASE DO" "DO PLEASE")
     "DO")
   ))

(defun intercal-return ()
  "Insert LFD + tab, to bring us back to code-indent level."
  (interactive)
  (if (eolp) (delete-horizontal-space))
  (insert "\n")
  (tab-to-tab-stop)
  )

(defun intercal-paren ()
  "Generate an INTERCAL label."
  (interactive)
  (if (and (bolp) (looking-at "[ \t]\\|$"))
    (insert (format "(%d)"
		    (save-restriction
			       (widen)
			       (save-excursion
				 (beginning-of-line)
				 (1+ (count-lines 1 (point))))))
		    "\t")
    (insert "(")))

(defun intercal-splat ()
  "Begin an INTERCAL comment."
  (interactive)
  (insert "*")
  (forward-char -1)
  (delete-horizontal-space)
  (forward-char 1)
  (insert " ")
)

(defun intercal-mode ()
  "A major editing mode for the language Intercal.
It activates the following abbrevs (each one appropriately modified to a
gerund if it occurs on a line with ABSTAIN or REINSTATE).

ab   ABSTAIN	co   COME FROM	fo   FORGET	
gi   GIVE UP	ig   IGNORE	ne   NEXT	
rea  READ IN	rei  REINSTATE	rem  REMEMBER	
res  RESUME	ret  RETRIEVE	st   STASH	
wr   WRITE OUT	pl   PLEASE

Carriage return takes you to the first tab stop (code indent level).
Certain other single keys are bound to things which may or may not be useful.  
You may consider discovering these one of the pleasures awaiting you in your
discovery of INTERCAL's unique ambience.

Turning on Intercal mode calls the value of the variable intercal-mode-hook
with no args, if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map intercal-mode-map)
  (setq major-mode 'intercal-mode)
  (setq mode-name "Intercal")
  (setq local-abbrev-table intercal-mode-abbrev-table)
  (set-syntax-table intercal-mode-syntax-table)
  (make-local-variable 'comment-start)
  (setq comment-start "* ")
  (make-local-variable 'comment-end)
  (setq comment-end "")
  (make-local-variable 'comment-column)
  (setq comment-column 32)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (setq abbrev-mode t)
  (setq abbrev-all-caps t)
  (run-hooks 'intercal-mode-hook))

(provide 'intercal-mode)

;;; intercal.el ends here
