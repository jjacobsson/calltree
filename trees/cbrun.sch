(define act_set_gc (lambda ()
	(define construct (lambda () 
		(print "act_set_gc construct")))
	(define execute (lambda () 
		(begin
			(print "act_set_gc execute")
			'success)))
	(define destruct (lambda () 
		(print "act_set_gc destruct")))
	(define dispatch (lambda (m)
		(cond 
			((eq? m 'construct) (construct))
			((eq? m 'execute) (execute))
			((eq? m 'destruct) (destruct)))))
	dispatch))

(define act_count_to_zero (lambda ()
	(define construct (lambda () 
		(print "act_count_to_zero construct")))

	(define execute (lambda () 
		(begin
			(print "act_count_to_zero execute")
			'success)))

	(define destruct (lambda () 
		(print "act_count_to_zero destruct")))

	(define dispatch (lambda (m)
		(cond 
			((eq? m 'execute) (execute))
			((eq? m 'destruct) (destruct)))))
	dispatch))

(define act_print (lambda ()
	(define construct (lambda () 
		(print "act_print construct")))
	(define execute (lambda () 
		(begin
			(print "act_print execute")
			'success)))
	(define destruct (lambda () 
		(print "act_print destruct")))
	(define dispatch (lambda (m)
		(cond 
			((eq? m 'construct) (construct))
			((eq? m 'execute) (execute))
			((eq? m 'destruct) (destruct)))))
	dispatch))

(define act_check_gc_grtr (lambda ()
	(define construct (lambda () 
		(print "act_check_gc_grtr construct")))
	(define execute (lambda () 
		(begin
			(print "act_check_gc_grtr execute")
			'success)))
	(define destruct (lambda () 
		(print "act_check_gc_grtr destruct")))
	(define dispatch (lambda (m)
		(cond 
			((eq? m 'construct) (construct))
			((eq? m 'execute) (execute))
			((eq? m 'destruct) (destruct)))))
	dispatch))

