# Hire-me challenge - NERD

This is my attempt at the [HireMe.c](https://www.nerd.nintendo.com/files/HireMe) challenge proposed by [Nintendo European Research & Development](https://www.nerd.nintendo.com/) on their website. The challenge consists in reverse-engineering a data encryption function as efficiently as possible.

Here is the detail of each .cpp file:
 - The original challenge is in the `hireMeRef.cpp` file.
 - The solution (to compile) is in the `hireMeSolution.cpp` file.
 - `hireMeUtility.cpp` contains a few methods I used when iterating and debugging my code: copying, printing, etc.
 - `hireMeOldTest.cpp` regroups some experiments I did to get a better grasp at the encryption algorithm and the matrices used.
 - `hireMeStatistics` use a variant of the solution to print various stats about the algorithm length. To test it, uncomment the `BackwardAnalysis(...)` line in the main file.

### Solution

My solution resolve the level 1 of the challenge: for any given encrypted output, it finds an input that can be given to the encryption function to find the same output. The process is doing a backtracking algorithm by reversing the *confusion* table (which is not injective, so there is no simple inversion table) and trying to rewind the *Forward* method each time the *confusion* table inversion has multiple possibilities (always 2, in practice), it tries them one by one to see if thee is one working. If the inversion has no possibility, then the rewind reached a dead end and must go back to the previous junction to test another path.

The *Forward* algorithm is split into three part, with the two last parts being repeated 256 times (and called jointly the `secondStep` in the code), and the first one being executed once (and forming the `firstStep`). They are briefly exlained here in decreasing order:
 - The third part is directly using the *confusion* table as a look-up table to convert a vector of bytes (what I call a "word"). into another one. The inversion method is directly what I described in the previous paragraph.
 - The second one is a bunch of words multiplied by the bits of the *diffusion* list *xor*ed together. By representing the words and the *diffusion* list as matrices of bits, and considering the field $\mathbb F_2$ (the addition being a *xor*), we can see that this part is actually a matrix multiplication. We can thus reverse the process by doing another multiplication with the matrix inverse of *diffusion*, if it exists. Luckily for us, it exists, and is even equal to *diffusion* itself, the matrix being in fact a vector symmetry (not to be confused with a symmetric matrix), which means that $D² = Id$.
 - The first one is trickier: not only does it only output a half-word (a 16-byte list), but it computes it by the *xor* of two values of the *confusion* table. Reducing two bytes into one loses a full byte of information, so for a half-word we are losing 16 bytes of info. This means that we have $2^{128}$ potential candidates to reverse this part, just because of the *xor*. In fact, the *confusion* table doubles some candidates and removes some others, so in practice we can measure that there are exactly $2^{128}$ candidates. The implemented solution precompute the possible values giving a byte of ouput, and uses once again a backtracking method to tries every possibilities until it finds one that is working.

The resulting algorithm returns a working solution in a few seconds, which corresponds to what is expected of the first level of the challenge. I have not (yet?) found a solution for the second (and third) level, which probably manage to pre-emptively detect paths with no solution. Some more observations on the algorithm can be found on the `Analysis.md` file.