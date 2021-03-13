Чат 3, Вариант 6
Модули:
- cmd.h - модуль для распознания команд
- utils.h - модуль с полезными вещами при реализации сервера: различные структуры
данных и функции

Цели make:
- all(по умолчанию) - сборка всего
- server.out - сборка сервера
- client.out - сборка клиент
- clean - удаление объектных файлов
- distclean - clean + удаление бинарных файлов

Реализованы следующие функции:
(А) - нужны права администратора
Пароль устанавливается при запуске сервера:
- \\users - вывести список пользователей
- \\quit - покинуть сервер
- \\private - отправить приавтное сообщение пользователю
- \\privates - посмотреть кто отправлял вам приватные сообщения
- \\help - помощь
- \\ban - забанить человека(А)
- \\kick - кикнуть человека(А)
- \\nick - сменить имя человеку(А)
- \\shutdown - завершить сервер(А)
- \\admin - получить права администратора

task-7\_1
Автор: Железов Максим, 209 группа
