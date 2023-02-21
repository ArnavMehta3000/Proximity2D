#pragma once


template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

#define COM_RELEASE(x) SafeRelease(x.GetAddressOf())
#define SAFE_DELETE(ptr) if (ptr){ delete ptr; ptr = nullptr;}

// Create and zero's memory of struct
#define CREATE_ZERO(type, varName) type varName; ZeroMemory(&varName, sizeof(type))

#define STRINGIFY(x) #x
