#include "BinaryHeap.h"

BinaryHeap::BinaryHeap()
{
    // TODO: or not
    size  = 0;
    elements.resize(1);
}


bool BinaryHeap::Add(int uniqueId, double weight)
{
    // TODO:
    if(finder(uniqueId) != -1) return false;
    struct HeapElement newItem;
    newItem.uniqueId = uniqueId;
    newItem.weight = weight;
    elements[0] = newItem;
    if(size + 1 == elements.size())
    {
        elements.resize(elements.size()*2 + 1);
    }

    int hole = ++size;
    for(; newItem.weight < elements[hole/2].weight; hole /= 2)
        elements[hole] = elements[hole/2];
    elements[hole] = newItem;
    return true;
}

bool BinaryHeap::PopHeap(int& outUniqueId, double& outWeight)
{
    // TODO:
    if(size == 0) return false;
    HeapElement temp = elements[1];
    elements[1] = elements[size--];
    bubbledown(1);
    outUniqueId = temp.uniqueId;
    outWeight = temp.weight;
    return true;
}

bool BinaryHeap::ChangePriority(int uniqueId, double newWeight)
{
    // TODO:
    int index = finder(uniqueId);
    if(index == -1) return false;
    elements[index].weight = newWeight;
    elements[0].weight = newWeight;
    elements[0].uniqueId = uniqueId;
    int hole = index;
    if(newWeight < elements[hole/2].weight)
    {
            for(; newWeight < elements[hole/2].weight; hole /= 2)
            elements[hole] = elements[hole/2];
        elements[hole].weight = newWeight;
        elements[hole].uniqueId = uniqueId;
    }
    else bubbledown(index);
    return true;
}

int BinaryHeap::HeapSize() const
{
    // TODO:
    return size;
}

void BinaryHeap::bubbledown(int hole)
{
    int child;
    HeapElement tmp = elements[hole];
    for(; hole*2 <= size; hole = child)
    {
        child = hole*2;
        if(child != size && elements[child + 1].weight < elements[child].weight)
            child++;
        if(elements[child].weight < tmp.weight)
            elements[hole] = elements[child];
        else 
            break;
    }
    elements[hole] = tmp;
}

int BinaryHeap::finder(int uniqueId)
{
    for(int i = 1; i< size+1; i++)
    {
        if(elements[i].uniqueId == uniqueId) return i;
    }
    return -1;
}