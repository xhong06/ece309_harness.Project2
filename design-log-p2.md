# Design Log — Project 2

(500–800 words total. See spec §5 for what each section must cover.)

## Growth factor and amortized cost

When the conversation is started, it is initially empty. Every time the conversation is reallocated, the size is doubled; however, if the size is 0, the size is just reallocated to 1. Reallocating an array of size K requires allocating a new contiguous buffer and 
copying K elements, costing O(K) work. This is the worst case.

## Rule of Five evidence

The destructor deallocates the underlying Message array via delete[] data_ and resets pointer and capacity state. For both the copy constructor and the copy assignment, deep copies are made of the initial conversation because memory for a new conversation object is allocated before the data is moved from the initial to the copied. Additionally, the data_, size_, and capacity_ of the initial conversation stays correct, meaning that the copy is truly a copy, not just a moved conversation. For both the move constructor and the move assignment functions, not only is the latter instance given the same data as the initial, but makes the initial's data_ = nullptr, size_ = 0 and capacity_ = 0, effectively nulling the initial in order to prevent double-deletion.

## Sentinel scanner: bounded pending_ proof

The SentinelScanner is designed to process arbitrary, potentially infinite streaming text while guaranteeing that its internal state pending_ never exceeds sentinel() - 1 characters. Since we know that the (inputted text) - (outputted text) = pending_, we need only count the different between the input and output and see if those characters exceed the size of sentinel() - 1. And from the tests, we've established that they do not.

## What I would change differently

If I could do something differently, I would make the harness more testable by making a class for Inputs and Outputs instead of making them virtual.
