``` ----------------------------------------------------------------------------
Описание:

Программа представляет сетевую версию игры "Крестики - Нолики" с компьютером.
За стратегию компьютера выбрано построение дерева ходов и оценка узлов методом
минимакс (http://ru.wikipedia.org/wiki/Минимакс).

Приложение состоит из сервера и клиента. Сервер может играть с несколькими
игроками одновременно.

Дерево строится единожды, во время старта серверного приложения.

// ----------------------------------------------------------------------------
Протокол передачи данных:

start:x: - запрос на игру за X (start:o:)
goto:0: - запрос на игровой ход в ячейку по номером 0 в таблице:
|0 1 2|
|3 4 5|
|6 7 8|                                         

-1:0:0:-1 - ответ на ошибочное действие (ход в занятую клетку, не начата игра)
0:0:0:4: - ответный ход на ход клиента
0:0:0:-1: - ответ на старт игры за X	  
0:1:1:4 - ответ сервера о том, что последний ход крестиков победный (отключение)

Пояснение протокола:
Все ответы строятся по структуре:
is_error:is_end_of_game:who_win:last_move:

Где "who_win" равен 1 - выйграл X
		    0 - ничья/ситуация не ясна
		    -1 - выйграл O
А "last_move" равен 0 - 8, цифре соотвествующей ходу

``` ----------------------------------------------------------------------------
