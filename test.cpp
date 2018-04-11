#include "quake_heap.hpp"
#include <unordered_map>
int main(){
	const unsigned int N = 100;
	using QH = QuakeHeap<int, double >;
	QH pq;
	using EntryIterator = std::unordered_map<int, QH::Entry>::iterator;
	std::unordered_map<int, QH::Entry> entries;
	entries.reserve(N); 
	try{
		int i =0;

		for(int x = 0; x < 100; x++){
			std::cout << "\ninserting " << N <<  " ...";
			std::cout.flush();
			for(unsigned long i = x; i< N; i++){
				entries[i] = QH::makeEntry((i+i)*x,i*0.09);
				pq.insert(entries[i]);
			}
			std::cout << "\ndecreasing " << N/2 <<  " ...";
			std::cout.flush();
			i =0;
			for(EntryIterator it = entries.begin(); it != entries.end() && i< N/2 ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}		

			std::cout << "\ndeleting-min " << N/2 <<  " ...";
			std::cout.flush();
			i=0;
			for(EntryIterator it = entries.begin(); it != entries.end() && i < N/2 ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}	
			std::cout << "\ndecreasing " << N/2 <<  " ...";
			std::cout.flush();
			i =0;
			for(EntryIterator it = entries.begin(); it != entries.end() && i< N/4 ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}		
			std::cout << "\ndeleting-min " << N/2 <<  " ...";
			std::cout.flush();
			i=0;
			for(EntryIterator it = entries.begin(); it != entries.end() && i < N/2 ; it++){
				pq.decreaseKey(it->second, it->second->key()/2);
				++i;
			}	
		

		}
		std::cout << "\ndeleting-min remaining " << pq.size() << "...";
		std::cout.flush();
		i = 0;
		while(!pq.empty()){
			auto min = pq.deleteMin();
			std::cout << min->key() << " ";
			if(i%10)//flugh every 199
				std::cout.flush();
			++i;
		}
	}catch(QH::Exception e){
		std::cout << e.what() << "\n";
	}
	std::cout << "\nGoodbye\n";
	return 0;
}