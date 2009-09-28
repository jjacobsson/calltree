#ifndef CBRUN_H_
#define CBRUN_H_

action : exit				: id=0 ;
action : check_exit			: id=1 ;
action : getline			: id=2 ;
action : strcmp				: id=3 : string str ;
action : print				: id=4 : string str ;
action : act_fail			: id=5 ;
action : act_succeed		: id=6 ;
action : count_to_zero		: id=7,construct=true,bss=4 : int32 start ;
action : set_gc				: id=8 : int32 value ;
action : dec_gc 			: id=9 ;
action : inc_gc 			: id=10 ;
action : check_gc_smlr		: id=11 : int32 value ;
action : check_gc_grtr		: id=12 : int32 value ;
action : time_delay			: id=13,construct=true,bss=4 : float seconds ;

decorator : modify_return	: id=100,modify=true : int32 f, int32 s, int32 r ;

#endif
