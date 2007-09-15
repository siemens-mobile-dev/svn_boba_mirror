Установка BalletMini.

1. Копируете BalletMini.elf в папку 4:\ZBin\BalletMini\
2. Создаёте папки 4:\ZBin\BalletMini\OmsCache и 4:\ZBin\BalletMini\UrlCache
3. Запускаете OperaMini, идёте в Меню->Настройки->Параметры->Код автор.
4. Создаёте файл AuthCode, в который копируете всё что в опере до первой точки
в первую строку, а всё, что от первой до второй - во вторую. Сами точки копировать
не нужно.
5. Кладёте картинки OMS_SMALL.png, OMS_BIG.png, URL_SMALL.png и URL_BIG.png в 
папку 4:\ZBin\img\
6. Прописываете в 4:\ZBin\etc\extension.cfg следующие строчки:
[oms]
RUN=4:\ZBin\utilities\BalletMini.elf
SMALL=4:\ZBin\img\OMS_SMALL.png
BIG=4:\ZBin\img\OMS_BIG.png

[url]
RUN=4:\ZBin\utilities\BalletMini.elf
SMALL=4:\ZBin\img\URL_SMALL.png
BIG=4:\ZBin\img\URL_BIG.png

7. Перезагружаете трубу.
8. Тыкаете по файлу с расширением url, в котором написана ссылка, и пользуете.
9. Если что-то не получилось - виновата кривизна ваших рук и ничего больше, вопросы
на эту тему не принимаются.