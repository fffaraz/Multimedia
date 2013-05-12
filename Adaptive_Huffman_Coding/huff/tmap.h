#ifndef _MAP_H
#define _MAP_H

#include <utility>  // from STL
using namespace std;

#include "iterator.h"


// author: Owen Astrachan
// first written: 11/9/95, for CPS 100
// revisted     : 03/01/00

// abstract base class for a Map class
// (sometimes called Table or Dictionary or Associative Array class)
//
// The class Map supports a mapping of Keys -> Values
// for example: string -> int
//
// Abstract functions
//
// applyAll(Applicant & obj)     -- apply a class inherited from Applicant
//                             to ALL map entries, for Applicant see below
//
//    use: map.applyAll(instance); -- instance inherits from Applicant
//
// bool contains(key)        -- returns true if key stored in map
//
//    use: if (map.contains("apple")) ...
//
// Value & getValue(key)        -- returns reference to Value associated
//                                 with key, error if key not in table
//                                 (const version exists too)
//
//    use: value = map.getValue("apple")
//
//
// insert(Key,Value)           -- add new key,value pair
// insert(pair)                -- add new pair (with key) to map
//                             if key in map this is a no-op
//
//    use: map.insert(key,value);
//    use: map.insert(p); 
//
// makeIterator()           -- returns pointer to a usable iterator
//                             the iterator returns (Key,Value) pairs
//

template <class Key, class Value>
class Applicant
{
  public:
    virtual ~Applicant() {};
    virtual void applyOne(Key & key, Value & value) = 0;
};


template <class Key, class Value>
class tmap
{
  public:
    virtual ~tmap() {};
    virtual void applyAll(Applicant<Key,Value> & obj) = 0;  // apply obj
    virtual bool contains(const Key & key) const = 0;    // is key in map?

    // get values
    virtual const Value& get(const Key & key) const = 0;  // get value
    virtual Value& get(const Key & key) = 0;        

    virtual void clear() = 0;                             // remove all
    virtual void remove(const Key & key) = 0;             // remove one

    virtual void insert(const Key & key, const Value & value) = 0;
    virtual void insert(const pair<Key,Value> & pair) = 0;


    virtual int size() const = 0;
    
    // iterate over key/value pairs
    
    virtual Iterator<pair<Key,Value> > *   makeIterator() = 0;

};


#endif // _MAP_H undefined
