/*
 * HashTable.h
 *
 *  Created on: 26 серп. 2013
 *      Author: vladimir
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <cstring>
#include <iterator>
#include "HashTableIterator.h"

template<typename T>
class ZendHashTable {
public:
	bool isSet(const char* key) const
	{
		return this->isSet(key, std::strlen(key)+1);
	}

	bool isSet(const char* key, uint keylen) const
	{
		return zend_symtable_exists(this->m_hash, key, keylen);
	}

	bool isSet(ulong index) const
	{
		return zend_hash_index_exists(this->m_hash, index);
	}

	bool isSet(const char* key, uint keylen, ulong hash) const
	{
		return zend_hash_quick_exists(this->m_hash, key, keylen, hash);
	}

	bool unSet(const char* key)
	{
		return this->unSet(key, std::strlen(key)+1);
	}

	bool unSet(const char* key, uint keylen)
	{
		return SUCCESS == zend_symtable_del(this->m_hash, key, keylen);
	}

	bool unSet(ulong index)
	{
		return SUCCESS == zend_hash_del_key_or_index(this->m_hash, NULL, 0, index, HASH_DEL_INDEX);
	}

	bool unSet(const char* key, uint keylen, ulong hash)
	{
		return SUCCESS == zend_hash_del_key_or_index(this->m_hash, key, keylen, hash, HASH_DEL_KEY_QUICK);
	}

	T* find(const char* key) const
	{
		return this->find(key, std::strlen(key)+1);
	}

	T* find(const char* key, uint keylen) const
	{
		T* ret;
		return (SUCCESS == zend_symtable_find(this->m_hash, key, keylen, static_cast<void**>(&ret))) ? ret : 0;
	}

	T* find(ulong index) const
	{
		T* ret;
		return (SUCCESS == zend_hash_index_find(this->m_hash, index, static_cast<void**>(&ret))) ? ret : 0;
	}

	T* find(const char* key, uint keylen, ulong hash) const
	{
		T* ret;
		return (SUCCESS == zend_hash_quick_find(this->m_hash, key, keylen, hash, static_cast<void**>(&ret))) ? ret : 0;
	}

	bool insert(const char* key, uint keylen, const T* item)
	{
		return SUCCESS == zend_symtable_update(this->m_hash, key, keylen, static_cast<void**>(&item), sizeof(T), NULL);
	}

	bool insert(ulong index, const T* item)
	{
		return SUCCESS == zend_hash_index_update(this->m_hash, index, static_cast<void**>(&item), sizeof(T), NULL);
	}

	bool insert(const char* key, uint keylen, ulong hash, const T* item)
	{
		return SUCCESS == zend_hash_quick_update(this->m_hash, key, keylen, hash, static_cast<void**>(&item), sizeof(T), NULL);
	}

	int size(void) const
	{
		return zend_hash_num_elements(this->m_hash);
	}

	void clear(void)
	{
		zend_hash_clean(this->m_hash);
	}

	operator HashTable*(void)
	{
		return this->m_hash;
	}

	operator const HashTable*(void) const
	{
		return this->m_hash;
	}

	bool rehash(void)
	{
		return SUCCESS == zend_hash_rehash(this->m_hash);
	}

	const ZendHashTableIterator<T> first(void) const
	{
		HashPosition pos;
		zend_hash_internal_pointer_reset_ex(this->m_hash, &pos);
		return ZendHashTableIterator<T>(this->m_hash, pos);
	}

	ZendHashTableIterator<T> first(void)
	{
		HashPosition pos;
		zend_hash_internal_pointer_reset_ex(this->m_hash, &pos);
		return ZendHashTableIterator<T>(this->m_hash, pos);
	}

	const ZendHashTableIterator<T> last(void) const
	{
		HashPosition pos;
		zend_hash_internal_pointer_end_ex(this->m_hash, &pos);
		return ZendHashTableIterator<T>(this->m_hash, pos);
	}

	ZendHashTableIterator<T> last(void)
	{
		HashPosition pos;
		zend_hash_internal_pointer_end_ex(this->m_hash, &pos);
		return ZendHashTableIterator<T>(this->m_hash, pos);
	}

private:
	HashTable* m_hash;
};

#endif /* HASHTABLE_H_ */
