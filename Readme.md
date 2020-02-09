﻿## Реализация структуры "Базисное дерево" (также известное как "Компактное префексное дерево", RadixTree, RadixTrie)

### Поддерживаемые методы
 - **append(const string&)** - добавление строки в дерево

 - **print()** - печати добавленных имён с их короткими именами
 
 - **printTree()** - отладочная печать внутренней структуры дерева (в случае использования национальных алфавитов и кодировки UTF8, следует включить режим печати шестнадцатиричных кодов, см. ниже раздел "Настройки")

### Настройки

 - Чтобы указать на то, что данные будут приходить в однобайтовой кодировке надо сбросить флаг isUseUTF8 (а как иначе - откуда программе знать что данные будут приходить в однобайтовой не-UTF8 кодировке):
```
RadixTrie rtree;
rtree.isUseUTF8 = false;
```

 - При печати дерева включены специальные символы (линии зависимостей), их можно отключить сбросив флаг isOutSpecForDeps:
```
RadixTrie rtree;
rtree.isOutSpecForDeps = false;
```
Примечание(!): при проверке работы c не-UTF8 (с любой однобайтовой кодировкой) - вывод "линий зависимостей" следует отключать (в main.cpp - прописано автоматическое отключение для не-UTF8)

 - Настроить отступ можно указав его в поле sGap (по умолчанию символ табуляции "\t":
```
RadixTrie rtree;
rtree.sGap = "  ";
```


 - При отладочной печати дерева (методом printTree)? в режиме использования кодировки UTF8 (rtree.isUseUTF8 == true), нужно включить режим вывода
шестнадцатриричных кодов, установкой флага isOutCodesInPrintTree (иначе теряется смысл отображения - "расщеплённые" символы консоль никак не сможет отображать):
```
RadixTrie rtree;
rtree.isOutCodesInPrintTree = true;
```