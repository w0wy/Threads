#include <exception>

struct BadAllocException : public std::bad_alloc
{
	BadAllocException(const std::string& action,
		const std::string& type,
		uintptr_t addr):
	action_(action),
	type_(type),
	addr_(addr)
	{}

	virtual const char * what () const throw () {
		std::string msg = "Action : " + action_ +
			" failed for ptr of type : " + type_ +
			" at address : " + std::to_string(addr_);
		return msg.c_str();
	}

private:
	std::string action_;
	std::string type_;
	uintptr_t addr_;
};