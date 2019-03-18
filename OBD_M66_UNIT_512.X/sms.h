/****************************************************/
/* (�) ��� "���", 2019                              */
/* �����: ������ ���������  						*/
/* ����: 6.03.2019									*/
/* ��������:                                        */
/****************************************************/

#ifndef SMS_H
#define	SMS_H

#include <stdint.h>

// ������� 1 . �������� ����� ������ PDU ������������� SMS
// �������� ���� PDU            |   �����, ����   |                 ������� ��������
// SCA - Service Center Address |       1-12      |     ����� �������� ������ SMS (����� �� �����������)
// PDU Type                     |        1        |                     ��� PDU
// MR - Message Reference       |        1        | ���������� ����� ���������, ��������������� ����� �������
// DA - Destination Address     |       2-12      |          ����� �������� ���������� ���������
// PID - Protocol Identifier    |        1        |                ������������� ���������
// DCS - Data Coding Scheme     |        1        |               ��������� ���������
// VP - Validity Period         |     0,1 ��� 7   |                 ����� ����� SMS
// UDL - User Data Length       |        1        |           ����� ���� User Data � ������
// UD - User Data               |      0�140      |                    ���������

//       |  SCA  | PDU-Type | MR |   DA  | PID | DCS |    VP    | UDL |    UD   |
// Byte:  1..12       1      1     2..12    1     1    0,1 or 7    1     0..140

// � PDU Type � ��� PDU.
//       | RP | UDHI | SRR |  VPF  | RD |  MTI  |
// Bits: |  7 |  6   |  5  |  4  3 |  2 |  1  0 |

// PDU Type � ��� PDU
// � RP (Reply Path):
//      0 - ���� ��� ������ �� ���������;
//      1 - ���� ��� ������ ���������, ������������ ��� �� ����� SMS (SMSC);
// � UDHI (User Data Header Included):
//      0 - ���� UD �������� ������ ���� ���������;
//      1 - ���� UD �������� ��������� � �������������� ���������;
// � SRR (Status Report Request):
//      0 - ������ ��������� �� �������������;
//      1 - ������ ��������� �������������.
// � VPF (Validity Period Format):
//      00 - ���� VP �����������;
//      01 - ���������������;
//      10 - ���� VP �������� ��������� ������ � ������������� �������;
//      11 - ���� VP �������� ��������� ������ � ���������� �������;
// � RD (Reject Duplicates):
//      0 - SMSC ������� ��������� ��������� ���������� � ������, ���� ��� ����� �� ��
//          �������� ����� MR � DA, ��� � ���������� ���������;
//      1 - SMSC ������� ��������� ��������� � ������, ���� ��� ����� �� �� �������� �����
//          MR � DA, ��� � ���������� ���������.
// � MTI (Message Type Indicator):
//      00 - ����������� ��������� (�� SMSC � ������) ��� ������������� ������
//           (�� ������ � SMSC);
//      01 - ������������ ��������� (�� ������ � SMSC) ��� ������������� �������� (��
//           SMSC � ������);
//      10 - ����� � �������� (�� SMSC � ������) ��� SMS-������� (�� ������ � SMSC);
//      11 - ���������������.

// � MR (Message Reference) - ���������� ����� ���������, ������������ ����� �������. 
//   � PDU �������� ���� ��������������� ������ 00h.

// � DA (Destination Address) - ����� �������� ���������� ���������.
// ���� DA ������� �� ���� ������:
//  ������� 2 . ���� DA ������ UDP
//  | ���������� ���� � ������ ���������� | ��� ������ ���������� | ����� ���������� |
//  |                1 ����               |          1 ����       |     0�6 ����     |
// � ���������� ���� � ������ ���������� �������������� ��� ����� ����� �+� � ����-
// ���������� � ����������������� �������. ��������, ��� ������ +70123456789 �������� 
// ����� ����������� ���� � ������ ����������� ��������� 0Bh (11 � ���������� ������� ���������).
// � ��� ������ ����������.
// � ������ �������������� ������� ������ ���� ���� ������ ����������� ��������������� 
// ������ 91h, � ������ �������� ������� - 81h.
//� ����� ����������.       
//  ���� ������ ����������� ����������� ��������� �������: 
//   �) � ������ �������������� ������� ������ ���� �+� �������������;
//   �) ���� ���������� ���� � ������ ��������, � ����� ����������� �F�; 
//   �) ����� ������ ������� �������������� �������.
//  ��������, ��� ������ +70123456789 ���� ������ ����������� ����� �����
//  ���: 0721436587F9h, � ������� ���� DA - 0B910721436587F9h.

// � PID (Protocol Identifier) � ������������� ���������.
// ���� PID ���������� ������������� � �������� 00h.
// ��������� �� ������������ ������� ���� ����� ��������� � ETSI GSM 03.40, 
// ����� 9.2.3.9.

// � DCS (Data Coding Scheme) � ��������� ���������.
//   ��� ���������, ��� ����� ������������ ���������� ���������.
//      00h: ��������� 7-��� (160 ������, �� �� �������������, ������ ��������� �������);
//      08h: ��������� UCS2 (��� �� Unicode), 70 ������, 2 ����� �� ������;
//   ���� ������ ���������� ������� �� �0�, � �1� � �� ��������� ��������� ���� 
//   Flash � �.�. �� ������ � ���������� ����������, � ����� �� ��������� ��� �� �����. 
//      10h: Flash-���������, ��������� 7-���;
//      18h: Flash-���������, ��������� UCS2

// � VP (Validity Period) � ����� ����� ���������.
//   ����� ����� ��������� � �����, �� ��������� �������� ��������� ������������
//   (��������� �� ������ SMSC), ���� �� ���� ���������� ��������.
//   ��� ���� ������� � ������ VPF ���� PDU Type, ���������� ��� �������� �� �����������
//   �������������.
//  � ���� VP �� ������������.
//      � ���� ������ ����� ���� VP � 0 ����, ���� VPF ������ ���� ����������� � 
//      �������� 00;
//  � ���� VP �������� ������ � ������� ����� � ������������� �������.
//      � ���� ������ ����� ���� VP � 1 ����, ���� VPF ������ ���� ����������� � 
//      �������� 10 (� �������� �������);
//
//  ��������� �������� ���� VP � ������ ������������� �������������� ������� �������
// � ������� ��� ������� ���������������� ������� ����� ��������� ��������� � ������� 3.
//  � ���� VP �������� ������ � ������� ����� � ���������� �������.
//  � ���� ������ ����� ���� VP � 7 ����, ���� VPF ������ ���� ����������� � �������� 11
// (� �������� �������).

// ������� 3 . ����� ����� SMS ��� ������������� �������������� ������� �������
// �������� VP       |  �������� VP ���������� |          ���������������        | ������������ �����
// ����������������� |                         |         �������� VP �����       |        �����                                                       
//      1..8F        |          0..143         |    (VP + 1) X 5 �����           |      12 �����
//     90..A7        |         144..167        |12 ����� + (VP - 143) X 30 ����� |       24 ����
//     A8..C4        |         168..196        |        (VP - 166) X 1 ����      |       30 ����
//     C5..FF        |         197..255        |      (VP - 192) X 1 ������      |      63 ������

//  ���������� ���� � ���� VP ��� ������������� ����������� ������� ������� ���������
// � ������� 4.
//  ��� ���� ������ ���� �������� �� ��� ���������� �����, �������������� �������.
// ��������, ���� 2 � ������ ������ ��� ����� ����� �������� 50h.
//  ��� ����������� ���������� ����� �������.
//  ������� ���� ��������� ������� ����� ������� �������� � �������� �� ��������
// (GMT), ���������� � ��������� ����. ��� ���� ������ ��� ��������� ���� ���� �������:
// 0 � ������� �������������, 1 � ������� �������������. �� ���� ���� 7 � ������ ��������
// ����� GMT+3 ����� ����� �������� 21h.

// ������� 4 . ���� VP ��� ������������� ����������� ������� �������
//  ���� 1  | ���� 2 |  ���� 3 |  ���� 4  |  ���� 5  |  ���� 6  |    ���� 7
//    ���   |  ����� |   ����  |   ���    |  ������  |  ������� |  ������� ����

// !!!!!
//  ��� ��������� ����� �� ������������ ���� VP. ���������� ��� ���� VPF 
// ���� PDU Type � �������� 00 (���� VP �����������). ������� � ������ PDU  
// ������ ���� ����� �������������.

// � UDL (User Data Length) � ����� ���� UD � ������.
//  ��� ������������� ������������ ��� ��������� UCS2 �������� ���� UDL �����
// ���������, ������� ���������� �������� � ������������ ��������� �� 2 (������ ������
// ���������� ����� �������). ��� ��������� �������!� �������� ���� UDL ����� 0Eh (14
// � ���������� ������� ���������).

// � UD (User Data) � ���������.
//  ��� �������� ��������� ���������� ���������� ������������ ��������� UCS2,
// � ���� ��������� ������ ����� ��� ������ ���������� ����� �������.

//[ ]  0020  SPACE
//[!]  0021  EXCLAMATION MARK
//["]  0022  QUOTATION MARK
//[#]  0023  NUMBER SIGN
//[$]  0024  DOLLAR SIGN
//[%]  0025  PERCENT SIGN
//[&]  0026  AMPERSAND
//[']  0027  APOSTROPHE
//[(]  0028  LEFT PARENTHESIS
//[)]  0029  RIGHT PARENTHESIS
//[*]  002A  ASTERISK
//[+]  002B  PLUS SIGN
//[,]  002C  COMMA
//[-]  002D  HYPHEN-MINUS
//[.]  002E  FULL STOP
//[/]  002F  SOLIDUS
//[0]  0030  DIGIT ZERO
//[1]  0031  DIGIT ONE
//[2]  0032  DIGIT TWO
//[3]  0033  DIGIT THREE
//[4]  0034  DIGIT FOUR
//[5]  0035  DIGIT FIVE
//[6]  0036  DIGIT SIX
//[7]  0037  DIGIT SEVEN
//[8]  0038  DIGIT EIGHT
//[9]  0039  DIGIT NINE
//[:]  003A  COLON
//[;]  003B  SEMICOLON
//[<]  003C  LESS-THAN SIGN
//[=]  003D  EQUALS SIGN
//[>]  003E  GREATER-THAN SIGN
//[?]  003F  QUESTION MARK
//[@]  0040  COMMERCIAL AT
//[A]  0041  LATIN CAPITAL LETTER A
//[B]  0042  LATIN CAPITAL LETTER B
//[C]  0043  LATIN CAPITAL LETTER C
//[D]  0044  LATIN CAPITAL LETTER D
//[E]  0045  LATIN CAPITAL LETTER E
//[F]  0046  LATIN CAPITAL LETTER F
//[G]  0047  LATIN CAPITAL LETTER G
//[H]  0048  LATIN CAPITAL LETTER H
//[I]  0049  LATIN CAPITAL LETTER I
//[J]  004A  LATIN CAPITAL LETTER J
//[K]  004B  LATIN CAPITAL LETTER K
//[L]  004C  LATIN CAPITAL LETTER L
//[M]  004D  LATIN CAPITAL LETTER M
//[N]  004E  LATIN CAPITAL LETTER N
//[O]  004F  LATIN CAPITAL LETTER O
//[P]  0050  LATIN CAPITAL LETTER P
//[Q]  0051  LATIN CAPITAL LETTER Q
//[R]  0052  LATIN CAPITAL LETTER R
//[S]  0053  LATIN CAPITAL LETTER S
//[T]  0054  LATIN CAPITAL LETTER T
//[U]  0055  LATIN CAPITAL LETTER U
//[V]  0056  LATIN CAPITAL LETTER V
//[W]  0057  LATIN CAPITAL LETTER W
//[X]  0058  LATIN CAPITAL LETTER X
//[Y]  0059  LATIN CAPITAL LETTER Y
//[Z]  005A  LATIN CAPITAL LETTER Z
//[[]  005B  LEFT SQUARE BRACKET
//[\]  005C  REVERSE SOLIDUS
//[]]  005D  RIGHT SQUARE BRACKET
//[^]  005E  CIRCUMFLEX ACCENT
//[_]  005F  LOW LINE
//[`]  0060  GRAVE ACCENT
//[a]  0061  LATIN SMALL LETTER A
//[b]  0062  LATIN SMALL LETTER B
//[c]  0063  LATIN SMALL LETTER C
//[d]  0064  LATIN SMALL LETTER D
//[e]  0065  LATIN SMALL LETTER E
//[f]  0066  LATIN SMALL LETTER F
//[g]  0067  LATIN SMALL LETTER G
//[h]  0068  LATIN SMALL LETTER H
//[i]  0069  LATIN SMALL LETTER I
//[j]  006A  LATIN SMALL LETTER J
//[k]  006B  LATIN SMALL LETTER K
//[l]  006C  LATIN SMALL LETTER L
//[m]  006D  LATIN SMALL LETTER M
//[n]  006E  LATIN SMALL LETTER N
//[o]  006F  LATIN SMALL LETTER O
//[p]  0070  LATIN SMALL LETTER P
//[q]  0071  LATIN SMALL LETTER Q
//[r]  0072  LATIN SMALL LETTER R
//[s]  0073  LATIN SMALL LETTER S
//[t]  0074  LATIN SMALL LETTER T
//[u]  0075  LATIN SMALL LETTER U
//[v]  0076  LATIN SMALL LETTER V
//[w]  0077  LATIN SMALL LETTER W
//[x]  0078  LATIN SMALL LETTER X
//[y]  0079  LATIN SMALL LETTER Y
//[z]  007A  LATIN SMALL LETTER Z
//[{]  007B  LEFT CURLY BRACKET
//[|]  007C  VERTICAL LINE
//[}]  007D  RIGHT CURLY BRACKET
//[~]  007E  TILDE
        
//[ ]  00A0  NO-BREAK SPACE
//        
//[?]  0400  CYRILLIC CAPITAL LETTER IE WITH GRAVE
//[�]  0401  CYRILLIC CAPITAL LETTER IO
//[�]  0402  CYRILLIC CAPITAL LETTER DJE
//[�]  0403  CYRILLIC CAPITAL LETTER GJE
//[�]  0404  CYRILLIC CAPITAL LETTER UKRAINIAN IE
//[�]  0405  CYRILLIC CAPITAL LETTER DZE
//[�]  0406  CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
//[�]  0407  CYRILLIC CAPITAL LETTER YI
//[�]  0408  CYRILLIC CAPITAL LETTER JE
//[�]  0409  CYRILLIC CAPITAL LETTER LJE
//[�]  040A  CYRILLIC CAPITAL LETTER NJE
//[�]  040B  CYRILLIC CAPITAL LETTER TSHE
//[�]  040C  CYRILLIC CAPITAL LETTER KJE
//[?]  040D  CYRILLIC CAPITAL LETTER I WITH GRAVE
//[�]  040E  CYRILLIC CAPITAL LETTER SHORT U
//[�]  040F  CYRILLIC CAPITAL LETTER DZHE
//[�]  0410  CYRILLIC CAPITAL LETTER A
//[�]  0411  CYRILLIC CAPITAL LETTER BE
//[�]  0412  CYRILLIC CAPITAL LETTER VE
//[�]  0413  CYRILLIC CAPITAL LETTER GHE
//[�]  0414  CYRILLIC CAPITAL LETTER DE
//[�]  0415  CYRILLIC CAPITAL LETTER IE
//[�]  0416  CYRILLIC CAPITAL LETTER ZHE
//[�]  0417  CYRILLIC CAPITAL LETTER ZE
//[�]  0418  CYRILLIC CAPITAL LETTER I
//[�]  0419  CYRILLIC CAPITAL LETTER SHORT I
//[�]  041A  CYRILLIC CAPITAL LETTER KA
//[�]  041B  CYRILLIC CAPITAL LETTER EL
//[�]  041C  CYRILLIC CAPITAL LETTER EM
//[�]  041D  CYRILLIC CAPITAL LETTER EN
//[�]  041E  CYRILLIC CAPITAL LETTER O
//[�]  041F  CYRILLIC CAPITAL LETTER PE
//[�]  0420  CYRILLIC CAPITAL LETTER ER
//[�]  0421  CYRILLIC CAPITAL LETTER ES
//[�]  0422  CYRILLIC CAPITAL LETTER TE
//[�]  0423  CYRILLIC CAPITAL LETTER U
//[�]  0424  CYRILLIC CAPITAL LETTER EF
//[�]  0425  CYRILLIC CAPITAL LETTER HA
//[�]  0426  CYRILLIC CAPITAL LETTER TSE
//[�]  0427  CYRILLIC CAPITAL LETTER CHE
//[�]  0428  CYRILLIC CAPITAL LETTER SHA
//[�]  0429  CYRILLIC CAPITAL LETTER SHCHA
//[�]  042A  CYRILLIC CAPITAL LETTER HARD SIGN
//[�]  042B  CYRILLIC CAPITAL LETTER YERU
//[�]  042C  CYRILLIC CAPITAL LETTER SOFT SIGN
//[�]  042D  CYRILLIC CAPITAL LETTER E
//[�]  042E  CYRILLIC CAPITAL LETTER YU
//[�]  042F  CYRILLIC CAPITAL LETTER YA
//[�]  0430  CYRILLIC SMALL LETTER A
//[�]  0431  CYRILLIC SMALL LETTER BE
//[�]  0432  CYRILLIC SMALL LETTER VE
//[�]  0433  CYRILLIC SMALL LETTER GHE
//[�]  0434  CYRILLIC SMALL LETTER DE
//[�]  0435  CYRILLIC SMALL LETTER IE
//[�]  0436  CYRILLIC SMALL LETTER ZHE
//[�]  0437  CYRILLIC SMALL LETTER ZE
//[�]  0438  CYRILLIC SMALL LETTER I
//[�]  0439  CYRILLIC SMALL LETTER SHORT I
//[�]  043A  CYRILLIC SMALL LETTER KA
//[�]  043B  CYRILLIC SMALL LETTER EL
//[�]  043C  CYRILLIC SMALL LETTER EM
//[�]  043D  CYRILLIC SMALL LETTER EN
//[�]  043E  CYRILLIC SMALL LETTER O
//[�]  043F  CYRILLIC SMALL LETTER PE
//[�]  0440  CYRILLIC SMALL LETTER ER
//[�]  0441  CYRILLIC SMALL LETTER ES
//[�]  0442  CYRILLIC SMALL LETTER TE
//[�]  0443  CYRILLIC SMALL LETTER U
//[�]  0444  CYRILLIC SMALL LETTER EF
//[�]  0445  CYRILLIC SMALL LETTER HA
//[�]  0446  CYRILLIC SMALL LETTER TSE
//[�]  0447  CYRILLIC SMALL LETTER CHE
//[�]  0448  CYRILLIC SMALL LETTER SHA
//[�]  0449  CYRILLIC SMALL LETTER SHCHA
//[�]  044A  CYRILLIC SMALL LETTER HARD SIGN
//[�]  044B  CYRILLIC SMALL LETTER YERU
//[�]  044C  CYRILLIC SMALL LETTER SOFT SIGN
//[�]  044D  CYRILLIC SMALL LETTER E
//[�]  044E  CYRILLIC SMALL LETTER YU
//[�]  044F  CYRILLIC SMALL LETTER YA        

//[�]  2116  NUMERO SIGN

//[�]  0451  CYRILLIC SMALL LETTER IO
//[�]  0452  CYRILLIC SMALL LETTER DJE
//[�]  0453  CYRILLIC SMALL LETTER GJE
//[�]  0454  CYRILLIC SMALL LETTER UKRAINIAN IE
//[�]  0455  CYRILLIC SMALL LETTER DZE
//[�]  0456  CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
//[�]  0457  CYRILLIC SMALL LETTER YI
//[�]  0458  CYRILLIC SMALL LETTER JE
//[�]  0459  CYRILLIC SMALL LETTER LJE
//[�]  045A  CYRILLIC SMALL LETTER NJE
//[�]  045B  CYRILLIC SMALL LETTER TSHE
//[�]  045C  CYRILLIC SMALL LETTER KJE
//[?]  045D  CYRILLIC SMALL LETTER I WITH GRAVE
//[�]  045E  CYRILLIC SMALL LETTER SHORT U
//[�]  045F  CYRILLIC SMALL LETTER DZHE
        
#endif	/* SMS_H */

