#include <iostream>
#include <vector>
#include <iterator>


class ctx_dmin_heap {
public:
    ctx_dmin_heap();
    ~ctx_dmin_heap();
    void insert(int element);
    int deletemin();
    void print();
    int size() { return heap.size(); }
private:
    int left(int parent);
    int right(int parent);
    int parent(int child);
    void heapifyup(int index);
    void heapifydown(int index);
private:
    std::vector<int> heap;
};