/*
~~ Quake-Heap ~~
	github.com/138paulmiller
	Header-only implementation of Timothy Chan's Quake Heap and is a proposed alternative to the preexisting Fibonacii Heap. 
	See http://tmc.web.engr.illinois.edu/heap_ianfest.pdf
	Code was created for benchmark tests and makes use of modern C++ features.

*/
#include <utility>
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <iterator>
#include <cmath>
#include <iostream>
/*
	Implements the major three operation insert(x), deleteMin(), decreaseKey(x,k) 
*/

//Make sure that key has define overloaded the '<' operator
template <typename Key, typename Value>
class QuakeHeap {
private:
	//forward declare
	struct TEntry;
	struct TNode; 
	
	struct TEntry{
		TEntry(){}
		TEntry(Key k, Value v);
		Key key(){return _key;}
		Key value(){return _value;}
	private:
		friend class QuakeHeap;
		Key  _key;
		Value  _value;
		std::shared_ptr<TNode> _node; //reference to highest node 
	};

	//type aliases
	using RootIterator = typename std::list< std::shared_ptr<TNode> >::iterator;
	// Tournament tree node, binary node with a pointer to the min entry
	struct TNode{
		TNode(std::shared_ptr<TEntry> e=nullptr);//create new entry leaf, unique param since e should have been created before
		TNode(std::shared_ptr<TNode> l, std::shared_ptr<TNode> r); //linking two nodes
		std::shared_ptr<TNode> _l, _r, _p; 		//children
		std::shared_ptr<TEntry> _entry; //minimum entry of _l and _r 
		//to allow for O(1) cuts, each node, if it is a root contains a reference to the iterator that contains it
		RootIterator _container;
		unsigned int _height;
	};

	struct TException{
		TException(std::string msg):_msg(msg){}
		inline std::string what(){return _msg;}
		std::string _msg;
	};
	//utility
	inline void updateMin();
	void shake(std::shared_ptr<TNode> root);
	//ATTRIBUTES
	//quake heap attr
	RootIterator _minimum; //reference to Node with minimum entry 
	std::list< std::shared_ptr<TNode> > _forest;
	unsigned long _size;
	float _alpha; //alpha ratio that bounds tree heights

public:
	using Entry = typename std::shared_ptr<TEntry>;
	using Exception = TException;

	
	QuakeHeap(float alpha=1);
	virtual ~QuakeHeap();
	//primary operations
	void insert(std::shared_ptr<TEntry>& entry);
	void decreaseKey(std::shared_ptr<TEntry>& entry, Key newKey);
	std::shared_ptr<TEntry> getMin();
	std::shared_ptr<TEntry> deleteMin();
	bool empty();
	unsigned long size();
	//accessors
	static std::shared_ptr<TEntry> makeEntry(Key key, Value value);

};


//Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TEntry::TEntry(Key k, Value v):
	_key(k),
	_value(v),
	_node(nullptr){}



// ----------------------------------- TNode Definitions ---------------------------------------

template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(std::shared_ptr<TEntry> e):
	_l(nullptr),
	_r(nullptr),
	_p(nullptr),
	_height(0),
	_entry(nullptr){
		//move unique to nodes entry
		_entry = std::shared_ptr<TEntry>(e);
}


template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(std::shared_ptr<TNode> l, std::shared_ptr<TNode> r):
	_l(l),
	_r(r),
	_p(nullptr),
	_entry(nullptr){
		//pick min if both are nonnull
		if(l != nullptr && r != nullptr ){
			_entry = (l->_entry->_key < r->_entry->_key )? l->_entry : r->_entry;
		}else if(l != nullptr){
			_entry =  l->_entry;
		}else if(r != nullptr){
			_entry =  r->_entry;
		}else{
			throw TException("Tournament Node: Cannot Link two empty Nodes!");
		}
		//update entries height node	
	_height = std::max(l->_height, r->_height)+1;
}


// -------------------------------- Quake Heap Definitions -------------------------------------



template <typename Key, typename Value>
QuakeHeap<Key, Value>::QuakeHeap(float alpha):
	_size(0),
	_alpha(alpha){
		_minimum = _forest.end();
}
template <typename Key, typename Value>
QuakeHeap<Key, Value>::~QuakeHeap(){

}

//utlitities
template <typename Key, typename Value>
std::shared_ptr<typename QuakeHeap<Key, Value>::TEntry> QuakeHeap<Key, Value>::makeEntry(Key key, Value value){
	return std::make_shared<TEntry>(key, value);
}




template <typename Key, typename Value>
void QuakeHeap<Key, Value>::updateMin(){
	if( _minimum == _forest.end()){
		--_minimum;
	}else if (_forest.back()->_entry->_key < (*_minimum)->_entry->_key){
		_minimum = _forest.end(); //move to last element
		--_minimum;
	}
}

template <typename Key, typename Value>
void QuakeHeap<Key, Value>::shake(std::shared_ptr<TNode> root){
	std::shared_ptr<TEntry> entry = root->_entry; 
	while(root != nullptr ){
		 //if left contains entry
		if(root->_l && root->_l->_entry == entry){
			//add right if nonnull
			if(root->_r){
				root->_r->_p = nullptr;
				_forest.push_back(root->_r);
			}
			root =root->_l;
		}else if(root->_r && root->_r->_entry == entry){
			//add right if nonnull
			if(root->_l){
				root->_l->_p = nullptr;
				_forest.push_back(root->_l);	
			}
			root = root->_r;
		}else
			break; //should never occur due to tournament structure
	}
}
//primary operations

template <typename Key, typename Value>
bool QuakeHeap<Key, Value>::empty(){
	return _size == 0; //
}
template <typename Key, typename Value>
unsigned long QuakeHeap<Key, Value>::size(){
	return _size; //
}

/*
insert(x):
	create a new tree containing {x}
*/
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::insert(std::shared_ptr<TEntry>& entry){
	std::shared_ptr<TNode> node = std::make_shared<TNode>(entry);
	entry->_node = node;
	_forest.push_back(node);
	//update min
	updateMin();
	(*_forest.rbegin())->_container = _forest.end(); //move to last element
	--((*_forest.rbegin())->_container);
	++_size;
}

/*
decrease-key(x, k):
	cut the subtree rooted at the highest node storing x [yields 1 new tree]
	change x’s value to k
*/
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::decreaseKey(std::shared_ptr<TEntry>& entry, Key newKey){
	/*
	       y
		x
	a 		b  
	cut x from parent and reinsert into forest
	*/
	if(entry->_key < newKey)
		throw TException("Decrease Key: New key " + std::to_string(newKey)+ \
						" must be less than old key " + std::to_string(entry->_key)); //throw exception is fail
	entry->_key = newKey;
	std::shared_ptr<TNode> x = entry->_node;
	//cut x from tree
	if(x->_p == nullptr){
		_forest.erase(x->_container);
	}
	else if(x->_p->_l == x){
		x->_p->_l = nullptr;
	}
	else if(x->_p->_r == x){
		x->_p->_r = nullptr;
	}
	_forest.push_back(x);
	updateMin();

}
template <typename Key, typename Value>
std::shared_ptr<typename QuakeHeap<Key, Value>::TEntry> QuakeHeap<Key, Value>::getMin(){
	return (*_minimum)->_entry;
}
/*
delete-min():
	x = minimum of all the roots
	remove the path of nodes storing x [yields multiple new trees]
	while there are 2 trees of the same height:
		link the 2 trees [reduces the number of trees by 1]
	if n(i+1) > αn(i) for some i then:
	 	let i be the smallest such index
		remove all nodes at heights > i[increases the number of trees]
	return x
*/
template <typename Key, typename Value>
std::shared_ptr<typename QuakeHeap<Key, Value>::TEntry> QuakeHeap<Key, Value>::deleteMin(){
	//cut all nodes from min to leaf, insert nonmin children to tree
	RootIterator old_minimum = _minimum;
	std::shared_ptr<TEntry> min_entry = (*_minimum)->_entry; 
	shake(*_minimum);
	_forest.erase(old_minimum);
	//find min and bucket all trees of equal height
	std::vector<std::list< std::shared_ptr<TNode> > > buckets(1+std::ceil(log2(_size)));
	//counts number of heights at heights=index
	std::vector<int > heights(1+std::ceil(log2(_size)));
	while( _forest.size() > 0){
		std::shared_ptr<TNode> node = _forest.front();
		_forest.pop_front();
		buckets[node->_height].push_back(node);

	}
	// (link at roots of same size)
	//until bucket is empty at each, propagate linked upwards
	for(int i = 0; i < buckets.size(); ++i){

		std::list< std::shared_ptr<TNode> > * node_list = &buckets[i];
		if(node_list->size() > 1){
			heights[i] = 1;
		}
		while(node_list->size() > 1){
			//get two nodes, then link and add back
			std::shared_ptr<TNode> a = node_list->back();
			node_list->pop_back();
			std::shared_ptr<TNode> b = node_list->back();
			node_list->pop_back();
			std::shared_ptr<TNode> linked = std::make_shared<TNode>(a,b); 
			a->_p = linked;
			b->_p = linked;
			buckets[i+1].push_back(linked );
		}
	}
	//remove all nodes into root list at height > i for smallest i such that n(i+1) > alpha*ni 
	int min_i = -1;
	for(int i = 1; i < heights.size() && min_i != -1; ++i){
		if(heights[i] > heights[i-1]*_alpha){
			min_i = i;
		}
	}
	// //todo remove all nodes at height i, add children to rootlist
	for(int i = 0; i < buckets.size(); ++i){
		if(buckets[i].size() > 0){
			if ( min_i != -1 && i > min_i){
				shake(buckets[i].front());
			}else{

				_forest.push_back(buckets[i].front());
			}
		}
	}
	_minimum = _forest.begin();
	for(RootIterator it = ++_forest.begin(); it != _forest.end(); ++it){
		if(  (*it)->_entry->_key < (*_minimum)->_entry->_key)
			_minimum = it;
	}
	--_size;
	return min_entry;
}