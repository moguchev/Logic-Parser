# Task-2
Парсер, логические выражения.

Требуется написать программу, которая способна вычислять логическе выражения. 
Допустимые выражения чувствительны к регистру и могут содержать: 
1) знаки операций 'and' - и, 'or' - или, 'not' - отрицание, 'xor' - сложение по модулю 2 
2) Скобки '(', ')' 
3) Логические значения 'True', 'False' 
4) Пустое выражение имеет значение "False" 

[]Также требуется реализовать возможность задания переменных, которые могут состоять только из маленьких букв латинского алфавита (к примеру, 'x=True'). 
[]Объявление переменной имеет формат: <имя>=<True|False>; // допустимы пробелы
[]Допускается несколько объявлений одной и той же переменной, учитывается последнее. 
[]Все переменные, используемые в выражении, должны быть заданы заранее. Также запрещены имена, совпадающие с ключевыми словами (and, or и т.д.). 
[]Необходимо учитывать приоритеты операций. Пробелы ничего не значат. 
[]Если выражение или объявление не удовлетворяют описанным правилам, требуется вывести в стандартный поток вывода "[error]" (без кавычек). 

