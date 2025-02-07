#pragma once

#include <string>

std::string ReverseString(std::string & str)
{
    std::string new_str;
    for (int i = str.length() - 1; i >= 0 ; i--)
    {
        new_str += str[i];
    }
    return new_str;
}

std::string IntToBits(unsigned int n)
{
    std::string bit;
    while (n > 0)
    {
        bit += std::to_string(n % 2);
        n /= 2;
    }
    
    return ReverseString(bit);
}

std::string IntToHex(unsigned int n)
{
    std::string txt;
    std::string d;
    if(n == 0)
    {
        txt = "0x0";
        return txt;
    }
    while (n > 0)
    {
        if((0 <= (n % 16)) & ((n % 16) <= 9))
        {
            d = std::to_string(n%16);
        } else
        {
            switch (n % 16)
            {
            case 10:
                d = 'A';
                break;
            case 11:
                d = 'B';
                break;
            case 12:
                d = 'C';
                break;
            case 13:
                d = 'D';
                break;
            case 14:
                d = 'E';
                break;
            case 15:
                d = 'F';
                break;
            default:
                d = "DILAVNI"; //Invalid reversed
                break;
            }
        }
        txt += d;
        n /= 16;
    }
    
    return "0x" + ReverseString(txt);
}