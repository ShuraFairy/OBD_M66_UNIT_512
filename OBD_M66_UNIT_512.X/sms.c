/****************************************************/
/* (�) ��� "���", 2019                              */
/* �����: ������ ���������  						*/
/* ����: 6.03.2019									*/
/* ��������:                                        */
/****************************************************/
#include "sms.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// intput - phoneNumber[N] ������������� '\0'
// output - 
char * EncodePhoneNumber(const char phoneNumber[])
{
    char * result   = "";
    int size        = 0;
    size_t i        = 0;
    char temp[16];
        
    size = strlen(phoneNumber); //
    for(i = 1; i <= size; ++i)  // �������� "+" �� ������, ���� �� ���� (+79774874616)
    {
        if(i == size)
            temp[i] = '\0';
        else            
            temp[i-1] = phoneNumber[i];
        
    }
        
    if(((strlen(temp) % 2)) > 0)    // ���� ����� �� ������ ��������� � ����� "F"
        temp[size + 1] = 'F';
    
    for(i = 0; i < strlen(temp); i +=2 );   // ������ ���� ���� � ������ �������������� �������
    {
        result[i]       = temp[i + 1];
        result[i + 1]   = temp[i];        
    }
    
    return result;
}
////////////////////////////////////////////////////////////////////////////////
// intput -  str[N] ������������� '\0'
// output - 
char * stringToUCS2(const char str[])
{
    char * result   = "";
    size_t i        = 0;
//    int16_t temp = 0;
    
    for(i = 0; i < strlen(str); ++i)   
    {
    }
    return result;
}
////////////////////////////////////////////////////////////////////////////////
// intput -  str[N] ������������� '\0'
// output -    
char * ucs2ToString(const char str[])
{
    char * result   = "";
//    int16_t temp = 0;
    
    return result;
}
////////////////////////////////////////////////////////////////////////////////
