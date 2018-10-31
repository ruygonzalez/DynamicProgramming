/*
 * SeamCarveAlgorithm.cpp
 * Defines the seam carving algorithm.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include "SeamCarveApp.hpp"

#define min(x, y)           ((x) < (y) ? (x) : (y))

/*
 * Returns the value to be stored in cost table for point (c,r)
 */
int getValue(int r, int c, int w, int h, int **cost,unsigned int **smap){

// If on top row then return saliency value from smap
    if(r == 0)
        return smap[c][r];
    int a;
    int b;
    int d;
// If point on left border then only points above and above/to the right are possible
    if(c == 0){
        a = cost[c+1][r-1];
        b = cost[c][r-1];
        if(a < b)
            return a + smap[c][r];
        return b + smap[c][r];
    }

// If point on right border then only points above and above/to the left are possible 
    if(c == w-1){
        a = cost[c][r-1];
        b = cost[c-1][r-1];
        if(a < b)
            return a + smap[c][r];
        return b + smap[c][r];
    }

// Otherwise, there are three possible points, check all to find the minimum path value and add to this point's saliency value 
    a = cost[c][r-1];
    b = cost[c-1][r-1];
    d = cost[c+1][r-1];
    int min = a;
    if (b < a)
        min = b;
    if(d < min)
        min = d;
    return min + smap[c][r];
}

/*
 * This function returns the x-coordinate of the point above (c,r) with the mimumum value in the  cost table
 * Returned value will eventually be added into seam, as it is part of the optimal seam
 */
int getNext(int r, int c, int w, int h, int **cost){
// If on top row then return the x coordinate, c, of this point
    if(r == 0)
        return c;
    int a;
    int b;
    int d;

// If point on left border then only points above and above/to the right are possible
    if(c == 0){
        a = cost[c+1][r-1];
        b = cost[c][r-1];
        if(a < b)
            return c+1;
        return c;
    }

// If point on right border then only points above and above/to the left are possible
    if(c == w-1){
        a = cost[c][r-1];
        b = cost[c-1][r-1];
        if(a < b)
            return c;
        return c-1;
    }

// Otherwise, there are three possible points, check all to find the minimum path value and return the x-coordinate of that point
    a = cost[c][r-1];
    b = cost[c-1][r-1];
    d = cost[c+1][r-1];
    int min = a;
    int temp = c;
    if (b < min){
        min = b;
        temp = c-1;
    }   
    if(d < min)
        return c+1;
    return temp;
}

/**
 * @brief Peforms the seam carving algorithm.
 *
 * @param smap 2-d saliency map with width `w` and height `h`; can be
 * indexed by `smap[i][j]`
 *
 * @param w Width of the saliency map
 *
 * @param h Height of the saliency map
 *
 * @return An array of the x-coordinates of the seam, starting from the top of
 * the image.
 */
unsigned int *DoSeamCarve(unsigned int **smap, int w, int h)
{
// Create a cost table, in the form of a 2-D Array
    int **cost = new int*[w];
    for(int i = 0; i < w; i++)
        cost[i] = new int[h];

// Add values to the cost table using getValue
    for(int r = 0; r < h; r ++)
        for(int c = 0; c < w; c++)
            cost[c][r] = getValue(r, c, w, h, cost, smap);

// Find the index (x-coordinate) of the point on the bottom row with the minimum value in the cost table (minimum path to get there)
    int index = 0;
    for(int c = 1; c < w; c++)
        if(cost[c][h-1] < cost[index][h-1])
            index = c;

// Create temp and seam two arrays. Seam will end up being the reversed version of temp
    unsigned int *temp = new unsigned int[h];
    unsigned int *seam = new unsigned int[h];

// Add the point on bottom row with least cost value to temp
    temp[0] = index;

// Proceed to add the value above (directly, to right, or to left) with lowest cost value until you get to the top, to temp
    int ind = 1;                    
    int counter = h-1;
    while(counter > 0){
        temp[ind] = getNext(counter, temp[ind-1], w, h, cost);
        ind ++;
        counter --;
    }
    
// Add the values in temp to seam in reverse order
    for (int i = 0; i < h; i++)
    {
        seam[i] = temp[h-1-i];
    }

// Clear memory for the 2-D array cost
    for(int i = 0; i < w; i++)
       delete[] cost[i];
    delete[] cost;

// Return the optimal seam
    return seam;
}

