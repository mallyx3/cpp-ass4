#ifndef BTREE_ITERATOR_H
#define BTREE_ITERATOR_H

#include <iterator>

template <typename T> class btree;
template <typename T>class const_btree_iterator; 

template <typename T>
class btree_iterator {
public:
	friend class const_btree_iterator<T>;
	using valIterator = typename std::vector<T>::iterator;
	typedef std::ptrdiff_t  						difference_type;
	typedef std::bidirectional_iterator_tag 		iterator_category;
	typedef T 										value_type;
    typedef T* 										pointer;
    typedef T& 										reference;

    btree_iterator& operator++();
    btree_iterator operator++(int);
    btree_iterator& operator--();
    btree_iterator operator--(int);
    bool operator==(const btree_iterator<T>&) const;
    bool operator!=(const btree_iterator<T>&) const;
    bool operator==(const const_btree_iterator<T>&) const;
    bool operator!=(const const_btree_iterator<T>&) const;
    reference operator*() const; 
    pointer operator->() const; 

    btree_iterator(typename btree<T>::Node *pointee, valIterator v): ptr{pointee}, pos{v} {}

private:
	typename btree<T>::Node *ptr;
	valIterator pos;
};

template <typename T>
class const_btree_iterator {
public:
	friend class btree_iterator<T>;
	using valIterator = typename std::vector<T>::const_iterator;
	typedef std::ptrdiff_t  						difference_type;
	typedef std::bidirectional_iterator_tag 		iterator_category;
	typedef const T 								value_type;
    typedef const T* 								pointer;
    typedef const T& 								reference;

    const_btree_iterator& operator++();
    const_btree_iterator operator++(int);
    const_btree_iterator& operator--();
    const_btree_iterator operator--(int);
    bool operator==(const const_btree_iterator<T>&) const;
    bool operator!=(const const_btree_iterator<T>&) const;
    bool operator==(const btree_iterator<T>&) const;
    bool operator!=(const btree_iterator<T>&) const;
    reference operator*() const { return (*pos); }
    pointer operator->() const {return &(operator*()); }

    const_btree_iterator(const typename btree<T>::Node *pointee, valIterator v): ptr{pointee}, pos{v} {}

private:
	const typename btree<T>::Node *ptr;
	valIterator pos;
};
/**
 * You MUST implement the btree iterators as (an) external class(es) in this file.
 * Failure to do so will result in a total mark of 0 for this deliverable.
 **/

// iterator related interface stuff here; would be nice if you called your
// iterator class btree_iterator (and possibly const_btree_iterator)


template <typename T>
const_btree_iterator<T>& const_btree_iterator<T>::operator++() {
	T temp = (*pos);
	int offset = (pos - ptr->val.begin()) + 1;
	if (ptr->children[offset] != nullptr) {
		ptr = ptr->children[offset].get();
		while (ptr->children[0].get() != nullptr) {
           ptr = ptr->children[0].get();
        } 
        pos = ptr->val.begin();
	} else if (++pos == ptr->val.end()) {
		while ((*this) != ptr->root->cend()) {
			ptr = ptr->parent;
			pos = std::lower_bound(ptr->val.begin(), ptr->val.end(), temp);
			if (pos != ptr->val.end()) {
				break;
			}
		}
	}
	return *this;
}

template <typename T>
const_btree_iterator<T> const_btree_iterator<T>::operator++(int) {
	const_btree_iterator<T> tmp {*this};
	operator++();
	return tmp;
}

template<typename T>
const_btree_iterator<T>& const_btree_iterator<T>::operator--() {
	T temp = (*pos);
	int offset = (pos - ptr->val.begin());
	if (ptr->children[offset] != nullptr) {
		ptr = ptr->children[offset].get();
		while (ptr->children[ptr->val.size()].get() != nullptr) {
           ptr = ptr->children[ptr->val.size()].get();
        } 
        pos = ptr->val.end();
        --pos;
	} else if (pos == ptr->val.begin()) {
		while ((*this) != ptr->root->cbegin()) {
			ptr = ptr->parent;
			pos = std::lower_bound(ptr->val.begin(), ptr->val.end(), temp);
			if (pos != ptr->val.begin()) {
				--pos;
				break;
			}
		}
	} else {
		--pos;
	}
	return *this;
}

template <typename T>
const_btree_iterator<T> const_btree_iterator<T>::operator--(int) {
	const_btree_iterator<T> tmp {*this};
	operator--();
	return tmp;
}

template <typename T>
bool const_btree_iterator<T>::operator==(const btree_iterator<T>& other) const {
	return this->pos == other.pos;
}

template <typename T>
bool const_btree_iterator<T>::operator!=(const btree_iterator<T>& other) const {
	return (!operator==(other));
}

template <typename T>
bool const_btree_iterator<T>::operator==(const const_btree_iterator<T>& other) const {
	return this->pos == other.pos;
}

template <typename T>
bool const_btree_iterator<T>::operator!=(const const_btree_iterator<T>& other) const {
	return (!operator==(other));
}

template <typename T>
T& btree_iterator<T>::operator*() const { return (*pos); }

template <typename T>
T* btree_iterator<T>::operator->() const { return &(operator*()); }

template <typename T>
btree_iterator<T>& btree_iterator<T>::operator++() {
	T temp = (*pos);
	int offset = (pos - ptr->val.begin()) + 1;
	if (ptr->children[offset] != nullptr) {
		ptr = ptr->children[offset].get();
		while (ptr->children[0].get() != nullptr) {
           ptr = ptr->children[0].get();
        } 
        pos = ptr->val.begin();
	} else if (++pos == ptr->val.end()) {
		while ((*this) != ptr->root->end()) {
			ptr = ptr->parent;
			pos = std::lower_bound(ptr->val.begin(), ptr->val.end(), temp);
			if (pos != ptr->val.end()) {
				break;
			}
		}
	}
	return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator++(int) {
	btree_iterator<T> tmp {*this};
	operator++();
	return tmp;
}

template<typename T>
btree_iterator<T>& btree_iterator<T>::operator--() {
	T temp = (*pos);
	int offset = (pos - ptr->val.begin());
	if (ptr->children[offset] != nullptr) {
		ptr = ptr->children[offset].get();
		while (ptr->children[ptr->val.size()].get() != nullptr) {
           ptr = ptr->children[ptr->val.size()].get();
        } 
        pos = ptr->val.end();
        --pos;
	} else if (pos == ptr->val.begin()) {
		while ((*this) != ptr->root->begin()) {
			ptr = ptr->parent;
			pos = std::lower_bound(ptr->val.begin(), ptr->val.end(), temp);
			if (pos != ptr->val.begin()) {
				--pos;
				break;
			}
		}
	} else {
		--pos;
	}
	return *this;
}

template <typename T>
btree_iterator<T> btree_iterator<T>::operator--(int) {
	btree_iterator<T> tmp {*this};
	operator--();
	return tmp;
}

template <typename T>
bool btree_iterator<T>::operator==(const btree_iterator<T>& other) const {
	return this->pos == other.pos;
}

template <typename T>
bool btree_iterator<T>::operator!=(const btree_iterator<T>& other) const {
	return (!operator==(other));
}

template <typename T>
bool btree_iterator<T>::operator==(const const_btree_iterator<T>& other) const {
	return this->pos == other.pos;
}

template <typename T>
bool btree_iterator<T>::operator!=(const const_btree_iterator<T>& other) const {
	return (!operator==(other));
}

#endif
