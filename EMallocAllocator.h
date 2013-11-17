#ifndef EMALLOCALLOCATOR_H_
#define EMALLOCALLOCATOR_H_

#include <cstddef>
#include <limits>
#include <new>

extern "C" {
#include <Zend/zend.h>
}

template<typename T>
class EMallocAllocator {
public:
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	// convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef EMallocAllocator<U> other;
	};

	EMallocAllocator(void)
	{
	}

	~EMallocAllocator(void)
	{
	}

	EMallocAllocator(const EMallocAllocator&)
	{
	}

	template<typename U>
	EMallocAllocator(const EMallocAllocator<U>&)
	{
	}

	pointer address(reference r)
	{
		return &r;
	}

	const_pointer address(const_reference r)
	{
		return &r;
	}

	pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0)
	{
		pointer tmp = reinterpret_cast<pointer>(emalloc(cnt * sizeof(T)));
		if (!tmp) {
			throw std::bad_alloc();
		}

		return tmp;
	}

	void deallocate(pointer p, size_type)
	{
		efree(p);
	}

	size_type max_size(void) const
	{
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	void construct(pointer p, const T& t)
	{
		new(reinterpret_cast<void*>(p)) T(t);
	}

	void destroy(pointer p)
	{
		p->~T();
	}

	bool operator==(const EMallocAllocator&) const
	{
		return true;
	}

	bool operator!=(const EMallocAllocator& a) const
	{
		return !operator==(a);
	}
};

#endif /* EMALLOCALLOCATOR_H_ */
