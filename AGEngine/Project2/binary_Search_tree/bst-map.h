#ifndef BSTMAP_H
#define BSTMAP_H
//#define MY_TEST
#include <iostream>
#include <queue>
#include <stack>
namespace CS280 {

	template< typename KEY_TYPE, typename VALUE_TYPE >
	class BSTmap {
	public:
		class Node {
		public:
			Node(KEY_TYPE k, VALUE_TYPE val, Node* p, int h, int b, Node* l, Node* r);

			Node(const Node&) = delete;
			Node* operator=(const Node&) = delete;

			KEY_TYPE const& Key() const;   // return a const reference
			VALUE_TYPE& Value();       // return a reference
			Node*  last();
			Node* first();
			Node* decrement();
			Node* next();
			Node* getRoot();
		private:
			KEY_TYPE    key;
			VALUE_TYPE  value;
			int         height, balance; // optional
			Node* parent;
			Node* left;
			Node* right;

			friend class BSTmap;
		};
	private:
		struct BSTmap_iterator {
		private:
			Node* p_node;
		public:
			BSTmap_iterator(Node* p = nullptr);
			BSTmap_iterator& operator=(const BSTmap_iterator& rhs);
			BSTmap_iterator& operator++();
			BSTmap_iterator operator++(int);
			Node& operator*();
			Node* operator->();
			bool operator!=(const BSTmap_iterator& rhs);
			bool operator==(const BSTmap_iterator& rhs);
			friend class BSTmap;
		};
		struct BSTmap_iterator_const {
		private:
			Node* p_node;
		public:
			BSTmap_iterator_const(Node* p = nullptr);
			BSTmap_iterator_const& operator=(const BSTmap_iterator_const& rhs);
			BSTmap_iterator_const& operator++();
			BSTmap_iterator_const operator++(int);
			Node const& operator*();
			Node const* operator->();
			bool operator!=(const BSTmap_iterator_const& rhs);
			bool operator==(const BSTmap_iterator_const& rhs);
			friend class BSTmap;
		};
		// BSTmap implementation
		Node* pRoot = nullptr;
		unsigned int size_ = 0;
		// end iterators are same for all BSTmaps, thus static
		// make BSTmap_iterator a friend
		// to allow BSTmap_iterator to access end iterators 
		static BSTmap_iterator end_it;
		static BSTmap_iterator_const const_end_it;
		void DeleteData();
	public:
		//BIG FOUR
		BSTmap();
		BSTmap(const BSTmap& rhs);
		BSTmap& operator=(const BSTmap& rhs);
		virtual ~BSTmap();

		unsigned int size();

		//value setter and getter
		VALUE_TYPE& operator[](KEY_TYPE const& key);
		//next method doesn't make sense
		//because operator[] inserts a non-existing element
		//which is not allowed on const maps
		//VALUE_TYPE operator[](int key) const;

		//standard names for iterator types
		typedef BSTmap_iterator iterator;
		typedef BSTmap_iterator_const const_iterator;

		//BSTmap methods dealing with non-const iterator 
		BSTmap_iterator begin();
		BSTmap_iterator end();
		BSTmap_iterator find(KEY_TYPE const& key);
		void erase(BSTmap_iterator it);

		//BSTmap methods dealing with const iterator 
		BSTmap_iterator_const begin() const;
		BSTmap_iterator_const end() const;
		BSTmap_iterator_const find(KEY_TYPE const& key) const;
		//do not need this one (why)
		//BSTmap_iterator_const erase(BSTmap_iterator& it) const;

		void print(std::ostream& os, bool print_value = false) const;
		bool sanityCheck();
		char getedgesymbol(const Node* node) const;
		int getdepth(const Node* node) const;
		//inner class (BSTmap_iterator) doesn't have any special priveleges
		//in accessing private data/methods of the outer class (BSTmap)
		//so need friendship to allow BSTmap_iterator to access private "BSTmap::end_it"
		//BTW - same is true for outer class accessing inner class private data
		friend class BSTmap_iterator;
		friend class BSTmap_iterator_const;
	private:
		// ...
	};

	//notice that it doesn't need to be friend
	template< typename KEY_TYPE, typename VALUE_TYPE >
	std::ostream& operator<<(std::ostream& os, BSTmap<KEY_TYPE, VALUE_TYPE> const& map);

	//////////////node
	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Node(KEY_TYPE k, VALUE_TYPE val, Node* p, int h, int b, Node* l, Node* r) :key(k), value(val),  height(h), balance(b), parent(p), left(l), right(r)
	{

	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	KEY_TYPE const& BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Key() const
	{
		return key;
	}
	template<typename KEY_TYPE, typename VALUE_TYPE>
	VALUE_TYPE& BSTmap<KEY_TYPE, VALUE_TYPE>::Node::Value()
	{
		return value;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::Node::last()
	{
		
		std::queue<Node*> que;
		que.push(this);
		Node* node = this;
		while (node != nullptr)
		{
			if (node->right == nullptr)
			{
				return node;
			}
			node = node->right;
		}
		
		return node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::Node::first()
	{
		std::queue<Node*> que;
		que.push(this);
		Node* node = this;
		while (node != nullptr)
		{
			if (node->left == nullptr)
			{
				return node;
			}	
			node = node->left;
		}

		return node->parent;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::Node::decrement()
	{
		if (left == nullptr)
		{
			Node* p = parent;
			while (p != nullptr)
			{
				if (p->key < key)
				{
					return p;
				}
				p = p->parent;
			}
			return nullptr;
		}
		
		Node* p = left;
		while (p!= nullptr)
		{
			if (p->right == nullptr)
			{
				return p;
			}
			p = p->right;
		}
		return nullptr;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::Node::next()
	{
		if (right == nullptr)
		{
			Node* p = parent;
			while (p!= nullptr)
			{
				if (p->key > key)
				{
					return p;
				}
				p = p->parent;
			}
			return nullptr;
		}

		Node* n = right;
		while (right != nullptr) {			
			if (n->left != nullptr)
			{
				n = n->left;
			}
			else
			{
				return n;
			}
		}
		return nullptr;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::Node::getRoot()
	{
		Node* p = this;
		while (p != nullptr) {
			if (p->parent == nullptr)
			{
				return p;
			}
			p = p->parent;		
		}
	}

	////////////BSTmap_iterator
	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::BSTmap_iterator(Node* p) :p_node(p)
	{

	}
	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator=(const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& rhs)
	{
		p_node = rhs.p_node;
		return *this;
	}
	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++()
	{
		p_node = p_node->next();
		return *this;
	}
	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator++(int)
	{
		BSTmap_iterator P =BSTmap_iterator(p_node);
		p_node = p_node->next();
		return P;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator*()
	{
		return *p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node* BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator->()
	{
		return p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	bool BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator!=(const  BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& rhs)
	{
		return p_node != rhs.p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	bool BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator::operator==(const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator& rhs)
	{
		return p_node == rhs.p_node;
	}

	//////////////const iterator
	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::BSTmap_iterator_const(Node* p) :p_node(p)
	{
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator=(const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const& rhs)
	{
		p_node = rhs.p_node;
		return *this;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator++()
	{
		p_node = p_node->next();
		return *this;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator++(int i)
	{
		BSTmap_iterator_const p(p_node);
		p_node = p_node->next();
		return p;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node const& BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator*()
	{
		return *p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::Node const* BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator->()
	{
		return p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	bool BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator!=(const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const& rhs)
	{
		return p_node != p_node;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	bool BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const::operator==(const BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const& rhs)
	{
		return p_node == p_node;
	}




	// static data members
	
	template<typename KEY_TYPE, typename VALUE_TYPE>
	void BSTmap<KEY_TYPE, VALUE_TYPE>::DeleteData()
	{
		if (pRoot == nullptr)
		{
			return;
		}
		std::queue<Node*> que;
		que.push(pRoot);
		while (!que.empty()) {
			Node* n = que.front();
			que.pop();
			if (n->left != nullptr)
			{
				que.push(n->left);
			}
			if (n->right != nullptr)
			{
				que.push(n->right);
			}			
			delete n;
		}
		pRoot = nullptr;
		size_ = 0;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap() :pRoot(nullptr), size_(0)
	{

	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap(const BSTmap& rhs) :pRoot(nullptr), size_(rhs.size_)
	{
		Node* p = rhs.pRoot;
		if (p == nullptr)
		{
			return;
		}
		pRoot = new BSTmap<KEY_TYPE, VALUE_TYPE>::Node(p->key, p->value, p->parent, p->height, p->balance, nullptr, nullptr);
		//Node(KEY_TYPE k, VALUE_TYPE val, Node * p, int h, int b, Node * l, Node * r);
		std::queue<Node*> que;
		std::queue<Node*> queInsert;
		que.push(rhs.pRoot);
		queInsert.push(pRoot);
		while (!que.empty()) {
			Node* n = que.front();
			que.pop();
			Node* nn = queInsert.front();
			queInsert.pop();
			if (n != nullptr) {
				if (n->left != nullptr)
				{
					Node* l = n->left;
					nn->left = new  BSTmap<KEY_TYPE, VALUE_TYPE>::Node(l->key, l->value, nn, l->height, l->balance, nullptr, nullptr);
					que.push(n->left);
					queInsert.push(nn->left);
				}

				if (n->right != nullptr)
				{
					Node* l = n->right;
					nn->right = new  BSTmap<KEY_TYPE, VALUE_TYPE>::Node(l->key, l->value, nn, l->height, l->balance, nullptr, nullptr);
					que.push(n->right);
					queInsert.push(nn->right);
				}
			}
		}
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>& BSTmap<KEY_TYPE, VALUE_TYPE>::operator=(const BSTmap& rhs)
	{
		DeleteData();
		size_ = rhs.size_;
		Node* p = rhs.pRoot;
		if (p == nullptr)
		{
			return *this;
		}
		pRoot = new BSTmap<KEY_TYPE, VALUE_TYPE>::Node(p->key, p->value, nullptr, p->height, p->balance, nullptr, nullptr);
		//Node(KEY_TYPE k, VALUE_TYPE val, Node * p, int h, int b, Node * l, Node * r);
		std::queue<Node*> que;
		std::queue<Node*> queInsert;
		que.push(rhs.pRoot);
		queInsert.push(pRoot);
		while (!que.empty()) {
			Node* n = que.front();
			que.pop();
			Node* nn = queInsert.front();
			queInsert.pop();
			if (n != nullptr) {
				if (n->left != nullptr)
				{
					Node* l = n->left;
					nn->left = new  BSTmap<KEY_TYPE, VALUE_TYPE>::Node(l->key, l->value, nn, l->height, l->balance, nullptr, nullptr);
					que.push(n->left);
					queInsert.push(nn->left);
				}

				if (n->right != nullptr)
				{
					Node* l = n->right;
					nn->right = new  BSTmap<KEY_TYPE, VALUE_TYPE>::Node(l->key, l->value, nn, l->height, l->balance, nullptr, nullptr);
					que.push(n->right);
					queInsert.push(nn->right);
				}
			}
		}
		return *this;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	BSTmap<KEY_TYPE, VALUE_TYPE>::~BSTmap()
	{
		DeleteData();

	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	unsigned int BSTmap<KEY_TYPE, VALUE_TYPE>::size()
	{
		return size_;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	VALUE_TYPE& BSTmap<KEY_TYPE, VALUE_TYPE>::operator[](KEY_TYPE const& key)
	{
		if (pRoot == nullptr)
		{
			pRoot = new BSTmap<KEY_TYPE, VALUE_TYPE>::Node(key, VALUE_TYPE(), nullptr, 0, 0, nullptr, nullptr);
			size_++;
			return pRoot->value;
		}
		std::queue<Node*> que;
		que.push(pRoot);
		while (!que.empty()) {
			Node* n = que.front();
			que.pop();
			if (n != nullptr)
			{
				if (n->key == key)
				{
					return n->value;
				}
				if (key > n->key)
				{
					if (n->right == nullptr)
					{
						n->right = new BSTmap<KEY_TYPE, VALUE_TYPE>::Node(key, VALUE_TYPE(), n, n->height + 1, 0, nullptr, nullptr);
						size_++;
						n->balance++;
						return n->right->value;
					}
					else
					{
						que.push(n->right);
					}
				}
				else
				{
					if (n->left == nullptr)
					{
						n->left = new BSTmap<KEY_TYPE, VALUE_TYPE>::Node(key, VALUE_TYPE(), n, n->height + 1, 0, nullptr, nullptr);
						size_++;
						n->balance -- ;
						return n->left->value;
					}
					else
					{
						que.push(n->left);
					}
				}
			}
		}
		return pRoot->value;
	}

	////////////////////////////////////////////////////////////
	// typename is used to help compiler to parse
	template< typename KEY_TYPE, typename VALUE_TYPE >
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator BSTmap<KEY_TYPE, VALUE_TYPE>::begin() {
		if (pRoot) return BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator(pRoot->first());
		else       return end_it;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator BSTmap<KEY_TYPE, VALUE_TYPE>::end()
	{
		return end_it;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const& key)
	{
		if (pRoot == nullptr)
		{
			return end_it;
		}
		Node* pNode = pRoot;
		while (pNode != nullptr) {
			
			if (pNode->key == key)
			{
				return BSTmap_iterator(pNode);
			}

			if (pNode->key > key)
			{
				pNode = pNode->left;
			}
			else
			{
				pNode = pNode->right;
			}
		}
		return end_it;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	void BSTmap<KEY_TYPE, VALUE_TYPE>::erase(BSTmap_iterator it)
	{
		if (it == end_it)
		{
			return;
		}

		if (it.p_node->left != nullptr && it.p_node->right != nullptr)
		{
			Node* pNext = it.p_node->next();
			int key = it.p_node->key;
			int value = it.p_node->value;
			it.p_node->key = pNext->key;
			it.p_node->value = pNext->value;
			pNext->key = key;
			pNext->value = value;
			erase(BSTmap_iterator(pNext));
		}
		else if(it.p_node->left != nullptr)
		{
			Node* parent = it.p_node->parent;
			it.p_node->left->parent = parent;
			if (parent != nullptr)
			{
				if (parent->left == it.p_node)
				{
					parent->left = it.p_node->left;
				}
				else
				{
					parent->right = it.p_node->left;
				}
			}
			else
			{
				pRoot = it.p_node->left;
			}
			delete it.p_node;
			size_--;
		}
		else if(it.p_node->right != nullptr)
		{
			Node* parent = it.p_node->parent;
			it.p_node->right->parent = parent;
			if (parent != nullptr)
			{
				if (parent->left == it.p_node)
				{
					parent->left = it.p_node->right;
				}
				else
				{
					parent->right = it.p_node->right;
				}
			}
			else
			{
				pRoot = it.p_node->right;
			}
			delete it.p_node;
			size_--;
		}
		else
		{
			if (it.p_node->parent == nullptr)
			{
				delete pRoot;
				pRoot = nullptr;
				size_--;
			}
			else
			{

				if (it.p_node->parent->left == it.p_node)
				{
					it.p_node->parent->left = nullptr;
				}
				else
				{
					it.p_node->parent->right = nullptr;
				}
				delete it.p_node;
				size_--;
			}
			
		}

	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const BSTmap<KEY_TYPE, VALUE_TYPE>::begin() const
	{
		return BSTmap_iterator_const(pRoot);
	}


	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const BSTmap<KEY_TYPE, VALUE_TYPE>::end() const
	{
		return const_end_it;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	typename BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const BSTmap<KEY_TYPE, VALUE_TYPE>::find(KEY_TYPE const& key) const
	{
		if (pRoot == nullptr)
		{
			return end_it;
		}
		std::queue<Node*> que;
		que.push(pRoot);
		while (!que.empty()) {
			Node* n = que.front();
			que.pop();
			if (n->key == key)
			{
				return BSTmap_iterator_const(n);
			}

			if (n->left != nullptr)
			{
				que.push(n->left);
			}
			if (n->right != nullptr)
			{
				que.push(n->right);
			}
		}
		return const_end_it;
	}

	template<typename KEY_TYPE, typename VALUE_TYPE>
	int BSTmap<KEY_TYPE, VALUE_TYPE>::getdepth(const Node* node) const
	{
		int depth = -1;
		while (node != nullptr)
		{
			node = node->parent;
			depth++;
		}
		return depth;
	}

#ifdef MY_TEST
	template< typename KEY_TYPE, typename VALUE_TYPE >
	typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator
		CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::end_it = CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator(nullptr);

	template< typename KEY_TYPE, typename VALUE_TYPE >
	typename CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const
		CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::const_end_it = CS280::BSTmap<KEY_TYPE, VALUE_TYPE>::BSTmap_iterator_const(nullptr);
	////////////////////////////////////////////////////////////
	// do not change this code from here to the end of the file
	/* figure out whether node is left or right child or root
	 * used in print_backwards_padded
	 */
	template< typename KEY_TYPE, typename VALUE_TYPE >
	char BSTmap<KEY_TYPE, VALUE_TYPE>::getedgesymbol(const Node* node) const {
		const Node* parent = node->parent;
		if (parent == nullptr) return '-';
		else                 return (parent->left == node) ? '\\' : '/';
	}


	/* this is another "ASCII-graphical" print, but using
	 * iterative function.
	 * Left branch of the tree is at the bottom
	 */
	template< typename KEY_TYPE, typename VALUE_TYPE >
	std::ostream& operator<<(std::ostream& os, BSTmap<KEY_TYPE, VALUE_TYPE> const& map) {
		map.print(os);
		return os;
	}

	template< typename KEY_TYPE, typename VALUE_TYPE >
	void BSTmap<KEY_TYPE, VALUE_TYPE>::print(std::ostream& os, bool print_value) const {
		if (pRoot) {
			BSTmap<KEY_TYPE, VALUE_TYPE>::Node* b = pRoot->last();
			while (b) {
				int depth = getdepth(b);
				int i;
				/* printf(b); */

				char edge = getedgesymbol(b);
				switch (edge) {
				case '-':
					for (i = 0; i < depth; ++i) std::printf("       ");
					os << b->key;
					if (print_value) { os << " -> " << b->value; }
					os << std::endl;
					break;
				case '\\':
					for (i = 0; i < depth; ++i) std::printf("       ");
					os << edge << std::endl;
					for (i = 0; i < depth; ++i) std::printf("       ");
					os << b->key;
					if (print_value) { os << " -> " << b->value; }
					os << std::endl;
					break;
				case '/':
					for (i = 0; i < depth; ++i) std::printf("       ");
					os << b->key;
					if (print_value) { os << " -> " << b->value; }
					os << std::endl;
					for (i = 0; i < depth; ++i) std::printf("       ");
					os << edge << std::endl;
					break;
				}
				b = b->decrement();
			}
		}
		std::printf("\n");
	}
#endif

	
	template<typename KEY_TYPE, typename VALUE_TYPE>
	bool BSTmap<KEY_TYPE, VALUE_TYPE>::sanityCheck()
	{
		return false;
	}
}

#ifndef MY_TEST
#include "bst-map.cpp"
#endif // !MY_TEST


#endif
