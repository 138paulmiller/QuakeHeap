#include "quake_heap.hpp"
#include <unordered_map>
int main(){
	const unsigned int N = 1000;
	using QH = QuakeHeap<int, double >;
	QH pq;
	using EntryIterator = std::unordered_map<int, QH::Entry>::iterator;
	std::unordered_map<int, QH::Entry> entries;
	entries.reserve(N); 
	try{
		int i =0;
		int n = 0;

		for(int x = 1; x < 10; x++){
			n = N;
			std::cout << "\ninserting " << n <<  " ...";
			std::cout.flush();
			for(unsigned long i = x; i< N; i++){
				entries[i] = QH::makeEntry((i+i)*x,i*0.09);
				pq.insert(entries[i]);
			}
			n = N/x;
			i =0;
			std::cout << "\ndecreasing " << n <<  " ...";
			std::cout.flush();
			for(EntryIterator it = entries.begin(); it != entries.end() && i< n; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}		

			n = N/x;
			i=0;
			std::cout << "\ndeleting-min " << n <<  " ...";
			std::cout.flush();
			for(EntryIterator it = entries.begin(); it != entries.end() && i < n ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}	
			n = N/4*x;
			i =0;
			std::cout << "\ndecreasing " <<n<<  " ...";
			std::cout.flush();
			for(EntryIterator it = entries.begin(); it != entries.end() && i< n ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}		
			n = N/2*x;
			i=0;
			std::cout << "\ndeleting-min " << n <<  " ...";
			std::cout.flush();
			for(EntryIterator it = entries.begin(); it != entries.end() && i < n ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}	
		

		}
		std::cout << "\ndeleting-min remaining " << pq.size() << "...";
		std::cout.flush();
		i = 0;
		while(!pq.empty()){
			++i;
			auto min = pq.deleteMin();
			std::cout << min->key() << "\n";
			std::cout.flush();
			
		}
	}catch(QH::Exception e){
		std::cout << e.what() << "\n";
	}
	std::cout << "\nGoodbye\n";
	return 0;
}