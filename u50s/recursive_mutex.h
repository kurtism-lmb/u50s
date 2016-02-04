#ifndef __U50S_RECURSIVE_MUTEX
#define __U50S_RECURSIVE_MUTEX

#include "u50s/system.h"
#include "u50s/synchronization/mutex.h"
#include <stdint.h>
#include <stdexcept>

namespace u50s
{
namespace synchronization
{

class recursive_mutex
{
	mutex m_mutex;
	thread_id_t m_owner;
	uint32_t m_count;

public:
	recursive_mutex() :m_count(0) {}

	bool try_acquire()  
	{
		thread_id_t requester = get_current_thread_id();
		if(requester == m_owner)  {
			m_count++;
			return true;
		}
		if(!m_mutex.try_acquire())
			return false;
		m_owner = requester;
		m_count = 1;
	}

	void acquire()
	{
		thread_id_t requester = get_current_thread_id();
		if(requester == m_owner)  {
			m_count++;
			return;
		}
		m_mutex.acquire();
		m_owner = requester;
		m_count = 1;
	}

	void release() throw(std::logic_error)
	{
		thread_id_t releaser = get_current_thread_id();
		if(releaser != m_owner)  {
			std::ostringstream oss;
			oss << "u50s::synchronization::recursive_mutex::release called from non-owning thread " << releaser << " (owner " << m_owner << ")";
			throw std::logic_error(oss.str());
		}
		m_count--;
		if(0 == m_count)
			m_mutex.release();
	}

	uint32_t count()
	{
		thread_id_t requester = get_current_thread_id();
		if(requester != m_owner)
			return 0;
		return m_count;
	}
};

} // namespace synchronization
} // namespace u50s

#endif