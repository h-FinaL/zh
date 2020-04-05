#include <pthread.h>
#include <functional>
#include <tuple>

class Thread
{
  using SizeType = std::size_t;
  using IdType = pthread_t;

public:
	template<typename Fun, typename ...Args>
	Thread(Fun fun, Args... args)
	{
		Thread_f<Fun, Args...> thread_f(fun, args...);
		_id = thread_f.run();
	}
	~Thread(){}

	Thread(const Thread& ) = delete;
	Thread& operator=(const Thread&) = delete;

	inline void join()
	{
		pthread_join(_id, NULL);
	}
	inline void detach()
	{
		pthread_detach(_id);
	}

private:
	IdType _id;


private:

	template<typename Fun, typename ...Args>
	class Thread_f
	{
		template<SizeType...>
		struct IndexTuple {};

		template<SizeType N, SizeType... Indexes>
		struct MakeIndexes : MakeIndexes<N -1, N -1, Indexes...> {};

		template<SizeType ...Indexes>
		struct MakeIndexes<0, Indexes...>
		{
			typedef IndexTuple<Indexes...> type;
		};

		typedef typename MakeIndexes<sizeof ...(Args)>::type IndexType;

	public:
		Thread_f(Fun fun, Args... args) : _fun(fun), _args(args...) {}
		~Thread_f() {}
		IdType run()
		{
			IdType id;
			pthread_create(&id, NULL, Thread_f::thread_handle, this);
			return id;
		}
		static void* thread_handle(void* thread_f)
		{
			Thread_f<Fun, Args...>* th = (Thread_f<Fun, Args...>*) thread_f;
			th->invoke(IndexType());
		}
		template<SizeType ...Index>
		void invoke(IndexTuple<Index...>)
		{
			_fun(std::get<Index>(_args)...);
		}

		Fun _fun;
		std::tuple<Args...> _args;
	};

};

