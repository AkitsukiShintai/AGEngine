#include "disjoint_sets.h"

//class Node implementation
Node::Node( size_t const& value ) : value(value), next() {}

Node* Node::Next() const { return next; }

void  Node::SetNext( Node* new_next ) { next = new_next; }

size_t Node::Value() const { return value; }

std::ostream& operator<< (std::ostream& os, Node const& node) {
	os << "(" << node.value << " ) -> ";
	return os;
}

//class Head implementation
Head::Head( ) : counter(), first(), last() {}


Head::~Head() {
	while (first)
	{
		Node* p = first;
		first = first->Next();
		delete p;
	}
}

size_t Head::Size() const      { return counter; }

void   Head::Reset()           { counter = 0; last = first = nullptr; }

Node*  Head::GetFirst() const  { return first; }

Node*  Head::GetLast()  const  { return last; }

void   Head::Init( size_t value ) {
	first = last = new Node( value );
	counter = 1;
}

void   Head::Join( Head* pHead2 ) {
	pHead2->last->SetNext(first);
	pHead2->last = last;
	pHead2->counter += counter;
	Reset();
}

std::ostream& operator<< (std::ostream& os, Head const& head) {
	os << "[" << head.counter << " ] -> ";
	return os;
}

DisjointSets::DisjointSets():size(0),
capacity(0),
representatives(new size_t[capacity]),
heads(new Head[capacity])
{
}

//class DisjointSets implementation
DisjointSets::DisjointSets( size_t const& capacity ) : 
	size(0),
	capacity(capacity),
	representatives(new size_t[capacity]),
	heads          (new Head[capacity])
{ }

DisjointSets::~DisjointSets() {
	delete [] heads;
	delete [] representatives;
}

void DisjointSets::Make( ) {
	if ( size == capacity ) throw "DisjointSets::Make(...) out of space";
	heads[size].Init( size );
	representatives[size] = size;
	++size;
}

void DisjointSets::Join( size_t const& id1, size_t const& id2 ) {
	size_t set1 = representatives[id1];
	size_t set2 = representatives[id2];
	if (set1 == set2)
	{
		return;
	}
	size_t changedReference = -1;
	size_t newRepresent = -1;
	//join head1 represent to head2 represent
	if (heads[set2].Size() >= heads[set1].Size())
	{
		heads[set1].Join(&heads[set2]);
		changedReference = representatives[id1];
		representatives[id1] = set2;
		newRepresent = set2;
	}
	else
	{

		heads[set2].Join(&heads[set1]);
		changedReference = representatives[id2];
		representatives[id2] = set1;
		newRepresent = set1;
	}

	for (size_t i = 0; i< size ; i++)
	{
		if (representatives[i] == changedReference)
		{
			representatives[i] = newRepresent;
		}
	}

}

size_t DisjointSets::GetRepresentative( size_t const& id ) const {
	return representatives[id];
}

size_t DisjointSets::operator[]( size_t const& id ) const {
	return representatives[id];
}


std::ostream& operator<< (std::ostream& os, DisjointSets const& ds) {
	for (size_t i=0; i<ds.size; ++i ) {
		os << i << ":  ";
		Head *p_head = &ds.heads[i];
		os << *p_head;
		Node* p_node = p_head->GetFirst();
		while ( p_node ) {
			os << *p_node;
			p_node = p_node->Next();
		}
		os << "NULL (representative " << ds.representatives[i] << ")\n";
	}
	return os;
}
