# Задача 4

## Часть 1
Разработать консольную утилиту **converter**, которая позволяет провести конвертацию изображения в формате bmp в негатив.

При этом необходимо поддержать обработку двух типов bmp файлов версии 3: использующих 8 битов или 24 бита для хранения цвета пикселя. При этом необходимо предоставить пользователю две возможности: использовать «самописную» реализацию конвертера, а также реализацию из библиотеки [qdbmp](https://github.com/madwyn/qdbmp). При использовании самостоятельной конвертации для других форматов приложение должно выводить сообщение о неподдерживаемом формате и возвращать код возврата -1. Этот же код необходимо возвращать для иных ошибок, не связанных со нарушением структуры формата. Если же при разборе структуры изображения возникли ошибки, необходимо вывести *понятное* сообщение о сути проблемы и вернуть код возврата -2. При использовании третьесторонней реализации в случае любых ошибок необходимо вывести сообщение о проблеме в произвольной форме и вернуть -3.

Для перевода пикселя в негатив необходимо использовать **битовые операции**. Перевод изображений, использующих 8 бит для хранения цвета пикселя, осуществляется на основе инвертирования таблицы цветов.

Утилита должна обрабатывать следующие аргументы командной строки:

\-\-mine или \-\-theirs для выбора алгоритма конвертации,

а также информировать пользователя о некорректности ввода в случае отсутствия каких-либо из указанных аргументов или присутствия «мусора».

**Пример:** converter \-\-mine &lt;input\_name&gt;.bmp &lt;output\_name&gt;.bmp

## Часть 2
Разработать консольную утилиту **comparer**, которая позволяет сравнить попиксельно два bmp файла. 

Если все пиксели изображений совпали, то утилита возращает 0, в противном случае осуществляет запись координат первой сотни не совпадающих в точности пикселей (или всех, если их меньше сотни) в стандартный поток ошибок в формате 

x1 y1 

x2 y2

…

Если изображения имеют разный размер или присутствуют иные причины, препятствующие адекватному сравнению, приложение должно вернуть код возврата -1 и вывести *понятное* сообщение о проблеме, этому сравнению препятствующей.

**Пример:** comparer &lt;file1\_name&gt;.bmp &lt;file2\_name&gt;.bmp

**Примечание:** кажется, что код возврата при сравнении изображений, сконвертированных из одного исходного в негатив при помощи самостоятельной и предложенной третьесторонней реализации, должен быть равен нулю.

## Работа с репозиторием
Необходимо создать папку *src/*, в которую необходимо поместить файлы с кодом. В файле CMakeLists.txt необходимо при создании target'ов *converter* и *comparer* в *add\_executable*  прописать путь до всех файлов, которые неоходимы для компиляции данного target'а.

