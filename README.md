# 1. (Де)кодер ASCII85

Задача — написать (де)кодер [ASCII85](https://en.wikipedia.org/wiki/Ascii85).

Работать должен примерно как стандартная для Unix утилита: `base64`

```
$ ascii85    # читает байты из STDIN и выдаёт ASCII85 в STDOUT
$ ascii85 -e # то же самое
$ ascii85 -d # читает ASCII85 из STDIN и выдаёт байты в STDOUT; при ощбке завершается с кодом, отличным от 0
```

## Как запустить тесты на Python?
1. Установить python3
2. Собрать программу `./build.sh`
3. Запустить тесты `python3 test/test_random.py`

Задачи:

## 1.1. Сам декодер

`STDIN` следует читать, на ваш выбор, одним из способов:

- постепенно, позволяя работать с данными произволной длины, и постепенно же выдавать данные в `STDOUT`;
- до конца, и потом весь результат выдать в `STDOUT`, как делает `base64`;
- добавить опцию в командную строку, которая позволит выбрать, как действовать.

Для этого можно использовать [`basic_istream::read`](https://en.cppreference.com/w/cpp/io/basic_istream/read),
а для побайтовой записи — [`basic_ostream::write`](https://en.cppreference.com/w/cpp/io/basic_ostream/write).

## 1.2. Unit-тесты

- Использовать [GoogleTest](https://github.com/google/googletest) на небольших наборах данных;
- Для имитации «картоного» ввода-вывода можно использовать
  [стандартного вывода](https://en.cppreference.com/w/cpp/io/basic_ios/rdbuf).
  <!-- [gMock](https://google.github.io/googletest/gmock_for_dummies.html). -- нет, лучше не связываться =) -->

## 1.3. Тесты на случайных корректных и некорректных данных

Отдельные тесты, которые проверяют работу готового (де)кодера при помощи [соответствующих функций](https://docs.python.org/3/library/base64.html#base64.a85encode)
модуля `base64` стандартной библиотеки Python.

Некорректные данные должны вызывать завершение декодера (для кодера входные данные все корректные) с кодом, отличным от 0.
