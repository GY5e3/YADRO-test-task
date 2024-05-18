**Сборка проекта:**  
```
$ make task
```
**Запуск программы:**
```
./task <название_файла>
```
**Для юнит-тестов:**  
```
$ make test
```
**Запуск тестирования:**
```
./test
```
### Примеры работы:  
**Тестовые данные:**
<br>3</br>
<br>09:00 19:00</br>
<br>10</br>
<br>08:48 1 client1</br> 
<br>09:41 1 client1</br> 
<br>09:48 1 client2</br> 
<br>09:52 3 client1</br> 
<br>09:54 2 client1 1</br> 
<br>10:25 2 client2 2</br> 
<br>10:58 1 client3</br> 
<br>10:59 2 client3 3</br> 
<br>11:30 1 client4</br> 
<br>11:35 2 client4 2</br> 
<br>11:45 3 client4</br> 
<br>12:33 4 client1</br> 
<br>12:43 4 client2</br>   
<br>15:52 4 client4</br> 
**Результат работы программы:**
<br>09:00</br> 
<br>08:48 1 client1</br> 
<br>08:48 13 NotOpenYet</br> 
<br>09:41 1 client1</br> 
<br>09:48 1 client2</br> 
<br>09:52 3 client1</br> 
<br>09:52 13 ICanWaitNoLonger!</br> 
<br>09:54 2 client1 1</br> 
<br>10:25 2 client2 2</br> 
<br>10:58 1 client3</br> 
<br>10:59 2 client3 3</br> 
<br>11:30 1 client4</br> 
<br>11:35 2 client4 2</br> 
<br>11:35 13 PlaceIsBusy</br> 
<br>11:45 3 client4</br> 
<br>12:33 4 client1</br> 
<br>12:33 12 client4 1</br> 
<br>12:43 4 client2</br> 
<br>15:52 4 client4</br> 
<br>19:00 11 client3</br> 
<br>19:00</br> 
<br>1 70 05:58</br> 
<br>2 30 02:18</br> 
<br>3 90 08:01</br>  
