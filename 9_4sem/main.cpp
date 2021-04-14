#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

class Chat
{
private:

    using shared_memory_t = boost::interprocess::managed_shared_memory;
    using manager_t = boost::interprocess::managed_shared_memory::segment_manager;
    using string_allocator_t = boost::interprocess::allocator < char, manager_t>;
    using string_t = boost::interprocess::basic_string < char, std::char_traits < char >, string_allocator_t>;
    using vector_allocator_t = boost::interprocess::allocator < string_t, manager_t >;
    using vector_t = boost::interprocess::vector < string_t, vector_allocator_t >;

public:

    explicit Chat(const std::string& user_name) : m_user_name(user_name), m_exit_flag(false),
        m_shared_memory(boost::interprocess::open_or_create, shared_memory_name.c_str(), 50000)
    {
        m_vector = m_shared_memory.find_or_construct < vector_t >("vector")(m_shared_memory.get_segment_manager());
        m_users = m_shared_memory.find_or_construct<counter_t>("m_users")(0);
        m_messages = m_shared_memory.find_or_construct<counter_t>("m_messages")(0);

        ++(*m_users);

        m_local_messages = 0;
    }


    ~Chat() noexcept
    {
        if (!(--(*m_users)))
        {
            boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
        }
    }

int main(int argc, char** argv)
{
	using allocator = boost::interprocess::allocator < char,
		boost::interprocess::managed_shared_memory::segment_manager >;

	using string = boost::interprocess::basic_string < char,
		std::char_traits < char >, allocator>;

	const std::string shared_memory_name = "managed_shared_memory";

	boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());

	boost::interprocess::managed_shared_memory shared_memory(
		boost::interprocess::open_or_create, shared_memory_name.c_str(), 1024);

	auto s = shared_memory.find_or_construct < string >("String")("Hello",
		shared_memory.get_segment_manager());

	*s += ", world!";

	std::cout << *s << std::endl;

	boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());

	system("pause");

	return EXIT_SUCCESS;
}