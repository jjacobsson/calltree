#ifndef CBRUN_H_
#define CBRUN_H_

action : exit				: 0 : false	: false	;
action : check_exit			: 1 : false	: false	;
action : getline			: 2 : false	: false	;
action : strcmp				: 3 : false	: false	: string str ;
action : print				: 4 : false	: false	: string str ;
action : act_fail			: 5 : false	: false	;
action : act_succeed		: 6 : false	: false	;
action : count_to_zero		: 7 : true	: false	: 4 : int32 start ;
action : set_gc				: 8	: false : false : 0 : int32 value ;
action : dec_gc 			: 9 : false : false : 0 ;
action : inc_gc 			: 10 : false : false : 0 ;
action : check_gc_smlr		: 11 : false : false : 0 : int32 value ;
action : check_gc_grtr		: 12 : false : false : 0 : int32 value ;
action : time_delay			: 13 : true : false : 4 : float seconds ;

decorator : modify_return	: 100 : false : true : false : false : 0 : int32 f, int32 s, int32 r ;

#endif
