#ifndef S7WRAP
#define S7WRAP

#include <utility>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "s7.h"
s7_pointer api_set_map (s7_scheme *sc, s7_pointer args);
s7_pointer api_comm_move (s7_scheme *sc, s7_pointer args);
s7_pointer api_comm_find (s7_scheme *sc, s7_pointer args);
s7_pointer api_comm_get (s7_scheme *sc, s7_pointer args);
s7_pointer api_start (s7_scheme *sc, s7_pointer args);
}

class S7 {
public:
// IMPLEMENT BELOW

	void api_set_map (long long w, long long h, const char* s);
	void api_comm_move (long long x, long long y);
	void api_comm_find (long long what);
	void api_comm_get (long long what);
	void api_start (long long cnt);

// IMPLEMENT ABOVE
private:
	s7_scheme *s7;
	char buff[4096];
	S7 ()
	: s7 (s7_init())
	{
		s7_define_function (s7, "api_set_map", ::api_set_map, 3, 0, false, "(set_map w h s) -> void\nマップをセットする\n  w (int): 横のブロック数\n  h (int): 縦のブロック数\n  s (str): 改行を含まない文字列形式のマップデータ");
		s7_define_function (s7, "api_comm_move", ::api_comm_move, 2, 0, false, "(comm_move x y) -> void\n移動命令を追加する\n  x (int): X座標\n  y (int): Y座標");
		s7_define_function (s7, "api_comm_find", ::api_comm_find, 1, 0, false, "(comm_find what) -> void\n探索命令を追加する\n  what (int): API_APPLE");
		s7_define_function (s7, "api_comm_get", ::api_comm_get, 1, 0, false, "(comm_get what) -> void\n取得命令を追加する\n  what (int): API_APPLE");
		s7_define_function (s7, "api_start", ::api_start, 1, 0, false, "(start cnt) -> void\n処理を開始する\n  cnt (int): 処理を打ち切るターン数");
	}
	virtual ~S7 ()
	{}
public:
	class Atom
	{
		s7_pointer atom;
		s7_int loc;
		Atom (const Atom& atom)=delete;
		Atom& operator= (const Atom& atom)=delete;
	public:
		Atom ()
		: atom (nullptr)
		{}
		Atom (s7_pointer atom)
		: atom (atom), loc (s7_gc_protect (S7::get(), atom))
		{}
		virtual ~Atom()
		{
			if (atom != nullptr)
				s7_gc_unprotect_at (S7::get(), loc);
		}
		operator s7_pointer() const
		{
			return atom;
		}
		Atom& operator= (s7_pointer s7p)
		{
			if (atom != nullptr)
				s7_gc_unprotect_at (S7::get(), loc);
			atom = s7p;
			if (atom != nullptr)
				loc = s7_gc_protect (S7::get(), atom);
			return *this;
		}
		Atom (Atom&& one)
		: atom (one.atom), loc (one.loc)
		{
			one.atom = nullptr;
		}
		const Atom& operator= (Atom&& one)
		{
			atom = one.atom;
			loc = one.loc;
			one.atom = nullptr;
			return *this;
		}
	};
	class Env {
		Atom env;
		Env (const Env& env)=delete;
	public:
		Env (s7_pointer e)
		: env (s7_sublet (S7::get(), e, s7_nil (S7::get())))
		{}
		Env ()
		: env (s7_sublet (S7::get(), s7_curlet (S7::get()), s7_nil (S7::get())))
		{}
		Env (Env& env)
		: env (s7_sublet (S7::get(), env, s7_nil (S7::get())))
		{}
		template<class T=s7_pointer>
		T eval (const char* s)
		{
			return S7::get().eval<T> (s, this);
		}
		virtual ~Env ()
		{}
		operator s7_pointer () { return env; }
		void load (const char* path)
		{
			S7& s7 = S7::get();
			if (!s7_load_with_environment (s7, path, env)) {
				sprintf (s7.buff, "S7::Env::load() : CAN'T FIND %s.", path);
				throw s7.buff;
			}
		}
	};
	static S7& get ()
	{
		static S7 s7;
		return s7;
	}
	operator s7_scheme* () { return s7; }

	template<class T=s7_pointer>
	T eval (const char* s, S7::Env* env=nullptr);
	const char* eval_as_string (const char* s) {
		s7_pointer s7_ret = s7_eval_c_string (s7, s);
		char* rs = s7_object_to_c_string (s7, s7_ret);
		strcpy (buff, rs);
		free (rs);
		return buff;
	}
	void def_var (const char* name, int val)
	{
		s7_define_variable (s7, name, s7_make_integer (s7, val));
	}
	void def_var (const char* name, double val)
	{
		s7_define_variable (s7, name, s7_make_real (s7, val));
	}
	void def_var (const char* name, const char* val)
	{
		s7_define_variable (s7, name, s7_make_string (s7, val));
	}
	void def_var (const char* name, void* val)
	{
		s7_define_variable (s7, name, s7_make_c_pointer (s7, val));
	}
	void load (const char* path)
	{
		if (!s7_load (s7, path)) {
			sprintf (buff, "S7::load() : CAN'T FIND %s.", path);
			throw buff;
		}
	}
	s7_pointer set_err_hook (const char* proc_name);
};

template<>
inline s7_pointer S7::eval (const char* s, S7::Env* env)
{
	return env == nullptr ?
		s7_eval_c_string (s7, s):
		s7_eval_c_string_with_environment (s7, s, *env);
}

template<>
inline const char* S7::eval (const char* s, S7::Env* env)
{
	s7_pointer s7_ret = eval (s, env);
	if (!s7_is_string (s7_ret))
		throw "S7::eval<const char*>() : NOT STRING";
	return s7_string (s7_ret);
}

template<>
inline long long S7::eval (const char* s, S7::Env* env)
{
	s7_pointer s7_ret = eval (s, env);
	if (!s7_is_integer (s7_ret))
		throw "S7::eval<long long>() : NOT INTEGER";
	return s7_integer (s7_ret);
}

template<>
inline double S7::eval (const char* s, S7::Env* env)
{
	s7_pointer s7_ret = eval (s, env);
	if (!s7_is_real (s7_ret))
		throw "S7::eval<double>() : NOT DOUBLE";
	return s7_real (s7_ret);
}

template<>
inline bool S7::eval (const char* s, S7::Env* env)
{
	s7_pointer s7_ret = eval (s, env);
	if (!s7_is_boolean (s7_ret))
		throw "S7::eval<bool>() : NOT BOOLEAN";
	return s7_boolean (s7, s7_ret);
}

inline s7_pointer S7::set_err_hook (const char* proc_name)
{
	sprintf (buff, "(set! (hook-functions *error-hook*) (list (lambda (hook) (set! (hook 'result) (%s (apply format #f (hook 'data)) (stacktrace))))))", proc_name);
	return eval (buff);
}

extern "C" {
inline s7_pointer api_set_map (s7_scheme *sc, s7_pointer args)
{
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_set_map", 1, s7_car (args), "an integer"));
	long long arg_w = s7_integer (s7_car (args));
	args = s7_cdr (args);
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_set_map", 2, s7_car (args), "an integer"));
	long long arg_h = s7_integer (s7_car (args));
	args = s7_cdr (args);
	if (!s7_is_string (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_set_map", 3, s7_car (args), "an string"));
	const char* arg_s = s7_string (s7_car (args));
	args = s7_cdr (args);
	S7::get().api_set_map (arg_w, arg_h, arg_s);
	return s7_unspecified (sc);
}
inline s7_pointer api_comm_move (s7_scheme *sc, s7_pointer args)
{
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_comm_move", 1, s7_car (args), "an integer"));
	long long arg_x = s7_integer (s7_car (args));
	args = s7_cdr (args);
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_comm_move", 2, s7_car (args), "an integer"));
	long long arg_y = s7_integer (s7_car (args));
	args = s7_cdr (args);
	S7::get().api_comm_move (arg_x, arg_y);
	return s7_unspecified (sc);
}
inline s7_pointer api_comm_find (s7_scheme *sc, s7_pointer args)
{
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_comm_find", 1, s7_car (args), "an integer"));
	long long arg_what = s7_integer (s7_car (args));
	args = s7_cdr (args);
	S7::get().api_comm_find (arg_what);
	return s7_unspecified (sc);
}
inline s7_pointer api_comm_get (s7_scheme *sc, s7_pointer args)
{
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_comm_get", 1, s7_car (args), "an integer"));
	long long arg_what = s7_integer (s7_car (args));
	args = s7_cdr (args);
	S7::get().api_comm_get (arg_what);
	return s7_unspecified (sc);
}
inline s7_pointer api_start (s7_scheme *sc, s7_pointer args)
{
	if (!s7_is_integer (s7_car (args)))
		return (s7_wrong_type_arg_error (sc, "api_start", 1, s7_car (args), "an integer"));
	long long arg_cnt = s7_integer (s7_car (args));
	args = s7_cdr (args);
	S7::get().api_start (arg_cnt);
	return s7_unspecified (sc);
}
} // extern "C"

#endif // S7_WRAP_H
