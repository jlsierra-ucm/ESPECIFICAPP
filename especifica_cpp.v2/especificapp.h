#include<functional>

#pragma once


  namespace _epp {

	  typedef enum {FORALL,EXISTS} tQPred;

	  template<class T, tQPred qpred>
	  bool do_qpred(T* vars[],
		  int nvars,
		  std::function<T()> init,
		  std::function<T()> end,
		  std::function<bool()> filter,
		  std::function<bool()> pred,
		  int nvar) {
		  if (nvar == nvars) {
			  if (qpred == FORALL) {
				  return !filter() || pred();
			  }
			  else {
				  return filter() && pred();
			  }
		  }
		  else {
			  for (T i = init(); i <= end(); i++) {
				  *(vars[nvar]) = i;
				   if (qpred == FORALL) {
					   if (!do_qpred<T, qpred>(vars, nvars, init, end, filter, pred, nvar + 1)) {
						  return false;
					   }
				   }
				  else {
					   if (do_qpred<T, qpred>(vars, nvars, init, end, filter, pred, nvar + 1)) {
						   return true;
					   }
				  }
				}
			  }
			  if (qpred == FORALL) {
				  return true;
			  }
			  else {
				  return false;
			  }
	  }

	  template<class T, tQPred qpred, class ... tPack>
	  bool do_qpred_launcher(std::function<T()> init,
		  std::function<T()> end,
		  std::function<bool()> filter,
		  std::function<bool()> pred,
		  tPack& ... vs) {
		  T* vars[] = { &vs ... };
		  int nvars = sizeof(vars) / sizeof(vars[0]);
		  return do_qpred<T,qpred>(vars, nvars, init, end, filter, pred, 0);
	  }


	  typedef enum { SUM, PROD, MAX, MIN } tQAExp;


	  template<class T, class TR, tQAExp qaexp>
	  void do_qaexp(T* vars[],
		  int nvars,
		  std::function<T()> init,
		  std::function<T()> end,
		  std::function<bool()> filter,
		  std::function<TR()> val,
		  int nvar,
		  TR& resul) {
		  if (nvar == nvars) {
			  if (filter()) {
				  switch (qaexp) {
				  case SUM:  resul += val(); break;
				  case PROD: resul *= val(); break;
				  case MAX: {T v = val(); resul = v > resul ? v : resul; }; break;
				  case MIN: {T v = val(); resul = v < resul ? v : resul; }; break;
				  }
			  }
		  }
		  else {
			  for (T i = init(); i <= end(); i++) {
				  *(vars[nvar]) = i;
				  do_qaexp<T,TR,qaexp>(vars, nvars, init, end, filter, val, nvar + 1, resul);
			  }
		  }
	  }

	  template<class T, class TR, tQAExp qaexp, class ... tPack>
	  TR do_qaexp_launcher(std::function<T()> init,
		  std::function<T()> end,
		  std::function<bool()> filter,
		  std::function<TR()> val,
		  tPack& ... vs) {
		  T* vars[] = { &vs ... };
		  int nvars = sizeof(vars) / sizeof(vars[0]);
		  TR resul;
		  if (qaexp == SUM) {
			  resul = TR(0);
		  }
		  else {
			  resul = TR(1);
		  }
		  do_qaexp<T,T,qaexp>(vars, nvars, init, end, filter, val, 0, resul);
		  return resul;
	  }

		class EEmptyRange {};

		template<class T, class TR>
		bool find_value(T* vars[],
			int nvars,
			std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<TR()> val,
			int nvar,
			TR& resul) {
			if (nvar == nvars) {
				if (filter()) {
					resul = val();
					return true;
				}
				else {
					return false;
				}
			}
			else {
				for (T i = init(); i <= end(); i++) {
					*(vars[nvar]) = i;
					if (find_value(vars, nvars, init, end, filter, val, nvar + 1, resul)) return true;
				}
				return false;
			}
		}

		template<class T, class TR>
		void do_max(T* vars[],
			int nvars,
			std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<TR()> val,
			int nvar,
			TR& resul) {
			if (nvar == nvars) {
				if (filter()) {
					TR v = val();
					if (v > resul) resul = v;
				}
			}
			else {
				for (T i = init(); i <= end(); i++) {
					*(vars[nvar]) = i;
					do_max(vars, nvars, init, end, filter, val, nvar + 1, resul);
				}
			}
		}

		template<class T, class TR, class ... tPack>
		TR do_max_launcher(std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<TR()> val,
			tPack& ... vs) {
			T* vars[] = { &vs ... };
			int nvars = sizeof(vars) / sizeof(vars[0]);
			TR resul;
			if (! find_value(vars, nvars, init, end, filter, val, 0, resul)) throw EEmptyRange();
			do_max(vars, nvars, init, end, filter, val, 0, resul);
			return resul;
		}

		template<class T, class TR>
		void do_min(T* vars[],
			int nvars,
			std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<TR()> val,
			int nvar,
			TR& resul) {
			if (nvar == nvars) {
				if (filter()) {
					TR v = val();
					if (v < resul) resul = v;
				}
			}
			else {
				for (T i = init(); i <= end(); i++) {
					*(vars[nvar]) = i;
					do_min(vars, nvars, init, end, filter, val, nvar + 1, resul);
				}
			}
		}

		template<class T, class TR, class ... tPack>
		TR do_min_launcher(std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<TR()> val,
			tPack& ... vs) {
			T* vars[] = { &vs ... };
			int nvars = sizeof(vars) / sizeof(vars[0]);
			TR resul;
			if (!find_value(vars, nvars, init, end, filter, val, 0, resul)) throw EEmptyRange();
			do_min(vars, nvars, init, end, filter, val, 0, resul);
			return resul;
		}


		template<class T>
		void do_count(T* vars[],
			int nvars,
			std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<bool()> pred,
			int nvar,
			int& resul) {
			if (nvar == nvars) {
				if (filter() && pred()) {
					resul++;
				}
			}
			else {
				for (T i = init(); i <= end(); i++) {
					*(vars[nvar]) = i;
					do_count(vars, nvars, init, end, filter, pred, nvar + 1, resul);
				}
			}
		}

		template<class T, class ... tPack>
		int do_count_launcher(std::function<T()> init,
			std::function<T()> end,
			std::function<bool()> filter,
			std::function<bool()> pred,
			tPack& ... vs) {
			T* vars[] = { &vs ... };
			int nvars = sizeof(vars) / sizeof(vars[0]);
			int resul = 0;
			do_count(vars, nvars, init, end, filter, pred, 0, resul);
			return resul;
		}


 };

#define _epp_var_list(...) __VA_ARGS__
#define _epp_lambda(_epp_exp) [&]() {return _epp_exp;}
#define _epp_qpred_prolog(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_pred)  \
     _epp_type _epp_var_list _epp_vars;                                                      \
	 std::function<_epp_type()> _init = _epp_lambda(_epp_init);                               \
	 std::function<_epp_type()> _end = _epp_lambda(_epp_end);                                 \
	 std::function<bool()> _filter = _epp_lambda(_epp_filter);                                \
	 std::function<bool()> _pred = _epp_lambda(_epp_pred);                   


#define _epp_qexp_prolog(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_type_return, _epp_val)  \
   _epp_type _epp_var_list _epp_vars;                                \
   std::function<_epp_type()> _init = _epp_lambda(_epp_init);              \
   std::function<_epp_type()> _end = _epp_lambda(_epp_end);                \
   std::function<bool()> _filter = _epp_lambda(_epp_filter);               \
   std::function<_epp_type_return()> _val = _epp_lambda(_epp_val);         


#define epp_forall(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_pred) [&](){\
   _epp_qpred_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), (_epp_pred)) \
   return _epp::do_qpred_launcher<_epp_type,_epp::FORALL>(_init,_end,_filter,_pred,_epp_var_list _epp_vars); \
 }()

#define epp_exists(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_pred) [&](){\
   _epp_qpred_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), (_epp_pred)) \
   return _epp::do_qpred_launcher<_epp_type,_epp::EXISTS>(_init,_end,_filter,_pred,_epp_var_list _epp_vars); \
 }()

#define epp_sum(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_type_return, _epp_val) [&](){\
   _epp_qexp_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), _epp_type_return, (_epp_val))  \
   return _epp::do_qaexp_launcher<_epp_type,_epp_type_return,_epp::SUM>(_init,_end,_filter,_val,_epp_var_list _epp_vars); \
 }()

#define epp_prod(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_type_return, _epp_val) [&](){\
   _epp_qexp_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), _epp_type_return, (_epp_val))  \
   return _epp::do_qaexp_launcher<_epp_type,_epp_type_return,_epp::PROD>(_init,_end,_filter,_val,_epp_var_list _epp_vars); \
 }()

#define epp_max(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_type_return, _epp_val) [&](){\
   _epp_qexp_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), _epp_type_return, (_epp_val))  \
   return _epp::do_max_launcher(_init,_end,_filter,_val,_epp_var_list _epp_vars); \
 }()


#define epp_min(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_type_return, _epp_val) [&](){\
   _epp_qexp_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), _epp_type_return, (_epp_val))  \
   return _epp::do_min_launcher(_init,_end,_filter,_val,_epp_var_list _epp_vars); \
 }()

#define epp_count(_epp_type, _epp_vars, _epp_init, _epp_end, _epp_filter, _epp_pred) [&](){\
   _epp_qpred_prolog(_epp_type, _epp_vars, (_epp_init), (_epp_end), (_epp_filter), (_epp_pred)) \
   return _epp::do_count_launcher(_init,_end,_filter,_pred,_epp_var_list _epp_vars); \
 }()

#define epp_imp(_po,_p1) (! (_po) || (_p1))
#define epp_equiv(_po,_p1) (epp_imp((_po),(_p1)) && epp_imp((_p1),(_po)))
