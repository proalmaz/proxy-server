# proxy-server
Нередко в приложении со сложной бизнес-логикой требуется найти баг, который корнями уходит куда-то глубоко в DB-слой. Дело еще больше усложняется, когда в приложении нетривиальные SQL-запросы выражены через ORM или, что еще хуже, когда в системе построения запросов напрочь отсутствует какая-то структура. В такой ситуации на помощь может прийти proxy-сервер, который перехватит все запросы из приложения в БД и отобразит их в удобочитаемом виде. \n
Место proxy-сервера как раз между приложением, запросы которого нужно перехватить, и базой данных MySQL. То есть это обычный TCP-сервер, который слушает на заданном порту входящие соединения, читает из него данные, парсит их и далее пересылает уже в базу данных в точности в том виде, в каком они и пришли.
## Using
```
make
./proxy $(ip-address) $(port DB) $(port proxy-server)
```
