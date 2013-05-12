#include <iostream>
using namespace std;

static int DEFAULT = 11;   // initial size, file scope variable

template <class Kind>
Comparer<Kind> tpqueue<Kind>::ourComp = Comparer<Kind>();

template <class Kind>
tpqueue<Kind>::tpqueue()
 : myComp(ourComp),
   myNumElts(0)
{
    init(DEFAULT);
}

template <class Kind>
tpqueue<Kind>::tpqueue(const Comparer<Kind>& cmp)
 : myComp(cmp),
   myNumElts(0)
// postcondition: all fields initialized, priority queue has room
//                for ten elements     
{
    init(DEFAULT);
}

template <class Kind>
void tpqueue<Kind>::init(int n)
// postcondition:  list reserves space for n elements
//                 myList[0] contains garbage heap starts at myList[1]
{
    myList.reserve(n);
    myList.push_back(Kind());

}


template <class Kind>
tpqueue<Kind>::~tpqueue()
// postcondition: priority queue is "garbage"     
{
}


template <class Kind>
int tpqueue<Kind>::size() const
// postcondition: returns number of items queued up
{
    return myNumElts;
}
    
template <class Kind>
bool tpqueue<Kind>::isEmpty() const
// postcondition: returns true if pqueue is empty, else false
{
    return myNumElts == 0;
}


template <class Kind>
void tpqueue<Kind>::insert(const Kind & elt)
{
    myList.push_back(elt);  // increase size of heap
    myNumElts++;            // add at end (heap shape)
    int k = myNumElts;      // location of new element
    
    while (k > 1 && myComp.compare(myList[k/2],elt) > 0)
    {
	myList[k] = myList[k/2];
	k /= 2;
    }
    myList[k] = elt;
}

template <class Kind>
void tpqueue<Kind>::deletemin()
// precondition: ! isEmpty()     
// postcondition: remove minimal element from priority queue     
{
    if (! isEmpty())
    {
	myList[1] = myList[myNumElts];   // move last to top
	myList.pop_back();               // reduce size of heap
	myNumElts--;
	if (myNumElts > 1)
	    heapify(1);                      // and push down
    }
}

template <class Kind>
void tpqueue<Kind>::deletemin(Kind & ref)
// precondition: ! isEmpty()     
// postcondition: remove minimal element from priority queue
//                set ref/prio to minimum element in heap          
{
    if (! isEmpty())
    {
	ref = getmin();
	deletemin();
    }
}

template <class Kind>
void tpqueue<Kind>::dump() const
{
    cout << size() << "\t";
    for(int k=1; k <= size(); k++)
    {
	cout << myList[k] << " ";
    }
    cout << endl;
}

template <class Kind>
Kind& tpqueue<Kind>::getmin()
// postcondition: set ref/prio to minimum element in heap     
{
    return myList[1];
}


template <class Kind>
void tpqueue<Kind>::heapify(int vroot)
     
// preconditon: subheaps of vroot satisfy heap property (and shape)
// postcondition: heap rooted at vroot satisfies heap property
     
{
    Kind last = myList[vroot];
    int child, k = vroot;

    while (2*k <= myNumElts)
    {
	// find minimal child (assume left, then check right)
	
	child = 2*k;  
	if (child < myNumElts &&
	    myComp.compare(myList[child], myList[child+1]) > 0)
	{
	    child++;
	}
	if (myComp.compare(last, myList[child]) <=0)  // it goes here
	{
	    break;
	}
	else
	{
	    myList[k] = myList[child];
	    k = child;
	}
    }
    // found "resting place", insert 'last element'
    
    myList[k] = last;
}
