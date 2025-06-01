# Hire-me challenge - NERD

This is my attempt at the [HireMe.c](https://www.nerd.nintendo.com/files/HireMe) challenge proposed by [Nintendo European Research & Development](https://www.nerd.nintendo.com/) on their website. The challenge consists in reverse-engineering a data encryption function as efficiently as possible.

Here is the detail of each .cpp file:
 - The original challenge is in the `hireMeRef.cpp` file.
 - The solution (to compile) is in the `hireMeSolution.cpp` file.
 - `hireMeUtility.cpp` contains a few methods I used when iterating and debugging my code: copying, printing, etc.
 - `hireMeOldTest.cpp` regroups some experiments I did to get a better grasp at the encryption algorithm and the matrices used.
 - `hireMeStatistics` use a variant of the solution to print various stats about the algorithm length. To test it, uncomment the `BackwardAnalysis(...)` line in the main file.

## Solution

My solution resolve the level 1 of the challenge: for any given encrypted output, it finds an input that can be given to the encryption function to find the same output. The process is doing a backtracking algorithm by reversing the *confusion* table (which is not injective, so there is no simple inversion table) and trying to rewind the *Forward* method each time the *confusion* table inversion has multiple possibilities (always 2, in practice), it tries them one by one to see if thee is one working. If the inversion has no possibility, then the rewind reached a dead end and must go back to the previous junction to test another path.

The *Forward* algorithm is split into three part, with the two last parts being repeated 256 times (and called jointly the `secondStep` in the code), and the first one being executed once (and forming the `firstStep`). They are briefly exlained here in decreasing order:
 - The third part is directly using the *confusion* table as a look-up table to convert a vector of bytes (what I call a "word"). into another one. The inversion method is directly what I described in the previous paragraph.
 - The second one is a bunch of words multiplied by the bits of the *diffusion* list *xor*ed together. By representing the words and the *diffusion* list as matrices of bits, and considering the field $\mathbb F_2$ (the addition being a *xor*), we can see that this part is actually a matrix multiplication. We can thus reverse the process by doing another multiplication with the matrix inverse of *diffusion*, if it exists. Luckily for us, it exists, and is even equal to *diffusion* itself, the matrix being in fact a vector symmetry (not to be confused with a symmetric matrix), which means that $D² = Id$.
 - The first one is trickier: not only does it only output a half-word (a 16-byte list), but it computes it by the *xor* of two values of the *confusion* table. Reducing two bytes into one loses a full byte of information, so for a half-word we are losing 16 bytes of info. This means that we have $2^{128}$ potential candidates to reverse this part, just because of the *xor*. In fact, the *confusion* table doubles some candidates and removes some others, so in practice we can measure that there are exactly $2^{128}$ candidates. The implemented solution precompute the possible values giving a byte of ouput, and uses once again a backtracking method to tries every possibilities until it finds one that is working.

The resulting algorithm returns a working solution in a few seconds, which corresponds to what is expected of the first level of the challenge. I have not (yet?) found a way to make it fask enough for the second (and third) level. To speed up the reversion algorithm means to reduce the number of paths to take, and so to detect wrong paths before reaching a hole. To reduce this number, it's good to know where the complexity resides.

## Analysis

Part 1 of the algorithm produces a lot of path: at this point, there are no hole yet, so we cannot detect if a path is good or not. The algo produces every possible byte "inversion" possibles. We can see by iterating through the *confusion* table that for every potential output byte, the number of corresponding inputs is always 256 (what a lucky coincidence). So for a half-word output, there exist $256^{16} = 2^{128}$ inputs, and we test them one by one by until we find a solution. Clearly, there is room for improvement here, even if it's unclear how much tests are needed until we find a solution on average.


Part 2 is a matrix multiplication and thus is of constant complexity. There are no potential improvements here, though the matrix might be important for improving the other parts.


For part 3, the backtracking method should not be too costly since the junctions occurs (probabilistically) as often as the hole. Since we can pre-detect the holes at each cycle before going through the third part, we can avoid even more paths. Here is a rough estimation of the `secondStep` length:

On the *confusion* table, there are 256 potential values, but 16 doubles and 16 holes. We will suppose there that at each iteration, the bytes are uniformely distributed and independant (which is false in practice, but convenient for mean calculations).

Let $X_i$ be the random variable indicating the number of times the part 3 is reached (combining every path followed), starting from the $i$th iteration, $0 \leq i \le 255$.
For ease of calculations, we also suppose that the algorithm doesn't terminate when reaching the solution, and instead go back to another junction to try all the possible paths (without holes).  This means that $X_i$ will actually be a majoration of the length of the algorithm. This is equivalent to saying that we will try to find every possible solutions, and not just one. 

We note $p$ the probability that an iteration contains no hole in its word. With our hypothesis, $p = \left( \frac{256 - 16}{256} \right)^{32} = \left( \frac{15}{16} \right)^{32}\simeq 0.1268$.

We estimate the mean value with a recursive formula summing the length over the possibilities of having $k = 0, 1, 2\dots$ junction (and so $2^k$ paths to follow).

$$\mathbb E(X_i) = (1 - p) 0 + p\left[ 1 + \sum_{k=0}^{32}\binom{32}{k} \left( \frac{1}{15} \right)^{k} \left( \frac{14}{15} \right)^{32 - k}\left( 2^k \mathbb E(X_{i+1}) \right) \right]$$

The presence of 14s and 15s comes from the fact that we already suppose that there are no holes when we iterate, so only 240 possible values.

$$\mathbb E(X_i) = p\left( 1 + \mathbb E(X_{i+1}) \left( \frac{16}{15} \right)^{32} \right) = p + \mathbb E(X_{i+1})$$

Stopping after 256 iterations, this means we have $\mathbb E(X_i) = 256\left( \frac{15}{16} \right)^{32} \simeq 32.46$. This is reasonable for a computer, but remember that we are on a theoretical model with rather strong hypothesis: namely, the independancy of the bytes between iterations, and the uniform distribution of the bytes.


We can compare the theory with the practice using the `BackwardAnalysis()` method: it follows the reversion algorithm on a bunch of outputs, and measure the call to the different parts of the algorithm. Here are the stats:

On average,
 - It takes 2.10533 seconds to find the solution.
 - It takes 0.735797 seconds to find the solution during the call to the correct __second step__.
 - The __second step__ (the 256 iterations of parts 2 and 3) is called 4980.36 times.
 - For a call to the second step, __part 2__ is called 267.458 times.
 - For a call to the second step, __part 3__ is called 34.0912 times.
 - During the call to the correct second step, __part 2__ is called 198363 times.
 - During the call to the correct second step, __part 3__ is called 25249 times.

Note that there is a considerable variance on those estimates, since we are dealing with algorithm with an exponential complexity: the estimates can vary by two order of magnitudes. A geometric mean would give a better idea of the length of the algorithm on typical scenarios (where holes are not too deep), but here we look at the general length.


We can observe that the first step produces about 20 times more branches than the second step, as expected; this is probably the most important part to improve in order to reach level 2.

The difference between the number of calls to part 2 and part 3 shows how valuable pre-checking the holes is : only about 12.7% of iterations contain no hole (as predicted by the maths), and even if we can't reach the next iteration when there are holes, we avoid creating junctions just before the dead end.

What's more, the estimate for $X_i$ pretty close to the theoretical value. This is particularly intriguing as we supposed that the algorithm was trying to find every possible solutions when doing the maths. It can be explained by remarking that almost every call to the second step ended without finding a solution, and when that happens, the algorithm effectively tests all paths.