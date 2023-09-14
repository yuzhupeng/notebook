# 循环左移,右移

循环左移,右移

```c
DWORD fROL(DWORD data, int length)
{
    DWORD result = 0;
    result = (data << length) | (data >> (32 - length));
    return result;
}
/*循环右移*/
DWORD fROR(DWORD data, int length)
{
    DWORD result = 0;
    result=(data >> length) | (data << (32 - length));
    return result;
}
```

