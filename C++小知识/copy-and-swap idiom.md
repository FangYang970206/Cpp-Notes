This answer is from https://stackoverflow.com/a/3279550/10133369

## Overview

### Why do we need the copy-and-swap idiom?

Any class that manages a resource (a *wrapper*, like a smart pointer) needs to implement [The Big Three](https://stackoverflow.com/questions/4172722/what-is-the-rule-of-three). While the goals and implementation of the copy-constructor and destructor are straightforward, the copy-assignment operator is arguably the most nuanced and difficult. How should it be done? What pitfalls need to be avoided?

The *copy-and-swap idiom* is the solution, and elegantly assists the assignment operator in achieving two things: avoiding [code duplication](http://en.wikipedia.org/wiki/Don't_repeat_yourself), and providing a [strong exception guarantee](http://en.wikipedia.org/wiki/Exception_guarantees).

### How does it work?

[Conceptually](https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom/3279616#3279616), it works by using the copy-constructor's functionality to create a local copy of the data, then takes the copied data with a `swap` function, swapping the old data with the new data. The temporary copy then destructs, taking the old data with it. We are left with a copy of the new data.

In order to use the copy-and-swap idiom, we need three things: a working copy-constructor, a working destructor (both are the basis of any wrapper, so should be complete anyway), and a `swap` function.

A swap function is a *non-throwing* function that swaps two objects of a class, member for member. We might be tempted to use `std::swap` instead of providing our own, but this would be impossible; `std::swap` uses the copy-constructor and copy-assignment operator within its implementation, and we'd ultimately be trying to define the assignment operator in terms of itself!

(Not only that, but unqualified calls to `swap` will use our custom swap operator, skipping over the unnecessary construction and destruction of our class that `std::swap` would entail.)

------

## An in-depth explanation

### The goal

Let's consider a concrete case. We want to manage, in an otherwise useless class, a dynamic array. We start with a working constructor, copy-constructor, and destructor:

```cpp
#include <algorithm> // std::copy
#include <cstddef> // std::size_t

class dumb_array
{
public:
    // (default) constructor
    dumb_array(std::size_t size = 0)
        : mSize(size),
          mArray(mSize ? new int[mSize]() : nullptr)
    {
    }

    // copy-constructor
    dumb_array(const dumb_array& other)
        : mSize(other.mSize),
          mArray(mSize ? new int[mSize] : nullptr),
    {
        // note that this is non-throwing, because of the data
        // types being used; more attention to detail with regards
        // to exceptions must be given in a more general case, however
        std::copy(other.mArray, other.mArray + mSize, mArray);
    }

    // destructor
    ~dumb_array()
    {
        delete [] mArray;
    }

private:
    std::size_t mSize;
    int* mArray;
};
```

This class almost manages the array successfully, but it needs `operator=` to work correctly.

### A failed solution

Here's how a naive implementation might look:

```cpp
// the hard part
dumb_array& operator=(const dumb_array& other)
{
    if (this != &other) // (1)
    {
        // get rid of the old data...
        delete [] mArray; // (2)
        mArray = nullptr; // (2) *(see footnote for rationale)

        // ...and put in the new
        mSize = other.mSize; // (3)
        mArray = mSize ? new int[mSize] : nullptr; // (3)
        std::copy(other.mArray, other.mArray + mSize, mArray); // (3)
    }

    return *this;
}
```

And we say we're finished; this now manages an array, without leaks. However, it suffers from three problems, marked sequentially in the code as `(n)`.

1. The first is the self-assignment test. This check serves two purposes: it's an easy way to prevent us from running needless code on self-assignment, and it protects us from subtle bugs (such as deleting the array only to try and copy it). But in all other cases it merely serves to slow the program down, and act as noise in the code; self-assignment rarely occurs, so most of the time this check is a waste. It would be better if the operator could work properly without it.

2. The second is that it only provides a basic exception guarantee. If `new int[mSize]` fails, `*this` will have been modified. (Namely, the size is wrong and the data is gone!) For a strong exception guarantee, it would need to be something akin to:

   ```cpp
   dumb_array& operator=(const dumb_array& other)
   {
       if (this != &other) // (1)
       {
           // get the new data ready before we replace the old
           std::size_t newSize = other.mSize;
           int* newArray = newSize ? new int[newSize]() : nullptr; // (3)
           std::copy(other.mArray, other.mArray + newSize, newArray); // (3)
   
           // replace the old data (all are non-throwing)
           delete [] mArray;
           mSize = newSize;
           mArray = newArray;
       }
   
       return *this;
   }
   ```

3. The code has expanded! Which leads us to the third problem: code duplication. Our assignment operator effectively duplicates all the code we've already written elsewhere, and that's a terrible thing.

In our case, the core of it is only two lines (the allocation and the copy), but with more complex resources this code bloat can be quite a hassle. We should strive to never repeat ourselves.

(One might wonder: if this much code is needed to manage one resource correctly, what if my class manages more than one? While this may seem to be a valid concern, and indeed it requires non-trivial `try`/`catch` clauses, this is a non-issue. That's because a class should manage [*one resource only*](http://en.wikipedia.org/wiki/Single_responsibility_principle)!)

### A successful solution

As mentioned, the copy-and-swap idiom will fix all these issues. But right now, we have all the requirements except one: a `swap` function. While The Rule of Three successfully entails the existence of our copy-constructor, assignment operator, and destructor, it should really be called "The Big Three and A Half": any time your class manages a resource it also makes sense to provide a `swap` function.

We need to add swap functionality to our class, and we do that as follows†:

```cpp
class dumb_array
{
public:
    // ...

    friend void swap(dumb_array& first, dumb_array& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.mSize, second.mSize);
        swap(first.mArray, second.mArray);
    }

    // ...
};
```

([Here](https://stackoverflow.com/questions/5695548/public-friend-swap-member-function) is the explanation why `public friend swap`.) Now not only can we swap our `dumb_array`'s, but swaps in general can be more efficient; it merely swaps pointers and sizes, rather than allocating and copying entire arrays. Aside from this bonus in functionality and efficiency, we are now ready to implement the copy-and-swap idiom.

Without further ado, our assignment operator is:

```cpp
dumb_array& operator=(dumb_array other) // (1)
{
    swap(*this, other); // (2)

    return *this;
}
```

And that's it! With one fell swoop, all three problems are elegantly tackled at once.

### Why does it work?

We first notice an important choice: the parameter argument is taken *by-value*. While one could just as easily do the following (and indeed, many naive implementations of the idiom do):

```cpp
dumb_array& operator=(const dumb_array& other)
{
    dumb_array temp(other);
    swap(*this, temp);

    return *this;
}
```

We lose an [important optimization opportunity](https://web.archive.org/web/20140113221447/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/). Not only that, but this choice is critical in C++11, which is discussed later. (On a general note, a remarkably useful guideline is as follows: if you're going to make a copy of something in a function, let the compiler do it in the parameter list.‡)

Either way, this method of obtaining our resource is the key to eliminating code duplication: we get to use the code from the copy-constructor to make the copy, and never need to repeat any bit of it. Now that the copy is made, we are ready to swap.

Observe that upon entering the function that all the new data is already allocated, copied, and ready to be used. This is what gives us a strong exception guarantee for free: we won't even enter the function if construction of the copy fails, and it's therefore not possible to alter the state of `*this`. (What we did manually before for a strong exception guarantee, the compiler is doing for us now; how kind.)

At this point we are home-free, because `swap` is non-throwing. We swap our current data with the copied data, safely altering our state, and the old data gets put into the temporary. The old data is then released when the function returns. (Where upon the parameter's scope ends and its destructor is called.)

Because the idiom repeats no code, we cannot introduce bugs within the operator. Note that this means we are rid of the need for a self-assignment check, allowing a single uniform implementation of `operator=`. (Additionally, we no longer have a performance penalty on non-self-assignments.)

And that is the copy-and-swap idiom.

## What about C++11?

The next version of C++, C++11, makes one very important change to how we manage resources: the Rule of Three is now **The Rule of Four** (and a half). Why? Because not only do we need to be able to copy-construct our resource, [we need to move-construct it as well](https://stackoverflow.com/questions/3106110/can-someone-please-explain-move-semantics-to-me).

Luckily for us, this is easy:

```cpp
class dumb_array
{
public:
    // ...

    // move constructor
    dumb_array(dumb_array&& other)
        : dumb_array() // initialize via default constructor, C++11 only
    {
        swap(*this, other);
    }

    // ...
};
```

What's going on here? Recall the goal of move-construction: to take the resources from another instance of the class, leaving it in a state guaranteed to be assignable and destructible.

So what we've done is simple: initialize via the default constructor (a C++11 feature), then swap with `other`; we know a default constructed instance of our class can safely be assigned and destructed, so we know `other` will be able to do the same, after swapping.

(Note that some compilers do not support constructor delegation; in this case, we have to manually default construct the class. This is an unfortunate but luckily trivial task.)

### Why does that work?

That is the only change we need to make to our class, so why does it work? Remember the ever-important decision we made to make the parameter a value and not a reference:

```cpp
dumb_array& operator=(dumb_array other); // (1)
```

Now, if `other` is being initialized with an rvalue, *it will be move-constructed*. Perfect. In the same way C++03 let us re-use our copy-constructor functionality by taking the argument by-value, C++11 will *automatically* pick the move-constructor when appropriate as well. (And, of course, as mentioned in previously linked article, the copying/moving of the value may simply be elided altogether.)

And so concludes the copy-and-swap idiom.

------

### Footnotes

*Why do we set `mArray` to null? Because if any further code in the operator throws, the destructor of `dumb_array` might be called; and if that happens without setting it to null, we attempt to delete memory that's already been deleted! We avoid this by setting it to null, as deleting null is a no-operation.

†There are other claims that we should specialize `std::swap` for our type, provide an in-class `swap` along-side a free-function `swap`, etc. But this is all unnecessary: any proper use of `swap` will be through an unqualified call, and our function will be found through [ADL](http://en.wikipedia.org/wiki/Argument-dependent_name_lookup). One function will do.

‡The reason is simple: once you have the resource to yourself, you may swap and/or move it (C++11) anywhere it needs to be. And by making the copy in the parameter list, you maximize optimization.