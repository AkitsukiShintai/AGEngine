////////////////////////////////////////////////////////////////////////////////
#ifndef LARIAT_H
#define LARIAT_H
////////////////////////////////////////////////////////////////////////////////

#include <string>     // error strings
#include <utility>    // error strings
#include <cstring>     // memcpy

class LariatException : public std::exception {
private:
	int m_ErrCode;
	std::string m_Description;

public:
	LariatException(int ErrCode, const std::string& Description) :
		m_ErrCode(ErrCode), m_Description(Description) {}

	virtual int code(void) const {
		return m_ErrCode;
	}

	virtual const char* what(void) const throw() {
		return m_Description.c_str();
	}

	virtual ~LariatException() throw() {
	}

	enum LARIAT_EXCEPTION { E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR };
};

// forward declaration for 1-1 operator<< 
template<typename T, int Size>
class Lariat;

template<typename T, int Size>
std::ostream& operator<< (std::ostream& os, Lariat<T, Size> const& rhs);

template <typename T, int Size>
class Lariat
{
public:

	Lariat();                  // default constructor                        
	Lariat(Lariat const& rhs); // copy constructor		
	~Lariat(); // destructor
	// more ctor(s) and assignment(s)
	template<typename T2, int Size2>
	Lariat<T, Size>(Lariat<T2, Size2> const& rhs); // copy constructor

	Lariat& operator=(const Lariat& rhs);

	template<typename T2, int Size2>
	Lariat<T, Size>& operator=(const Lariat<T2, Size2>& rhs);

	// inserts
	void insert(int index, const T& value);
	void push_back(const T& value);
	void push_front(const T& value);

	// deletes
	void erase(int index);
	void pop_back();
	void pop_front();
	//access
	T& operator[](int index);       // for l-values
	const T& operator[](int index) const; // for r-values
	T& first();
	T const& first() const;
	T& last();
	T const& last() const;

	unsigned find(const T& value) const;       // returns index, size (one past last) if not found

	friend std::ostream& operator<< <T, Size>(std::ostream& os, Lariat<T, Size> const& list);

	// and some more
	size_t size(void) const;   // total number of items (not nodes)
	void clear(void);          // make it empty
	void compact();             // push data in front reusing empty positions and delete remaining nodes
	bool Check();
public:
	template<typename U, int Size2>  //[nested template declaration]
	friend class Lariat;
	struct LNode { // DO NOT modify provided code
		LNode* next = nullptr;
		LNode* prev = nullptr;
		int    count = 0;         // number of items currently in the node
		T values[Size];
	};
	LNode* findNodeAndLoacalIndex(int index, int& localIndex);
	void ShiftUp(LNode* node, int index);
	void ShiftDown(LNode* node, int index);
	void Split(LNode* node,const T& value, int localIndex);
	void CreateHead();
	// DO NOT modify provided code
	LNode* head_;           // points to the first node
	LNode* tail_;           // points to the last node
	int size_;              // the number of items (not nodes) in the list
	mutable int nodecount_; // the number of nodes in the list
	int asize_;             // the size of the array within the nodes

};


template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(nullptr), tail_(nullptr), size_(0), nodecount_(0), asize_(Size)
{

}

template<typename T, int Size>
Lariat<T, Size>::Lariat(Lariat const& rhs) : head_(nullptr), tail_(nullptr), size_(rhs.size_), nodecount_(0), asize_(Size)
{
	LNode* start = rhs.head_;
	while (start != nullptr)
	{
		for (int i = 0; i < asize_; i++)
		{
			push_back(start->values[i]);
		}
		start = start->next;
	}
}

template<typename T, int Size>
template<typename T2, int Size2>
 Lariat<T, Size>::Lariat(Lariat<T2, Size2> const& rhs) : head_(nullptr), tail_(nullptr), size_(rhs.size_), nodecount_(0), asize_(Size)
{
	typename Lariat<T2, Size2>::LNode* start = rhs.head_;
	while (start != nullptr)
	{
		for (int i = 0; i < Size2; i++)
		{
			push_back(static_cast<T>(start->values[i]));
		}
		start = start->next;
	}
}



template<typename T, int Size>
Lariat<T, Size>::~Lariat()
{
	clear();
}

template<typename T, int Size>
Lariat<T, Size>& Lariat<T, Size>::operator=(const Lariat& rhs)
{
	clear();
	LNode* start = rhs.head_;
	while (start != nullptr)
	{
		for (int i = 0; i < asize_; i++)
		{
			push_back(start->values[i]);
		}
		start = start->next;
	}
	return *this;
}

template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T& value)
{
	if (index > size_)
	{
		throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
	}

	if (index == 0)
	{
		push_front(value);
		return;
	}
	if (index == size_ )
	{
		push_back(value);
		return;
	}


	int localIndex = 0;
	LNode* node = findNodeAndLoacalIndex(index, localIndex);
	if (node-> count == asize_)
	{
		Split(node, value, localIndex);
		if (node == tail_)
		{
			tail_ = node->next;
		}
 	}
	else
	{
		//move all right value
		ShiftUp(node, localIndex);
		//insert value
		node->values[localIndex] = value;
		//count ++
		node->count++;
		size_++;
	}
	
}

template<typename T, int Size>
void Lariat<T, Size>::push_back(const T& value)
{
	if (head_ == nullptr)
	{
		CreateHead();
		head_->values[0] = value;
		head_->count++;
		size_++;
		return;
	}
	if (tail_->count == asize_)//full, create new node
	{
		Split(tail_, value, tail_->count);
		tail_ = tail_->next;
		return;
	}
	tail_->values[tail_->count] = value;
	tail_->count++;
	size_++;
}

template<typename T, int Size>
void Lariat<T, Size>::push_front(const T& value)
{
	if (head_ == nullptr)
	{
		CreateHead();
		head_->values[0] = value;
		head_->count++;
		size_++;
		return;
	}
	if (head_->count == asize_)
	{
		Split(head_, value, 0);
		if (head_ == tail_)
		{
			tail_ = head_->next;
		}
		return;
	}
	ShiftUp(head_, 0);
	head_->values[0] = value;
	head_->count++;
	size_++;
}

template<typename T, int Size>
void Lariat<T, Size>::erase(int index)
{
	int localIndex = 0;
	LNode* node = findNodeAndLoacalIndex(index, localIndex);
	ShiftDown(node, localIndex);
	node->count--;
	size_--;
}

template<typename T, int Size>
void Lariat<T, Size>::pop_back()
{
	if (head_ == nullptr)
	{
		return;
	}

	//ShiftDown(tail_, tail_->count - 1);
	tail_->count--;
	if (tail_->count == 0)
	{
		if (tail_ == head_)
		{
			delete tail_;
			head_ = nullptr;
			tail_ = nullptr;
		}
		else
		{
			LNode* node = tail_;
			tail_ = tail_->prev;
			tail_->next = nullptr;
			delete node;
			
		}
	}

	size_--;
}

template<typename T, int Size>
void Lariat<T, Size>::pop_front()
{
	if (head_ == nullptr)
	{
		return;
	}
	ShiftDown(head_, 0);
	head_->count--;
	if (head_->count == 0)
	{
		
		if (tail_ != head_)
		{
			LNode* h = head_;
			head_ = head_->next;
			delete h;
		}
		else
		{
			delete head_;
			head_ = nullptr;
			tail_ = nullptr;
		}
		
	}	
	size_--;
}

template<typename T, int Size>
template<typename T2, int Size2>
Lariat<T, Size>& Lariat<T, Size>::operator=(const Lariat<T2, Size2>& rhs) {

	clear();
	typename Lariat<T2, Size2>::LNode* start = rhs.head_;
	while (start != nullptr)
	{
		for (int i = 0; i < Size2; i++)
		{
			push_back(static_cast<T>(start->values[i]));
		}
		start = start->next;
	}
	return *this;
}


template<typename T, int Size>
T& Lariat<T, Size>::operator[](int index)
{

	int remain = -1;
	typename Lariat<T, Size>::LNode* node = findNodeAndLoacalIndex(index, remain);
	
	return node->values[remain];

}

template<typename T, int Size>
const T& Lariat<T, Size>::operator[](int index) const
{
	int count = index / asize_;
	int remain = index % asize_;
	LNode* node = head_;
	while (count > 0 && node != nullptr)
	{
		node = node->next;
	}
	if (node == nullptr)
	{
		throw LariatException(1, "Bad Index");
	}

	return node->values[remain];
}

template<typename T, int Size>
size_t Lariat<T, Size>::size(void) const
{
	return (size_t)size_;
}

template<typename T, int Size>
void Lariat<T, Size>::clear()
{
	while (head_ != nullptr)
	{
		LNode* t = head_;
		head_ = head_->next;
		delete t;
	}
	size_ = 0;
	head_ = nullptr;
	tail_ = nullptr;
}

template<typename T, int Size>
void Lariat<T, Size>::compact()
 {
	if (head_ == nullptr)
	{
		return;
	}
	//std::cout << *this << std::endl;
	LNode* insertNode = head_;
	LNode* nextValueNode = head_;
	int localIndexOfInsertNode = 0;
	int localIndexOfNextValue = 0;
	int deadCount = 0;
	//find first insert position
	while (insertNode != nullptr) {
		if (insertNode->count < asize_)
		{
			localIndexOfInsertNode = insertNode->count;
			nextValueNode = insertNode->next;
			localIndexOfNextValue = 0;
			break;
		}
		insertNode = insertNode->next;
	}
	
	while (nextValueNode != nullptr && insertNode != nullptr)
	{
		if (insertNode != nextValueNode)
		{
			insertNode->values[localIndexOfInsertNode] = nextValueNode->values[localIndexOfNextValue];
			insertNode->count++;

			//check if need to jump to next node
			localIndexOfInsertNode++;
			if (localIndexOfInsertNode == asize_)
			{
				insertNode = insertNode->next;
				localIndexOfInsertNode = 0;
			}


			localIndexOfNextValue++;
			if (localIndexOfNextValue == nextValueNode->count)
			{
				nextValueNode->count = 0;
				nextValueNode = nextValueNode->next;
				localIndexOfNextValue = 0;
			}

		}
		else
		{
			if (localIndexOfInsertNode == 0)
			{
				deadCount = insertNode->count;
				insertNode->count = insertNode->count - localIndexOfNextValue;
			}
			insertNode->values[localIndexOfInsertNode] = nextValueNode->values[localIndexOfNextValue];
			
			//check if need to jump to next node
			localIndexOfInsertNode++;
			if (localIndexOfInsertNode == asize_)
			{
				insertNode = insertNode->next;
				localIndexOfInsertNode = 0;
			}

			localIndexOfNextValue++;
			if (localIndexOfNextValue == deadCount)
			{
				nextValueNode = nextValueNode->next;
				localIndexOfNextValue = 0;
				deadCount = 0;
			}
		}

	}
	
	LNode* deleteNode = head_;
	while (deleteNode != nullptr && deleteNode->count > 0)
	{
		deleteNode = deleteNode ->next;
	}
	if (deleteNode == nullptr)
	{
		return;
	}
	if (deleteNode->prev != nullptr)//the node is not head
	{
		deleteNode->prev->next = nullptr;
		tail_ = deleteNode->prev;

		while (deleteNode != nullptr)
		{
			LNode* deleteNodet = deleteNode;
			deleteNode = deleteNodet->next;
			delete deleteNodet;
		}

	}
	else
	{
		delete head_;
		head_ = nullptr;
		tail_ = nullptr;
	}
	
	
}

template<typename T, int Size>
bool Lariat<T, Size>::Check()
{
	if (head_ == nullptr )
	{
		return true;
	}
	if (head_ -> prev != nullptr)
	{
		return false;
	}

	if (tail_->next != nullptr)
	{
		return false;
	}

	if (head_ == tail_ )
	{
		return true;
	}
	int cont = 0;
	LNode* h = head_;
	while (h != nullptr)
	{
		LNode* next = h->next;
		if (next != nullptr)
		{
			if (next->prev != h)
			{
				return false;
			}
		
		}
		h = next;
	}
	return true;
}

template<typename T, int Size>
T& Lariat<T, Size>::first()
{
	if (head_ == nullptr)
	{
		throw LariatException(LariatException::E_DATA_ERROR, "Subscript is out of range");
	}
	return head_->values[0];
}

template<typename T, int Size>
T const& Lariat<T, Size>::first() const
{

	if (head_ == nullptr)
	{
		throw LariatException(LariatException::E_DATA_ERROR, "Subscript is out of range");
	}
	return head_->values[0];
}

template<typename T, int Size>
T& Lariat<T, Size>::last()
{
	if (head_ == nullptr)
	{
		throw LariatException(LariatException::E_DATA_ERROR, "Subscript is out of range");
	}
	return tail_->values[tail_->count - 1];
}

template<typename T, int Size>
T const& Lariat<T, Size>::last() const
{
	if (head_ == nullptr)
	{
		throw LariatException(LariatException::E_DATA_ERROR, "Subscript is out of range");
	}
	return tail_->values[tail_->count - 1];
}

template<typename T, int Size>
unsigned Lariat<T, Size>::find(const T& value) const
{
	LNode* node = head_;
	int index = 0;
	while (node != nullptr)
	{
		for (int i = 0; i < node->count; i++)
		{
			if (node->values[i] == value)
			{
				return index;
			}
			index++;
			if (index >= size_)
			{
				return size_;
			}
		}
		node = node->next;
	}
	return index;
}



template<typename T, int Size>
typename Lariat<T, Size>::LNode* Lariat<T, Size>::findNodeAndLoacalIndex(int index, int& localIndex)
{

	if (index >= size_ || index < 0)
	{
		throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
	}
	int temp = 0;
	LNode* node = head_;
	while (temp <= index)
	{
		temp += node->count;
		if (temp > index)
		{
			localIndex = node->count - (temp - index);
			return node;
		}
		node = node->next;
	}
	if (node == nullptr)
	{
		node = tail_;
	}

	localIndex = node->count - (temp - index);
	return node;

}

template<typename T, int Size>
void Lariat<T, Size>::ShiftUp(LNode* node, int localIndex)
{
	if (head_ == nullptr)
	{
		return;
	}
	if (node->count < asize_)//not full
	{
		for (int i = node->count; i > localIndex; i--)
		{
			node->values[i] = node->values[i - 1];
		}		
		
	}
	else
	{
		throw LariatException(LariatException::E_NO_MEMORY,"No memory");
	}
	
}

template<typename T, int Size>
void Lariat<T, Size>::ShiftDown(LNode* node, int localIndex)
{
	if (node == nullptr)
	{
		return;
	}

	for (int i = localIndex; i < node->count - 1; ++i)
	{
		node->values[i] = node->values[i + 1];
	}
}

template<typename T, int Size>
void Lariat<T, Size>::Split(LNode* node, const T& value, int localIndex)
{
	size_++;
	if (asize_ == 1)
	{
		LNode* newNode = new LNode();
		newNode->prev = node;
		if (node->next != nullptr)
		{
			newNode->next = node->next;
		}
		node->next = newNode;
		newNode->count = 1;		
		node->values[0] = value;
		return;
	}
	//assert(node->count == asize_);
	int mid = (asize_ + 1) / 2;
	LNode* newNode = new LNode();
	newNode->prev = node;
	if (node->next != nullptr)
	{
		node->next->prev = newNode;
		newNode->next = node->next;

	}
	node->next = newNode;
	if (localIndex > mid)//value should be inserted in next node
	{
		int needCopyToNext = mid - 1;//this value should copy to next
		int remain = asize_ + 1 - mid;//total asize + 1
		localIndex = localIndex - remain;
		for (int i = 0; i < needCopyToNext; i++)
		{
			newNode->values[i] = node->values[asize_ - mid + 1 + i];
		}
		newNode->count = mid-1;
		ShiftUp(newNode, localIndex);
		newNode->values[localIndex] = value;
		newNode->count = mid;
		node->count = asize_ - mid + 1;
	}
	else
	{
		for (int i = 0; i < mid; i++)
		{
			newNode->values[i] = node->values[asize_ - mid + i];
		}
		newNode->count = mid;
		node->count = asize_ - mid;
		ShiftUp(node, localIndex);
		node->values[localIndex] = value;
		node->count++;
	}


	
}

template<typename T, int Size>
void Lariat<T, Size>::CreateHead()
{
	if (head_ == nullptr)
	{
		LNode* newNode = new LNode();
		tail_ = newNode;
		head_ = newNode;
	}
	else
	{
		clear();
		LNode* newNode = new LNode();
		tail_ = newNode;
		head_ = newNode;
	}

	return;
}
#include "lariat.cpp"

#endif // LARIAT_H
