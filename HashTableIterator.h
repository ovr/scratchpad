#ifndef HASHTABLEITERATOR_H_
#define HASHTABLEITERATOR_H_

#include <iterator>

extern "C" {
#include <Zend/zend.h>
#include <Zend/zend_hash.h>
}

template<typename T>
class ZendHashTableIterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, T*, T&> {
private:
	const HashTable* m_ht;
	HashPosition m_pos;
	bool m_ok;

public:
	ZendHashTableIterator(const HashTable* h, const HashPosition& p)
		: m_ht(h), m_pos(p), m_ok(h && p)
	{
	}

	ZendHashTableIterator(const ZendHashTableIterator<T>& other)
		: m_ht(other.m_ht), m_pos(other.m_pos), m_ok(other.m_ok)
	{
	}

	ZendHashTableIterator& operator++(void)
	{
		this->m_ok = (SUCCESS == zend_hash_move_forward_ex(this->m_ht, &this->m_pos));
		return *this;
	}

	ZendHashTableIterator& operator--(void)
	{
		this->m_ok = (SUCCESS == zend_hash_move_backwards_ex(this->m_ht, &this->m_pos));
		return *this;
	}

	ZendHashTableIterator operator++(int)
	{
		ZendHashTableIterator<T> tmp(*this);
		this->m_ok = (SUCCESS == zend_hash_move_forward_ex(this->m_ht, &this->m_pos));
		return tmp;
	}

	ZendHashTableIterator operator--(int)
	{
		ZendHashTableIterator<T> tmp(*this);
		this->m_ok = (SUCCESS == zend_hash_move_backwards_ex(this->m_ht, &this->m_pos));
		return tmp;
	}

	bool operator==(const ZendHashTableIterator<T>& other) const
	{
		return this->m_ht == other.m_ht && this->m_pos == other.m_pos;
	}

	bool operator!=(const ZendHashTableIterator<T>& other) const
	{
		return this->m_ht != other.m_ht || this->m_pos != other.m_pos;
	}

	bool operator!(void) const
	{
		return !this->m_ok;
	}

	const T& operator*(void) const
	{
		T* res;
		zend_hash_get_current_data_ex(this->m_ht, static_cast<void**>(&res), &this->m_pos);
		return *res;
	}

	T& operator*(void)
	{
		T* res;
		zend_hash_get_current_data_ex(this->m_ht, static_cast<void**>(&res), &this->m_pos);
		return *res;
	}

	const T* operator->(void) const
	{
		return &(this->operator*());
	}

	T* operator->(void)
	{
		return &(this->operator*());
	}

	zval key(void) const
	{
		zval result = zval_used_for_init;
		char* str_index;
		uint str_length;
		ulong num_index;

		switch (zend_hash_get_current_key_ex(this->m_ht, &str_index, &str_length, &num_index, 0, this->m_pos)) {
			case HASH_KEY_IS_STRING:
				ZVAL_STRINGL(&result, str_index, str_length-1, 0);
				break;

			case HASH_KEY_IS_LONG:
				ZVAL_LONG(&result, num_index);
				break;
		}

		return result;
	}

	int keyType(void) const
	{
		return zend_hash_get_current_key_type_ex(this->m_ht, &this->m_pos);
	}
};

#endif /* HASHTABLEITERATOR_H_ */
