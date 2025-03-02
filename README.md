# Custom-Shared_Pointer

## Approach
This is a crack at implementing a custom version of the std::shared_ptr class. The basic desing of the class is fairly straightforward, ensuring we have proper reference counts was paramount and deletion when the SharedPtr ran out of references. 

One notable case I had to take care of implementing was the cases with nullptr. With the initialization of SharedPtr, any SharedPtr that pointed directly to nullptrs or indirectly pointed to nullptrs were explicitly given a count of 0 and will not get deleted, in hopes of keeping in line with std::shared_ptr. This leads to more memory usage, but kept in line with the std::shared_ptr approach. 

In considering thread safety, I made the count atomic and locked down all assignments, operators, functions, and deletors with mutexes, I am sure there is a better way to handle it but it worked for the cases I was testing with. Testing was done by comparing my SharedPtr behavior to the std::shared_ptr behavior with identical test cases and asserts, to ensure consistency in expected behavior.

For cleanup, I used a custom private built function that decrements while it checks for the last reference to an object, this would avoid deleting the SharedPtr with nullptrs.

## Trade-Offs
As mentioned before, special care was taken into not deleting SharedPtrs that pointed to nullptr, this would lead to more memory as SharedPtrs explicitly set to nullptr will not be deleted, but kept in line with the std::shared_ptr behavior so it is a tradeoff I am willing to make.

In regards to thread safety, the atomic count and mutexes guarding all instance of modification and access in the class will make these pointers slower but makes it much more resilient to thread based programming (but you should prefer to use task based programming anyways)