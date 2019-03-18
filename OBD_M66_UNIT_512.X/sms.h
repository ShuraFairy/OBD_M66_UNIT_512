/****************************************************/
/* (С) ООО "ЛУВ", 2019                              */
/* Автор: Петров Александр  						*/
/* Дата: 6.03.2019									*/
/* Описание:                                        */
/****************************************************/

#ifndef SMS_H
#define	SMS_H

#include <stdint.h>

// Таблица 1 . Описание полей пакета PDU передаваемого SMS
// Название поля PDU            |   Длина, байт   |                 Краткое описание
// SCA - Service Center Address |       1-12      |     Номер телефона Центра SMS (может не указываться)
// PDU Type                     |        1        |                     Тип PDU
// MR - Message Reference       |        1        | Порядковый номер сообщения, устанавливаемый самим модулем
// DA - Destination Address     |       2-12      |          Номер телефона получателя сообщения
// PID - Protocol Identifier    |        1        |                Идентификатор протокола
// DCS - Data Coding Scheme     |        1        |               Кодировка сообщения
// VP - Validity Period         |     0,1 или 7   |                 Время жизни SMS
// UDL - User Data Length       |        1        |           Длина поля User Data в байтах
// UD - User Data               |      0–140      |                    Сообщение

//       |  SCA  | PDU-Type | MR |   DA  | PID | DCS |    VP    | UDL |    UD   |
// Byte:  1..12       1      1     2..12    1     1    0,1 or 7    1     0..140

// • PDU Type — тип PDU.
//       | RP | UDHI | SRR |  VPF  | RD |  MTI  |
// Bits: |  7 |  6   |  5  |  4  3 |  2 |  1  0 |

// PDU Type — тип PDU
// – RP (Reply Path):
//      0 - путь для ответа не определен;
//      1 - путь для ответа определен, используется тот же Центр SMS (SMSC);
// – UDHI (User Data Header Included):
//      0 - поле UD содержит только само сообщение;
//      1 - поле UD содержит сообщение и дополнительный заголовок;
// – SRR (Status Report Request):
//      0 - статус сообщения не запрашивается;
//      1 - статус сообщения запрашивается.
// – VPF (Validity Period Format):
//      00 - поле VP отсутствует;
//      01 - зарезервировано;
//      10 - поле VP содержит временные данные в относительном формате;
//      11 - поле VP содержит временные данные в абсолютном формате;
// – RD (Reject Duplicates):
//      0 - SMSC следует переслать сообщение получателю в случае, если оно имеет те же
//          значения полей MR и DA, что и предыдущее сообщение;
//      1 - SMSC следует отклонить сообщение в случае, если оно имеет те же значения полей
//          MR и DA, что и предыдущее сообщение.
// – MTI (Message Type Indicator):
//      00 - принимаемое сообщение (от SMSC к модулю) или подтверждение приема
//           (от модуля к SMSC);
//      01 - отправляемое сообщение (от модуля к SMSC) или подтверждение отправки (от
//           SMSC к модулю);
//      10 - отчет о доставке (от SMSC к модулю) или SMS-команда (от модуля к SMSC);
//      11 - зарезервировано.

// • MR (Message Reference) - порядковый номер сообщения, определяется самим модулем. 
//   В PDU значение поля устанавливается равным 00h.

// • DA (Destination Address) - номер телефона получателя сообщения.
// Поле DA состоит из трех частей:
//  Таблица 2 . Поле DA пакета UDP
//  | Количество цифр в номере получателя | Тип номера получателя | Номер получателя |
//  |                1 байт               |          1 байт       |     0–6 байт     |
// – Количество цифр в номере получателя подсчитывается без учета знака «+» и пред-
// ставляется в шестнадцатеричном формате. Например, для номера +70123456789 значение 
// байта «Количество цифр в номере получателя» равняется 0Bh (11 в десятичной системе счисления).
// – Тип номера получателя.
// В случае международного формата номера байт «Тип номера получателя» устанавливается 
// равным 91h, в случае местного формата - 81h.
//– Номер получателя.       
//  Поле «Номер получателя» формируется следующим образом: 
//   а) в случае международного формата номера знак «+» отбрасывается;
//   б) если количество цифр в номере нечетное, в конце добавляется «F»; 
//   в) цифры номера попарно переставляются местами.
//  Например, для номера +70123456789 поле «Номер получателя» будет иметь
//  вид: 0721436587F9h, а целиком поле DA - 0B910721436587F9h.

// • PID (Protocol Identifier) — идентификатор протокола.
// Поле PID необходимо устанавливать в значение 00h.
// Подробнее об особенностях данного поля можно прочитать в ETSI GSM 03.40, 
// пункт 9.2.3.9.

// • DCS (Data Coding Scheme) — кодировка сообщения.
//   Она описывает, как будет закодировано отсылаемое сообщение.
//      00h: кодировка 7-бит (160 знаков, но не кириллическая, только латинские символы);
//      08h: кодировка UCS2 (тот же Unicode), 70 знаков, 2 байта на символ;
//   Если первым полубайтом указать не «0», а «1» – то сообщение получится типа 
//   Flash – т.е. не осядет в мобильнике получателя, а сразу же выведется ему на экран. 
//      10h: Flash-сообщение, кодировка 7-бит;
//      18h: Flash-сообщение, кодировка UCS2

// • VP (Validity Period) — время жизни сообщения.
//   Время жизни сообщения — время, по истечении которого сообщение уничтожается
//   (стирается из памяти SMSC), если не было доставлено адресату.
//   Это поле связано с битами VPF поля PDU Type, существует три варианта их совместного
//   использования.
//  – Поле VP не используется.
//      В этом случае длина поля VP — 0 байт, биты VPF должны быть установлены в 
//      значение 00;
//  – Поле VP содержит данные о времени жизни в относительном формате.
//      В этом случае длина поля VP — 1 байт, биты VPF должны быть установлены в 
//      значение 10 (в двоичной системе);
//
//  Возможные значения поля VP в случае использования относительного формата времени
// и формулы для расчета соответствующего времени жизни сообщения приведены в таблице 3.
//  – Поле VP содержит данные о времени жизни в абсолютном формате.
//  В этом случае длина поля VP — 7 байт, биты VPF должны быть установлены в значение 11
// (в двоичной системе).

// Таблица 3 . Время жизни SMS при использовании относительного формата времени
// Значение VP       |  Значение VP десятичное |          Соответствующее        | Максимальное время
// шестнадцатеричное |                         |         значению VP время       |        жизни                                                       
//      1..8F        |          0..143         |    (VP + 1) X 5 минут           |      12 часов
//     90..A7        |         144..167        |12 часов + (VP - 143) X 30 минут |       24 часа
//     A8..C4        |         168..196        |        (VP - 166) X 1 день      |       30 дней
//     C5..FF        |         197..255        |      (VP - 192) X 1 неделю      |      63 недели

//  Назначение байт в поле VP при использовании абсолютного формата времени приведено
// в таблице 4.
//  При этом каждый байт содержит по два десятичных числа, переставленных местами.
// Например, байт 2 в случае месяца мая будет иметь значение 50h.
//  Год представлен последними двумя цифрами.
//  Часовой пояс указывает разницу между местным временем и временем по Гринвичу
// (GMT), выраженную в четвертях часа. При этом первый бит указывает знак этой разницы:
// 0 — разница положительная, 1 — разница отрицательная. То есть байт 7 в случае часового
// пояса GMT+3 будет иметь значение 21h.

// Таблица 4 . Поле VP при использовании абсолютного формата времени
//  Байт 1  | Байт 2 |  Байт 3 |  Байт 4  |  Байт 5  |  Байт 6  |    Байт 7
//    Год   |  Месяц |   День  |   Час    |  Минуты  |  Секунды |  Часовой пояс

// !!!!!
//  Для упрощения можно не использовать поле VP. Установить два бита VPF 
// поля PDU Type в значение 00 (поле VP отсутствует). Поэтому в пакете PDU  
// данное поле будет отсутствовать.

// • UDL (User Data Length) — длина поля UD в байтах.
//  При использовании интересующей нас кодировки UCS2 значение поля UDL можно
// вычислить, умножив количество символов в передаваемом сообщении на 2 (каждый символ
// кодируется двумя байтами). Для сообщения «Привет!» значение поля UDL равно 0Eh (14
// в десятичной системе счисления).

// • UD (User Data) — сообщение.
//  Для отправки сообщения кириллицей необходимо использовать кодировку UCS2,
// в этой кодировке каждая буква или символ кодируется двумя байтами.

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
//[Ё]  0401  CYRILLIC CAPITAL LETTER IO
//[Ђ]  0402  CYRILLIC CAPITAL LETTER DJE
//[Ѓ]  0403  CYRILLIC CAPITAL LETTER GJE
//[Є]  0404  CYRILLIC CAPITAL LETTER UKRAINIAN IE
//[Ѕ]  0405  CYRILLIC CAPITAL LETTER DZE
//[І]  0406  CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
//[Ї]  0407  CYRILLIC CAPITAL LETTER YI
//[Ј]  0408  CYRILLIC CAPITAL LETTER JE
//[Љ]  0409  CYRILLIC CAPITAL LETTER LJE
//[Њ]  040A  CYRILLIC CAPITAL LETTER NJE
//[Ћ]  040B  CYRILLIC CAPITAL LETTER TSHE
//[Ќ]  040C  CYRILLIC CAPITAL LETTER KJE
//[?]  040D  CYRILLIC CAPITAL LETTER I WITH GRAVE
//[Ў]  040E  CYRILLIC CAPITAL LETTER SHORT U
//[Џ]  040F  CYRILLIC CAPITAL LETTER DZHE
//[А]  0410  CYRILLIC CAPITAL LETTER A
//[Б]  0411  CYRILLIC CAPITAL LETTER BE
//[В]  0412  CYRILLIC CAPITAL LETTER VE
//[Г]  0413  CYRILLIC CAPITAL LETTER GHE
//[Д]  0414  CYRILLIC CAPITAL LETTER DE
//[Е]  0415  CYRILLIC CAPITAL LETTER IE
//[Ж]  0416  CYRILLIC CAPITAL LETTER ZHE
//[З]  0417  CYRILLIC CAPITAL LETTER ZE
//[И]  0418  CYRILLIC CAPITAL LETTER I
//[Й]  0419  CYRILLIC CAPITAL LETTER SHORT I
//[К]  041A  CYRILLIC CAPITAL LETTER KA
//[Л]  041B  CYRILLIC CAPITAL LETTER EL
//[М]  041C  CYRILLIC CAPITAL LETTER EM
//[Н]  041D  CYRILLIC CAPITAL LETTER EN
//[О]  041E  CYRILLIC CAPITAL LETTER O
//[П]  041F  CYRILLIC CAPITAL LETTER PE
//[Р]  0420  CYRILLIC CAPITAL LETTER ER
//[С]  0421  CYRILLIC CAPITAL LETTER ES
//[Т]  0422  CYRILLIC CAPITAL LETTER TE
//[У]  0423  CYRILLIC CAPITAL LETTER U
//[Ф]  0424  CYRILLIC CAPITAL LETTER EF
//[Х]  0425  CYRILLIC CAPITAL LETTER HA
//[Ц]  0426  CYRILLIC CAPITAL LETTER TSE
//[Ч]  0427  CYRILLIC CAPITAL LETTER CHE
//[Ш]  0428  CYRILLIC CAPITAL LETTER SHA
//[Щ]  0429  CYRILLIC CAPITAL LETTER SHCHA
//[Ъ]  042A  CYRILLIC CAPITAL LETTER HARD SIGN
//[Ы]  042B  CYRILLIC CAPITAL LETTER YERU
//[Ь]  042C  CYRILLIC CAPITAL LETTER SOFT SIGN
//[Э]  042D  CYRILLIC CAPITAL LETTER E
//[Ю]  042E  CYRILLIC CAPITAL LETTER YU
//[Я]  042F  CYRILLIC CAPITAL LETTER YA
//[а]  0430  CYRILLIC SMALL LETTER A
//[б]  0431  CYRILLIC SMALL LETTER BE
//[в]  0432  CYRILLIC SMALL LETTER VE
//[г]  0433  CYRILLIC SMALL LETTER GHE
//[д]  0434  CYRILLIC SMALL LETTER DE
//[е]  0435  CYRILLIC SMALL LETTER IE
//[ж]  0436  CYRILLIC SMALL LETTER ZHE
//[з]  0437  CYRILLIC SMALL LETTER ZE
//[и]  0438  CYRILLIC SMALL LETTER I
//[й]  0439  CYRILLIC SMALL LETTER SHORT I
//[к]  043A  CYRILLIC SMALL LETTER KA
//[л]  043B  CYRILLIC SMALL LETTER EL
//[м]  043C  CYRILLIC SMALL LETTER EM
//[н]  043D  CYRILLIC SMALL LETTER EN
//[о]  043E  CYRILLIC SMALL LETTER O
//[п]  043F  CYRILLIC SMALL LETTER PE
//[р]  0440  CYRILLIC SMALL LETTER ER
//[с]  0441  CYRILLIC SMALL LETTER ES
//[т]  0442  CYRILLIC SMALL LETTER TE
//[у]  0443  CYRILLIC SMALL LETTER U
//[ф]  0444  CYRILLIC SMALL LETTER EF
//[х]  0445  CYRILLIC SMALL LETTER HA
//[ц]  0446  CYRILLIC SMALL LETTER TSE
//[ч]  0447  CYRILLIC SMALL LETTER CHE
//[ш]  0448  CYRILLIC SMALL LETTER SHA
//[щ]  0449  CYRILLIC SMALL LETTER SHCHA
//[ъ]  044A  CYRILLIC SMALL LETTER HARD SIGN
//[ы]  044B  CYRILLIC SMALL LETTER YERU
//[ь]  044C  CYRILLIC SMALL LETTER SOFT SIGN
//[э]  044D  CYRILLIC SMALL LETTER E
//[ю]  044E  CYRILLIC SMALL LETTER YU
//[я]  044F  CYRILLIC SMALL LETTER YA        

//[№]  2116  NUMERO SIGN

//[ё]  0451  CYRILLIC SMALL LETTER IO
//[ђ]  0452  CYRILLIC SMALL LETTER DJE
//[ѓ]  0453  CYRILLIC SMALL LETTER GJE
//[є]  0454  CYRILLIC SMALL LETTER UKRAINIAN IE
//[ѕ]  0455  CYRILLIC SMALL LETTER DZE
//[і]  0456  CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
//[ї]  0457  CYRILLIC SMALL LETTER YI
//[ј]  0458  CYRILLIC SMALL LETTER JE
//[љ]  0459  CYRILLIC SMALL LETTER LJE
//[њ]  045A  CYRILLIC SMALL LETTER NJE
//[ћ]  045B  CYRILLIC SMALL LETTER TSHE
//[ќ]  045C  CYRILLIC SMALL LETTER KJE
//[?]  045D  CYRILLIC SMALL LETTER I WITH GRAVE
//[ў]  045E  CYRILLIC SMALL LETTER SHORT U
//[џ]  045F  CYRILLIC SMALL LETTER DZHE
        
#endif	/* SMS_H */

