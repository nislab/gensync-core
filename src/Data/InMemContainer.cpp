//
// Created by GregoryFan on 7/10/2025
//
#include <GenSync/Data/InMemContainer.h>

InMemContainer::~InMemContainer(){clear();}

DataContainer::iterator InMemContainer::begin(){
    return DataIteratorWrapper(std::unique_ptr<DataIterator>(new InMemIterator(myData.begin())));
}

DataContainer::iterator InMemContainer::end(){
    return DataIteratorWrapper(std::unique_ptr<DataIterator>(new InMemIterator(myData.end())));
}

DataContainer::const_iterator InMemContainer::begin() const{
    return DataIteratorWrapper(std::unique_ptr<DataIterator>(new ConstInMemIterator(myData.cbegin())));
}

DataContainer::const_iterator InMemContainer::end() const{
    return DataIteratorWrapper(std::unique_ptr<DataIterator>(new ConstInMemIterator(myData.cend())));
}

DataContainer::size_type InMemContainer::size() const{
    return myData.size();
}

bool InMemContainer::empty() const{
    return myData.empty();
}

void InMemContainer::clear(){
    myData.clear();
}

bool InMemContainer::remove (const shared_ptr<DataObject>& val){
    int before = myData.size();
    auto it = find(myData.begin(), myData.end(), val);
    if(it != myData.end()){
        myData.erase(it);
    }
    return myData.size() < before;
}

void InMemContainer::add(const shared_ptr<DataObject>& val){
    myData.push_back(val);
}