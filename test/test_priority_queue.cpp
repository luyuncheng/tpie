//#define TP_LOG_APPS

#include "app_config.h"
#include <portability.h>
#include <priority_queue.h>
#include <iostream>
#include <queue>
#include <cstdlib>

using namespace std;

const double PI = acos(-1.0);

void pq_large_instance(){
  MM_manager.set_memory_limit(64*1024*1024);
  int cnt=0;
  tpie::priority_queue<int, std::greater<int> > pq(0.75);
  std::priority_queue<int, vector<int>,std::less<int> > pq2;
  for(int i=0;;i++){
    double th = (cos(i*2.0*PI/500000.0)+1.0)*(RAND_MAX/2);
    if(!pq.empty()){
      if(pq.top()!=pq2.top()){
	std::cerr << "Priority queues differ, got " << pq.top() << " but expected " 
	     << pq2.top() << "\n";
	assert(0);
      }
    }else{
      assert(cnt==0);
    }
    if(rand()<th){
      //std::cout << "Insert\n";
      cnt++;
      int r = rand();
      pq.push(r);
      pq2.push(r);
    }else{
      //std::cout << "Delete\n";
      if(pq.empty())
	continue;
      cnt--;
      pq.pop();
      pq2.pop();
    }
    std::cout << "Size: " << cnt << "\r";
  }
}

void pq_internal_instance(){
  int size=100000;
  tpie::pq_internal_heap<int, std::greater<int> > pq(size);
  std::priority_queue<int, vector<int>,std::less<int> > pq2;
  for(int i=0;i<size;i++){
    int r = rand();
    pq.insert(r);
    pq2.push(r);
  }
  while(!pq.empty()){
    if(pq.peekmin()!=pq2.top()){
      std::cerr << "Internal memory heap failed.\n";
      assert(0);
    }
    pq.delmin();
    pq2.pop();
  }
}

void pq_small_instance(){
  //MM_manager.set_memory_limit(10*1024*1024);
  //std::cout << "LOGGING: " << logstream::log_initialized << "\n";
  //tpie_log_init(TPIE_LOG_WARNING);
  //std::cout << "LOGGING: " << logstream::log_initialized << "\n";


  std::cout << "tpie::priority_queue Debug - M test" << std::endl;
    TPIE_OS_OFFSET iterations = 1000000;
    MM_manager.set_memory_limit(16*1024*1024);
    for(TPIE_OS_OFFSET it = 1100; it < iterations; it++)  {
      std::cerr << "Iteration: " << it;
      tpie::priority_queue<int, std::greater<int> > pq(0.75);
      std::priority_queue<int, vector<int>,std::less<int> > pq2;

      TPIE_OS_OFFSET elements = 71;
      TPIE_OS_SRANDOM(it);
      for(TPIE_OS_OFFSET i=0;i<elements;i++) {
        int src_int = TPIE_OS_RANDOM()%220;
        pq.push(src_int);
        pq2.push(src_int);
//        std::cout << "push " << src_int << std::endl;
      }
//std::cout << "all push done" << std::endl;
          pq.pop();
          pq2.pop();
          pq.pop();
          pq2.pop();
          pq.pop();
          pq2.pop();
//          pq.pop();
//          pq2.pop();
//pq2.dump("internal.dot");
      TPIE_OS_OFFSET pop = 61; 
      for(TPIE_OS_OFFSET i=0;i<pop;i++) {
        if(!pq.empty()) {
//          std::cout << "pop " << pq.top() << " " << pq2.top() << std::endl;
          if(pq.top() != pq2.top()) {
            std::cout << "main, run21 error1, " << i << " got: " << pq.top() << " expected " << pq2.top() << std::endl;
            //pq.dump();
            exit(-1);
          }
//          pq.dump();
          pq.pop();
          pq2.pop();
        }
      }
      for(TPIE_OS_OFFSET i=0;i<elements;i++) {
        int src_int = TPIE_OS_RANDOM()%220;
        pq.push(src_int);
        pq2.push(src_int);
//        std::cout << "push " << src_int << std::endl;
      }
//      std::cout << "Pop remaining" << std::endl;
//pq.dump();
      while(!pq.empty()) {
        if(pq.top() != pq2.top()) {
          std::cout << "main, run21 error2, got: " << pq.top() << " expected " << pq2.top() << std::endl;
          //pq.dump();
          exit(-1);
        }
	//        std::cout << "pop " << pq.top() << std::endl;
        pq.pop();
        pq2.pop();
      }
      std::cout << std::endl;
    }
  }


int main(int argc,char** argv){
  if(argc!=2){
    std::cout << "Arguments are test_priority_queue <type>\nWhere type is either \"small\" , \"large\" or \"internal\"\n";
  }
  if(strcmp(argv[1],"small")==0)
    pq_small_instance();
  else if(strcmp(argv[1],"large")==0)
    pq_large_instance();
  else if(strcmp(argv[1],"internal")==0)
    pq_internal_instance();
}