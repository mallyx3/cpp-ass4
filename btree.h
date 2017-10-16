/*
*
 * The btree is a linked structure which operates much like
 * a binary search tree, save the fact that multiple client
 * elements are stored in a single node.  Whereas a single element
 * would partition the tree into two ordered subtrees, a node 
 * that stores m client elements partition the tree 
 * into m + 1 sorted subtrees.
 */

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <map>


// we better include the iterator
#include "btree_iterator.h"

// we do this to avoid compiler errors about non-template friends
// what do we do, remember? :)

template <typename T> class btree;
template <typename T>
std::ostream &operator<<(std::ostream &os, const btree<T> &tree);

template <typename T> 
class btree {
 public:
  /** Hmm, need some iterator typedefs here... friends? **/
    typedef btree_iterator<T>                                 iterator;
    typedef const_btree_iterator<T>                           const_iterator;
    typedef std::reverse_iterator<const_iterator>             const_reverse_iterator;
    typedef std::reverse_iterator<iterator>                   reverse_iterator;
    friend class btree_iterator<T>;
    friend class const_btree_iterator<T>;

  /**
   * Constructs an empty btree.  Note that
   * the elements stored in your btree must
   * have a well-defined copy constructor and destructor.
   * The elements must also know how to order themselves
   * relative to each other by implementing operator<
   * and operator==. (These are already implemented on
   * behalf of all built-ins: ints, doubles, strings, etc.)
   * 
   * @param maxNodeElems the maximum number of elements
   *        that can be stored in each B-Tree node
   */
   btree(size_t maxNodeElems = 40) {
      rootNode = std::make_shared<Node>(this, nullptr, maxNodeElems);
   };

  /**
   * The copy constructor and  assignment operator.
   * They allow us to pass around B-Trees by value.
   * Although these operations are likely to be expensive
   * they make for an interesting programming exercise.
   * Implement these operations using value semantics and 
   * make sure they do not leak memory.
   */

  /** 
   * Copy constructor
   * Creates a new B-Tree as a copy of original.
   *
   * @param original a const lvalue reference to a B-Tree object
   */
  btree(const btree<T>& original) {
    if (original.rootNode == nullptr) {
      rootNode = nullptr;
    } else {
      rootNode = std::make_shared<Node>(*original.rootNode);
      rootNode->changeRoot(this);
      rootNode->changeParent(nullptr);
    }
  }

  /** 
   * Move constructor
   * Creates a new B-Tree by "stealing" from original.
   *
   * @param original an rvalue reference to a B-Tree object
   */
  btree(btree<T>&& original): rootNode{std::move(original.rootNode)} {
    rootNode->changeRoot(this);
    rootNode->changeParent(nullptr);
  }
  
  
  /** 
   * Copy assignment
   * Replaces the contents of this object with a copy of rhs.
   *
   * @param rhs a const lvalue reference to a B-Tree object
   */
  btree<T>& operator=(const btree<T>& rhs) {
    if (this != &rhs) {
      rootNode.reset();
      btree<T> tmp{rhs};
      *this = std::move(tmp);
      rootNode->changeRoot(this);
      rootNode->changeParent(nullptr);
    }
    return *this;
  }

  /** 
   * Move assignment
   * Replaces the contents of this object with the "stolen"
   * contents of original.
   *
   * @param rhs a const reference to a B-Tree object
   */
  btree<T>& operator=(btree<T>&& rhs) {
    if (this != &rhs) {
      rootNode.reset();
      rootNode = std::move(rhs.rootNode);
      rootNode->changeRoot(this);
      rhs.rootNode = nullptr;
    }
    return *this;
  }

  /**
   * Puts a breadth-first traversal of the B-Tree onto the output
   * stream os. Elements must, in turn, support the output operator.
   * Elements are separated by space. Should not output any newlines.
   *
   * @param os a reference to a C++ output stream
   * @param tree a const reference to a B-Tree object
   * @return a reference to os
*/
   friend std::ostream& operator<<(std::ostream& os, const btree<T>& tree) {
      auto it = tree.cbegin();
      if (it != tree.cend()) {
        os << (*it);
      }
      for (; it != tree.cend(); ++it) {
        os << " " << (*it) ;
      }
      return os;
   }
  
   //Iterators
   const_reverse_iterator crbegin() const {
      return rbegin();
   }
   const_reverse_iterator crend() const {
      return rend();
   }
   const_iterator cbegin() const {
      return begin();
   }
   const_iterator cend() const {
      return end();
   }

   reverse_iterator rbegin() { 
        return reverse_iterator(end()); 
   }
   reverse_iterator rend() { 
        return reverse_iterator(begin()); 
   }
   const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end()); 
   }
   const_reverse_iterator rend() const { 
        return const_reverse_iterator(begin()); 
   }


   const_iterator begin() const {
      return rootNode->cNodeBegin();
   }
   const_iterator end() const {
      return rootNode->cNodeEnd();
   }
   iterator begin() {
        return rootNode->nodeBegin();
   }
   iterator end() { 
        return rootNode->nodeEnd();
   }
    

  /**
    * Returns an iterator to the matching element, or whatever 
    * the non-const end() returns if the element could 
    * not be found.  
    *
    * @param elem the client element we are trying to match.  The elem,
    *        if an instance of a true class, relies on the operator< and
    *        and operator== methods to compare elem to elements already 
    *        in the btree.  You must ensure that your class implements
    *        these things, else code making use of btree<T>::find will
    *        not compile.
    * @return an iterator to the matching element, or whatever the
    *         non-const end() returns if no such match was ever found.
    */
   iterator find(const T& elem) {
      return rootNode->nodeFind(elem);
   }

  /**
    * Identical in functionality to the non-const version of find, 
    * save the fact that what's pointed to by the returned iterator
    * is deemed as const and immutable.
    *
    * @param elem the client element we are trying to match.
    * @return an iterator to the matching element, or whatever the
    *         const end() returns if no such match was ever found.
    */
    const_iterator find(const T& elem) const {
      return rootNode->cNodeFind(elem);
    }
      
  /**
    * Operation which inserts the specified element
    * into the btree if a matching element isn't already
    * present.  In the event where the element truly needs
    * to be inserted, the size of the btree is effectively
    * increases by one, and the pair that gets returned contains
    * an iterator to the inserted element and true in its first and
    * second fields.  
    *
    * If a matching element already exists in the btree, nothing
    * is added at all, and the size of the btree stays the same.  The 
    * returned pair still returns an iterator to the matching element, but
    * the second field of the returned pair will store false.  This
    * second value can be checked to after an insertion to decide whether
    * or not the btree got bigger.
    *
    * The insert method makes use of T's copy constructor,
    * and if these things aren't available, 
    * then the call to btree<T>::insert will not compile.  The implementation
    * also makes use of the class's operator== and operator< as well.
    *
    * @param elem the element to be inserted.
    * @return a pair whose first field is an iterator positioned at
    *         the matching element in the btree, and whose second field 
    *         stores true if and only if the element needed to be added 
    *         because no matching element was there prior to the insert call.
    */
   std::pair<iterator, bool> insert(const T& elem) {
      return rootNode->nodeInsert(elem);
   }

  /**
    * Disposes of all internal resources, which includes
    * the disposal of any client objects previously
    * inserted using the insert operation. 
    * Check that your implementation does not leak memory!
    */
  ~btree() {
    rootNode.reset();
  }
  
private:
  struct Node {
      //Default constructor for Node
      Node(btree *b, Node *n, const size_t& size = 40): root{b}, parent{n}, children{size+1, nullptr}, maxSize{size} {
      }

      //Copy constructor for node
      Node(const Node& n): root{n.root}, parent{n.parent}, children{n.maxSize+1, nullptr}, maxSize{n.maxSize}, val{n.val} {
         if (n.children.size() == 0) {
            std::cout << "should never happen" << std::endl;
            return;
         }
         for (unsigned i = 0; i < n.children.size(); ++i) {
            if (n.children[i] != nullptr) {
               children[i] = (std::make_shared<Node>(*n.children[i]));
            }
         }
         
      }

      //Important for copy/move semantics. Since root/parent are pointers, have to recursively update
      //for the new btree
      void changeRoot(btree* b) {
         root = b;
         for (unsigned i = 0; i < children.size(); ++i) {
            if (children[i] != nullptr) {
               children[i]->changeRoot(b);
            }
         }
      }

      //Same as above
      void changeParent(Node* n) {
         parent = n;
         for (unsigned i = 0; i < children.size(); ++i) {
            if (children[i] != nullptr) {
               children[i]->changeParent(this);
            }
         }
      } 

      //Recursive helper for insert
      std::pair<iterator, bool> nodeInsert(const T& elem) {
         if (val.empty()) {
            val.push_back(elem);
            return std::pair<iterator, bool>(iterator(this, val.begin()), true);
         }
         auto itPos = std::lower_bound(val.begin(), val.end(), elem);
         auto pos = itPos - val.begin();

         if ((itPos != val.end()) && ((*itPos) == elem)) {
            return std::pair<iterator, bool>(iterator(this, itPos), false);
         } else if (static_cast<unsigned>(val.size()) < maxSize) {
            auto newIt = val.insert(itPos, elem);
            return std::pair<iterator, bool>(iterator(this, newIt), true);
         } else if (children[pos].get() != nullptr) {
            return children[pos]->nodeInsert(elem);
         } else {
            children[pos] = std::make_shared<Node>(root, this, maxSize);
            return children[pos]->nodeInsert(elem);
         }
      }
      

    //Recursive helper function for find
    iterator nodeFind(const T& elem) {
      auto itPos = std::lower_bound(val.begin(), val.end(), elem);
      auto pos = itPos - val.begin();

      if ((*itPos) == elem) {
        return iterator(this, itPos);
      } else if (children[pos].get() != nullptr) {            
        return children[pos]->nodeFind(elem);
      }
        return root->end();
    }    

    //Recursive helper function for const find
    const_iterator cNodeFind(const T& elem) {
      auto itPos = std::lower_bound(val.begin(), val.end(), elem);
      auto pos = itPos - val.begin();

      if ((*itPos) == elem) {
        return const_iterator(this, itPos);
      } else if (children[pos].get() != nullptr) {
        return children[pos]->cNodeFind(elem);
      }

      return root->cend();
    }

    //Recursive function for begin()
    iterator nodeBegin() {
        if (children[0].get() != nullptr) {
           return children[0]->nodeBegin();
        } else {
            return iterator(this, val.begin());
        }
    }

    //Recursive const function for cbegin()
    const_iterator cNodeBegin() const {
        if (children[0].get() != nullptr) {
           return children[0]->cNodeBegin();
        } else {
            return const_iterator(this, val.begin());
        }
    }
  
  
    //Recursive function for end()
    iterator nodeEnd()  {
       if (children[val.size()].get() != nullptr) {
          return children[val.size()]->nodeEnd();
        } else {
          return iterator(this, val.end());
        }
    }

    //Recursive const function for cend()
    const_iterator cNodeEnd() const {
       if (children[val.size()].get() != nullptr) {
          return children[val.size()]->cNodeEnd();
        } else {
          return const_iterator(this, val.end());
        }
    }
     
    btree *root;
    Node *parent;
    std::vector<std::shared_ptr<Node>> children;
    const size_t maxSize;
    std::vector<T> val;
  };


  std::shared_ptr<Node> rootNode;
    
  // The details of your implementation go here
};

#endif
