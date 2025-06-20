#include<functional>
#include<vector>
#include<iostream>
#include<cassert>
#include<memory>

#ifndef _SPECPP
#define _SPECPP


using namespace std;

typedef std::function<bool()> Predicate;

template<class T>
class Var : public shared_ptr<T> {
public:
	Var(T* p): shared_ptr<T>(p)  {};
};

#define pred(exp) [=](){return (exp);}

#define exp(e) [=](){return (e);}


#define var(T, N) Var<T> N(new T);


Predicate operator && (Predicate p1, Predicate p2) {
	return [=]() {
		return p1() && p2();	
		};
}

Predicate operator || (Predicate p1, Predicate p2) {
	return [=]() {return p1() || p2(); };
}

Predicate operator ! (Predicate p) {
	return [=]() {return !(p()); };
}

Predicate operator >> (Predicate p1, Predicate p2) {
	return (!p1) || p2;
}

Predicate  equiv  (Predicate p1, Predicate p2) {
	return (p1>>p2) && (p2>>p1);
}


template<class TVal>
class QuantifiedPred {
public:
	class RangeCollector;
	class FilterCollector;
	class YieldCollector {
	public:
		Predicate yield(Predicate p) {
			return _enc->yield(p);
		}
		friend FilterCollector;
	private:
		YieldCollector(QuantifiedPred* enc) : _enc(enc) {}
		QuantifiedPred* _enc;
	};

	class FilterCollector {
	public:
		YieldCollector filter(Predicate filter) {
			_enc->_filter = filter;
			return YieldCollector(_enc);
		}
		Predicate yield(Predicate p) {
			return YieldCollector(_enc).yield(p);
		}
		friend QuantifiedPred;
	private:
		FilterCollector(QuantifiedPred* enc) : _enc(enc) {}
		QuantifiedPred* _enc;
	};


	FilterCollector range(TVal inf, TVal sup) {
		_inf = exp(inf);
		_sup = exp(sup);
		return FilterCollector(this);
	}

	FilterCollector range(Var<TVal> inf, Var<TVal> sup) {
		_inf = exp(*inf);
		_sup = exp(*sup);
		return FilterCollector(this);
	}

protected:
	virtual bool check(int nvar) const = 0;
	virtual Predicate yield(Predicate p) = 0;
	vector<shared_ptr<TVal>> _vars;
	std::function<TVal()> _inf;
	std::function<TVal()> _sup;
	Predicate _filter;
	Predicate _yield;
};

template<class TVal>
class forall : public QuantifiedPred<TVal> {
public:
	template<class... tPack>
	forall(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true; };
	}

protected:
	bool check(int nvar) const {
		if (nvar == this->_vars.size()) {
			return !this->_filter() || this->_yield();
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				if (!check(nvar + 1)) return false;
			}
			return true;
		}
	}
	Predicate yield(Predicate p) {
		this->_yield = p;
		forall ctx(*this);
		return [=]() {
			return ctx.check(0);
		};
	};
};



template<class TVal>
class exits : public QuantifiedPred<TVal> {
public:
	template<class... tPack>
	exits(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true; };
	}

protected:
	bool check(int nvar) const {
		if (nvar == this->_vars.size()) {
			return this->_filter() && this->_yield();
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				if (check(nvar + 1)) return true;
			}
			return false;
		}
	}
	Predicate yield(Predicate p) {
		this->_yield = p;
		exits ctx(*this);
		return [=]() {
			return ctx.check(0);
		};
	};
};


template<class T1, class T2>
auto operator +(T1 f1, T2 f2) {
	return [=]() {return f1() + f2(); };
}

template<class T1, class T2>
auto operator -(T1 f1, T2 f2) {
	return [=]() {return f1() - f2(); };
}

template<class T1, class T2>
auto operator *(T1 f1, T2 f2) {
	return [=]() {return f1() * f2(); };
}

template<class T1, class T2>
auto operator / (T1 f1, T2 f2) {
	return [=]() {return f1() / f2(); };
}

template<class T1, class T2>
auto operator % (T1 f1, T2 f2) {
	return [=]() {return f1() % f2(); };
}


template <class tOp1, class tOp2>
Predicate operator == (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() == op2(); };
}

template <class tOp1, class tOp2>
Predicate operator != (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() != op2(); };
}

template <class tOp1, class tOp2>
Predicate operator < (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() < op2(); };
}

template <class tOp1, class tOp2>
Predicate operator > (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() > op2(); };
}

template <class tOp1, class tOp2>
Predicate operator <= (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() <= op2(); };
}

template <class tOp1, class tOp2>
Predicate operator >= (tOp1 op1, tOp2 op2) {
	return [=]() {return op1() >= op2(); };
}


template<class TVal, class TResul>
class QuantifiedExp {
public:
	class RangeCollector;
	class FilterCollector;
	class ValCollector {
	public:
		std::function<TResul()> val(std::function<TResul()> p) {
			return _enc->val(p);
		}
		friend FilterCollector;
	private:
		ValCollector(QuantifiedExp* enc) : _enc(enc) {}
		QuantifiedExp* _enc;
	};

	class FilterCollector {
	public:
		ValCollector filter(Predicate filter) {
			_enc->_filter = filter;
			return ValCollector(_enc);
		}
		std::function<TResul()> val(std::function<TResul()> p) {
			return ValCollector(_enc).val(p);
		}
		friend QuantifiedExp;
	private:
		FilterCollector(QuantifiedExp* enc) : _enc(enc) {}
		QuantifiedExp* _enc;
	};


	FilterCollector range(TVal inf, TVal sup) {
		_inf = exp(inf);
		_sup = exp(sup);
		return FilterCollector(this);
	}

	FilterCollector range(Var<TVal> inf, Var<TVal> sup) {
		_inf = exp(*inf);
		_sup = exp(*sup);
		return FilterCollector(this);
	}


protected:
	virtual TResul eval() const = 0;
	virtual std::function<TResul()> val(std::function<TResul()> f) = 0;
	vector<shared_ptr<TVal>> _vars;
	std::function<TVal()> _inf;
	std::function<TVal()> _sup;
    Predicate _filter;
    std::function<TResul()> _val;
};



template<class TVal,class TResul>
class sum : public QuantifiedExp<TVal,TResul> {
public:
	template<class... tPack>
	sum(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true;
	};
	}

protected:
	TResul eval()  const {
		TResul resul = TResul(0);
		do_eval(0,resul);
		return resul;
	}

	std::function<TResul()> val(std::function<TResul()> f) {
		this->_val = f;
		sum ctx(*this);
		return  [=]() {
			return ctx.eval();
			};
	};

private:

	void do_eval(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) resul += this->_val();
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				do_eval(nvar + 1,resul);
			}
		}
	}
};


template<class TVal, class TResul>
class prod : public QuantifiedExp<TVal, TResul> {
public:
	template<class... tPack>
	prod(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true;
			};
	}

protected:
	TResul eval() const {
		TResul resul = TResul(1);
		do_eval(0, resul);
		return resul;
	}

	std::function<TResul()> val(std::function<TResul()> f) {
		this->_val = f;
		prod ctx(*this);
		return  [=]() {
			return ctx.eval();
			};
	};

private:

	void do_eval(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) resul *= this->_val();
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				do_eval(nvar + 1, resul);
			}
		}
	}
};




class EEmptyRange {};

template<class TVal, class TResul>
class mini : public QuantifiedExp<TVal, TResul> {
public:
	template<class... tPack>
	mini(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true;
			};
	}

protected:
	TResul eval() const {
		TResul resul;
		bool ok = find_value(0, resul);
		if (! ok) throw EEmptyRange();
		do_eval(0, resul);		
		return resul;
	}

	std::function<TResul()> val(std::function<TResul()> f) {
		this->_val = f;
		mini ctx(*this);
		return  [=]() {
			return ctx.eval();
			};
	};

private:

	bool find_value(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) {
				resul = this->_val();
				return true;
			}
			else {
				return false;
			}
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				if (find_value(nvar + 1, resul)) return true;
			}
			return false;
		}
	}

	void do_eval(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) {
				TResul v = this->_val();
				if (v < resul) resul = v;
			}
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				do_eval(nvar + 1, resul);
			}
		}
	}
};


template<class TVal, class TResul>
class maxi : public QuantifiedExp<TVal, TResul> {
public:
	template<class... tPack>
	maxi(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true;
			};
	}

protected:
	TResul eval() const {
		TResul resul;
		bool ok = find_value(0, resul);
		if (! ok) throw EEmptyRange();
		do_eval(0, resul);		
		return resul;
	}

	std::function<TResul()> val(std::function<TResul()> f) {
		this->_val = f;
		maxi ctx(*this);
		return  [=]() {
			return ctx.eval();
			};
	};

private:

	bool find_value(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) {
				resul = this->_val();
				return true;
			}
			else {
				return false;
			}
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				if (find_value(nvar + 1, resul)) return true;
			}
			return false;
		}
	}

	void do_eval(int nvar, TResul& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter()) {
				TResul v = this->_val();
				if (v > resul) resul = v;
			}
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				do_eval(nvar + 1, resul);
			}
		}
	}
};

template<class TVal>
class number {
public:
	template<class... tPack>
	number(tPack... vars) {
		this->_vars = { vars ... };
		this->_filter = []() {return true;
		};
	}

	class RangeCollector;
	class FilterCollector;
	class YieldCollector {
	public:
		std::function<int()> yield(Predicate p) {
			return _enc->yield(p);
		}
		friend FilterCollector;
	private:
		YieldCollector(number* enc) : _enc(enc) {}
		number* _enc;
	};

	class FilterCollector {
	public:
		YieldCollector filter(Predicate filter) {
			_enc->_filter = filter;
			return YieldCollector(_enc);
		}
		std::function<int()> yield(Predicate p) {
			return YieldCollector(_enc).yield(p);
		}
		friend number;
	private:
		FilterCollector(number* enc) : _enc(enc) {}
		number* _enc;
	};


	FilterCollector range(TVal inf, TVal sup) {
		_inf = exp(inf);
		_sup = exp(sup);
		return FilterCollector(this);
	}

	FilterCollector range(Var<TVal> inf, Var<TVal> sup) {
		_inf = exp(*inf);
		_sup = exp(*sup);
		return FilterCollector(this);
	}

private:
	std::function<int()> yield(Predicate p) {
		this->_yield = p;
		number ctx(*this);
		return  [=]() {
			return ctx.eval();
		};
	};

	int eval() const {
		int resul=0;
		do_eval(0, resul);
		return resul;
	}

	void do_eval(int nvar, int& resul) const {
		if (nvar == this->_vars.size()) {
			if (this->_filter() && this->_yield()) {
				resul++;
			}
		}
		else {
			for (TVal v = this->_inf(); v <= this->_sup(); v++) {
				*(this->_vars[nvar]) = v;
				do_eval(nvar + 1, resul);
			}
		}
	}

	vector<shared_ptr<TVal>> _vars;
	std::function<TVal()> _inf;
	std::function<TVal()> _sup;
	Predicate _filter;
	Predicate _yield;
};



#endif
