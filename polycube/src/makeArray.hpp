// polycube:  A polyomino and polycube puzzle solver.
// Copyright 2010 Matthew T. Busche.
//
// This program is free software:  you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MAKEARRAY_HPP
#define MAKEARRAY_HPP

#include <stdlib.h>
#include <iostream>

template <class T>
inline T* makeArray(int dim1)
{
    if(dim1 == 0)
        return NULL;
    T* a = new T[dim1]; 
    return a;
}

template <class T>
inline T** makeArray(int dim1, int dim2)
{
    if(dim1 == 0 || dim2 == 0)
        return NULL;
    T** a = new T*[dim1];
    T* b = new T[dim1*dim2];
    for(int i = 0; i < dim1; ++i)
        a[i] = b + i*dim2;
    return a;
}

template <class T>
inline T*** makeArray(int dim1, int dim2, int dim3)
{
    if(dim1 == 0 || dim2 == 0 || dim3 == 0)
        return NULL;
    T*** a = new T**[dim1];
    T** b = new T*[dim1*dim2];
    T* c = new T[dim1*dim2*dim3];
    for(int i = 0; i < dim1; ++i)
    {
        a[i] = b + i*dim2;
        for(int j = 0; j < dim2; ++j)
        {
            a[i][j] = c + i*dim2*dim3 + j*dim3;
        }
    }
    return a;
}

template <class T>
inline T**** makeArray(int dim1, int dim2, int dim3, int dim4)
{
    if(dim1 == 0 || dim2 == 0 || dim3 == 0 || dim4 == 0)
        return NULL;
    T**** a = new T***[dim1];
    T*** b = new T**[dim1*dim2];
    T** c = new T*[dim1*dim2*dim3];
    T* d = new T[dim1*dim2*dim3*dim4];
    for(int i = 0; i < dim1; ++i)
    {
        a[i] = b + i*dim2;
        for(int j = 0; j < dim2; ++j)
        {
            a[i][j] = c + i*dim2*dim3 + j*dim3;
            for(int k = 0; k < dim3; ++k)
            {
                a[i][j][k] = c + i*dim2*dim3*dim4 + j*dim3*dim4 + k*dim4;
            }
        }
    }
    return a;
}

template <class T>
inline T* initArray(int dim1, const T& init)
{
    if(dim1 == 0)
        return NULL;
    T* a = new T[dim1];
    for(int i = 0; i < dim1; ++i)
        a[i] = init;
    return a;
}

template <class T>
inline T** initArray(int dim1, int dim2, const T& init)
{
    if(dim1 == 0 || dim2 == 0)
        return NULL;
    T** a = new T*[dim1];
    T* b = new T[dim1*dim2];
    for(int i = 0; i < dim1; ++i)
    {
        a[i] = b + i*dim2;
        for(int j = 0; j < dim2; ++j)
            a[i][j] = init;
    }
    return a;
}

template <class T>
inline T*** initArray(int dim1, int dim2, int dim3, const T& init)
{
    if(dim1 == 0 || dim2 == 0 || dim3 == 0)
        return NULL;
    T*** a = new T**[dim1];
    T** b = new T*[dim1*dim2];
    T* c = new T[dim1*dim2*dim3];
    for(int i = 0; i < dim1; ++i)
    {
        a[i] = b + i*dim2;
        for(int j = 0; j < dim2; ++j)
        {
            a[i][j] = c + i*dim2*dim3 + j*dim3;
            for(int k = 0; k < dim3; ++k)
                a[i][j][k] = init;
        }
    }
    return a;
}

template <class T>
inline T**** initArray(int dim1, int dim2, int dim3, int dim4, const T& init)
{
    if(dim1 == 0 || dim2 == 0 || dim3 == 0 || dim4 == 0)
        return NULL;
    T**** a = new T***[dim1];
    T*** b = new T**[dim1*dim2];
    T** c = new T*[dim1*dim2*dim3];
    T* d = new T[dim1*dim2*dim3*dim4];
    for(int i = 0; i < dim1; ++i)
    {
        a[i] = b + i*dim2;
        for(int j = 0; j < dim2; ++j)
        {
            a[i][j] = c + i*dim2*dim3 + j*dim3;
            for(int k = 0; k < dim3; ++k)
            {
                a[i][j][k] = c + i*dim2*dim3*dim4 + j*dim3*dim4 + k*dim4;
                for(int l = 0; l < dim4; ++l)
                    a[i][j][k][l] = init;
            }
        }
    }
    return a;
}

template <class T>
inline void deleteArray(T**** a, int, int, int, int)
{
    if(a == NULL)
        return;
    delete [] a[0][0][0];
    delete [] a[0][0];
    delete [] a[0];
    delete [] a;
    a = NULL;
}

template <class T>
inline void deleteArray(T*** a, int, int, int)
{
    if(a == NULL)
        return;
    delete [] a[0][0];
    delete [] a[0];
    delete [] a;
    a = NULL;
}

template <class T>
inline void deleteArray(T** a, int, int)
{
    if(a == NULL)
        return;
    delete [] a[0];
    delete [] a;
    a = NULL;
}

template <class T>
inline void deleteArray(T* a, int)
{
    if(a == NULL)
        return;
    delete [] a;
    a = NULL;
}

#endif
