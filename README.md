# Декомпилятор байткода классического Рефала-5

Декомпилятор может реконструировать RSL’ки, созданные при помощи `refc`
или `crefal`’а (последний — в режиме по умолчанию). При этом сгенерированные
файлы можно снова откомпилировать `refc` или `crefal`’ом и получить идентичную
RSL’ку. Алгоритмы компиляции `refc` и `crefal`’а немного различаются, поэтому
идентичный файл RASL’а получится только при использовании того же компилятора.

`crefal` может создавать обфусцированные RSL’ки (в частности, он в таком виде
и распространяется) — имена функций заменяются на последовательности пробелов,
табуляций, переводов строк и некоторых других непечатаемых знаков. При
декомпиляции такие функции получают имена `Fnnn`, где `nnn` — смещение функции
в байткоде в виде десятичного числа.

Декомпилятор успешно справляется с корректными RSL’ками (включая обфусцированные
`crefal`’ом), если на вход положить некорректный файл, то декомпилятор упадёт
с выдачей дампа, либо сформирует файл с какой-то ерундой.

## Использование

Командная строка:

    decompiler.exe filename1.rsl filename2.rsl ...

Для каждого указанного файла будет сформирован файл с именем
`filename-decompiled.ref`, в котором будет располагаться исходный текст
на Рефале, в конце файла в виде комментариев будет распечатан байткод
в символьной форме.

Читабельность сгенерированного файла не ставилась в приоритет, поэтому
генерируемый текст можно назвать читабельным лишь условно. Отступы
в результирующем файле отражают логическую структуру программы (правые части,
и условия имеют дополнительный отступ от образцов, блоки выписываются
с дополнительным отступом). Пробелы в выражениях логичным образом разбивают
конструкции — обязательны пробелы вокруг слов, чисел, выражений в кавычках,
но внутри скобок пробелы не ставятся. Недостатком вывода является разбиение
длинных выражений в рандомных местах — разбиение делалось не для читабельности,
а для того, чтобы не получалось длинных строк. Дело в том, что `refc`
не поддерживает строки длиной более 200 знаков, поэтому приходится их бить.
