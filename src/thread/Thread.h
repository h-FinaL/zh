#include <pthread.h>
#include <functional>
#include <tuple>

namespace zh
{

class Thread
{

public:
	template<typename Fun, typename ...Args>
	Thread(Fun&& fun, Args&&... args)
	{
		int errorCode = Thread_f<Fun, Args...>::start_thread(&m_id, 
			std::forward<Fun>(fun), 
			std::forward<Args>(args)...);
		if (errorCode)
		{
			std::cout << "pthread_create thread fail, errorCode = " << errorCode << std::endl;
        	throw std::logic_error("pthread_create error");
		}
	}
	Thread() : m_id(0) {}
	~Thread(){}

	Thread(const Thread& ) = delete;
	Thread& operator=(const Thread&) = delete;

	void join()
	{
		if (m_id > 0)
			pthread_join(m_id, NULL);
	}
	void detach()
	{
		if (m_id > 0)
			pthread_detach(m_id);
	}
	pthread_t getId() { return m_id; }

private:
	//线程id
	pthread_t m_id;

private:

	template<typename Fun, typename ...Args>
	class Thread_f
	{
  		using SizeType = std::size_t;
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
		Thread_f(Fun fun, Args... args) : m_fun(fun), m_args(args...) {}
		~Thread_f() {}

		static int start_thread(pthread_t* id, Fun&& fun, Args&&... args)
		{
			Thread_f<Fun, Args...>* thread_f = new Thread_f<Fun, Args...>(fun, args...);
			return pthread_create(id, NULL, Thread_f::thread_handle, thread_f);
		}
		static void* thread_handle(void* thread_f)
		{
			Thread_f<Fun, Args...>* th = (Thread_f<Fun, Args...>*) thread_f;
			th->invoke(IndexType());
			delete th;
		}
		template<SizeType ...Index>
		void invoke(IndexTuple<Index...>)
		{
			m_fun(std::get<Index>(m_args)...);
		}

		Fun m_fun;
		std::tuple<Args...> m_args;
	};

};

}