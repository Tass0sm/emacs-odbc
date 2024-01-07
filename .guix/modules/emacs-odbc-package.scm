;;; Commentary:
;;
;; GNU Guix development package.  To build and install, run:
;;
;;   guix package -f guix.scm
;;
;; To use as the basis for a development environment, run:
;;
;;   guix shell -D -f guix.scm
;;
;;; Code:

(define-module (emacs-odbc-package)
  #:use-module (guix packages)
  #:use-module (guix gexp)
  #:use-module (guix git-download)
  #:use-module (guix build-system cmake)
  #:use-module (guix build-system emacs)
  #:use-module (guix utils)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (gnu packages gtk)
  #:use-module (gnu packages glib)
  #:use-module (gnu packages gettext)
  #:use-module (gnu packages cmake)
  #:use-module (gnu packages databases)
  #:use-module (gnu packages autotools)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages build-tools)
  #:use-module (gnu packages freedesktop)
  #:use-module (gnu packages lisp-check)
  #:use-module (gnu packages lisp-check)
  #:use-module (gnu packages lisp-xyz)
  #:use-module (gnu packages texinfo)
  #:use-module (gnu packages xdisorg)
  #:use-module (gnu packages wm))

(define vcs-file?
  ;; Return true if the given file is under version control.
  (or (git-predicate (string-append (current-source-directory) "/../.."))
      (const #t)))                                ;not in a Git checkout

(define-public emacs-odbc
  (package
    (name "emacs-odbc")
    (version "0.1.0")
    (source (local-file "../.." "emacs-odbc-checkout"
                        #:recursive? #t
                        #:select? vcs-file?))
    (build-system emacs-build-system)
    (arguments
     `(#:modules ((guix build emacs-build-system)
                  ((guix build cmake-build-system) #:prefix cmake:)
                  (guix build emacs-utils)
                  (guix build utils))
       #:imported-modules (,@%emacs-build-system-modules
                           (guix build cmake-build-system))
       #:phases
       (modify-phases %standard-phases
         (add-after 'unpack 'substitute-odbc-module-path
           (lambda* (#:key outputs #:allow-other-keys)
             (chmod "emacs-odbc.el" #o644)
             (emacs-substitute-sexps "emacs-odbc.el"
               ("(require 'emacs-odbc-module nil t)"
                `(module-load
                  ,(string-append (assoc-ref outputs "out")
                                  "/lib/emacs-odbc-module.so"))))))
         (add-after 'build 'configure
           ;; Run cmake.
           (lambda* (#:key outputs #:allow-other-keys)
             ((assoc-ref cmake:%standard-phases 'configure)
              #:outputs outputs
              #:out-of-source? #f)))
         (add-after 'configure 'build-and-install-module
           ;; Run make.
           (lambda* (#:key outputs #:allow-other-keys)
             ;; Compile the shared object file.
             (apply invoke (list "make" "all"))
             ;; Move the file into /lib.
             (install-file
              "emacs-odbc-module.so"
              (string-append (assoc-ref outputs "out") "/lib")))))
       #:tests? #f))
    (native-inputs
     (list cmake-minimal unixodbc))
    (home-page "https://github.com/Tass0sm/emacs-odbc")
    (synopsis "")
    (description "")
    (license license:gpl3+)))

emacs-odbc
