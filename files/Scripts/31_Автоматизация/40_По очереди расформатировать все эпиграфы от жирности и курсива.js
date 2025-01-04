// Скрипт "По очереди расформатировать все эпиграфы от жирности и курсива" (пошаговый вариант)
// Версия 1.5
// Автор Sclex

   // =====================================================================================

   // Инструкция к скрипту:

   // Скрипт "Запуск скриптов по списку" может работать в 2 режимах:

   // 1) Пошаговый режим выполнения указанных действий.
   // 2) Выполнение указанных действий сразу во всем документе (цикл с условием).

   // 1) Для пошаговой работы скрипта надо указать 1 или более строк, начинающихся с runScriptInFolder
   // Пошаговая версия более удобна для постепенной контролируемой обработки
   // какого-то одного из элементов от места установки курсора вниз или вверх по тексту документа.

   // Например:
   // runScriptInFolder("16_Поиск форматирования\\50_Перейти на следующий эпиграф.js");
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\55_Расформатировать абзац(ы) от жирности.js");


   // ======================

   // 2) Для выполнения указанных действий сразу во всем документе (цикл с условием),
   // надо указать 2 или более строк, первая из которых должна начинаться с while (runScriptInFolder

   // Например:

   // while (runScriptInFolder("31_Автоматизация\\50_Перейти на следующий эпиграф.js")=="Found") {
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\46_Сформатировать абзац(ы) программным кодом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\43_Сформатировать абзац(ы) зачеркнутостью.js");
   // runScriptInFolder("\\19_Обработка форматирования\\45_Сформатировать абзац(ы) нижним индексом.js");
   // }

   // while означает цикл.

   // Условие
   // (runScriptInFolder("16_Поиск форматирования\\50_Перейти на следующий эпиграф.js")=="Found")
   // означает "выполнять цикл, пока что-то оказывается найденным скриптом "50_Перейти на следующий эпиграф.js".

   // Строки
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\46_Сформатировать абзац(ы) программным кодом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\43_Сформатировать абзац(ы) зачеркнутостью.js");
   // runScriptInFolder("\\19_Обработка форматирования\\45_Сформатировать абзац(ы) нижним индексом.js");

   // То есть, скрипт с таким кодом сформатирует курсивом, потом программным кодом, зачеркнутостью и нижним индексом
   // все найденные эпиграфы (от позиции курсора до конца документа).

   // Для выполнения более, чем 1 скрипта после строки, содержащей while, обязательно наличие фигурных скобок:
   // после =="Found") { 
   // и
   // } после последней строки с последним по очереди скриптом.


   // Если надо произвести ОДИНАКОВЫЕ действия (например, назначить курсив и снять жирность)
   // с РАЗНЫМИ элементами (например с цитатами и эпиграфами), или
   // Если надо произвести РАЗНЫЕ действия (например назначить жирность и снять курсив)
   // с РАЗНЫМИ элементами (например с цитатами и эпиграфами):

   // То для каждой обработки нужно прописать отдельный блок с while - у каждого блока должно быть свое условие и свои выполняемые команды.

   // Также перед каждым таким блоком нужно разместить строку со скриптом, устанавливающим курсор
   // в начало документа перед выполнением каждого из блоков.

   // ======================
   // Пример выполнения нескольких действий с разными выбранными элементами во всем документе:

   // Устанавливаем курсор в начало документа соответствующим скриптом:
   // runScriptInFolder("\\31_Автоматизация\\001_Установить курсор в начало документа.js");

   // Указываем блок действий № 1:
   // while (runScriptInFolder("16_Поиск форматирования\\52_Перейти на следующую цитату.js")=="Found") {
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\44_Сформатировать абзац(ы) верхним индексом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\05_Снять форматирование блочным элементом.js");
   // }

   // Устанавливаем курсор в начало документа соответствующим скриптом:
   // runScriptInFolder("\\31_Автоматизация\\001_Установить курсор в начало документа.js");

   // Указываем блок действий № 2:
   // while (runScriptInFolder("16_Поиск форматирования\\48_Перейти на следующий заголовок.js")=="Found") {
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");
   // runScriptInFolder("\\19_Обработка форматирования\\40_Сформатировать абзац(ы) жирностью.js");
   // }

   // Устанавливаем курсор в начало документа соответствующим скриптом:
   // runScriptInFolder("\\31_Автоматизация\\001_Установить курсор в начало документа.js");

   // Указываем блок действий № 3:
   // while (runScriptInFolder("16_Поиск форматирования\\24_Перейти на следующий абзац, содержащий код.js")=="Found") {
   // runScriptInFolder("\\19_Обработка форматирования\\43_Сформатировать абзац(ы) зачеркнутостью.js");
   // runScriptInFolder("\\19_Обработка форматирования\\40_Сформатировать абзац(ы) жирностью.js");
   // }

   // ...
   // Теоретически, кол-во блоков выполняемых действий быть любым.

   // Конец описания работы скрипта
   // =====================================================================================



function Run() {
 window.RunSubscripts=function() {
   // Команда, автоматически задающая папку скриптов:
   scriptsFolder(document.getElementById("userCmd").getAttribute("src").replace("file:///", "").replace(/\\[^\\]+?\\[^\\]+?$/, ""));
   // Команда, позволяющая вручную задать папку скриптов (закомментируйте строку выше
   // и раскомментируйте строку ниже):
   // scriptsFolder("c:\\program files\\fbe 2.6.7\\scripts");
   
   // Команда, запускающая скрипт из заданной папки скриптов
   // (можно сделать несколько таких строк):

   // while (runScriptInFolder("16_Поиск форматирования\\52_Перейти на следующую цитату.js")=="Found")
   // runScriptInFolder("\\19_Обработка форматирования\\41_Сформатировать абзац(ы) курсивом.js");


   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!======================!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
   // Тут сами пользовательские команды поиска и обработки:

     runScriptInFolder("\\16_Поиск форматирования\\50_Перейти на следующий эпиграф.js"); {
     runScriptInFolder("\\19_Обработка форматирования\\55_Расформатировать абзац(ы) от жирности.js");
     runScriptInFolder("\\19_Обработка форматирования\\56_Расформатировать абзац(ы) от курсива.js");
    }

   // Конец пользовательских строк с командами

   // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!======================!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  
   
   setTimeout("window.RunSubscripts=undefined; window.scriptsFolderVar=undefined;",0);
   
   function scriptsFolder(path) {
     window.scriptsFolderVar=path;
   }
   
   function runScriptInFolder(path) {
     var fullPath=window.scriptsFolderVar;
     if (!fullPath) {
       MsgBox("В скрипте для запуска скриптов вы используете команду runScriptInFolder, но вы не задали путь к папке скриптов командой scriptsFolder.");
       return;
     }
     if (fullPath.substr(fullPath.length-1,1)!="\\" && path.substr(0,1)!="\\")
       fullPath+="\\";
     fullPath+=path;
     return myRunCmd(fullPath);
   }
   
   function runScript(path) {
     return myRunCmd(path);
   }
   
   function myRunCmd(path) {
    window.msgText="Скрипт\n\n"+path+"\n\nне был запущен, т.к. не удалось его успешно загрузить.\n\n"+
                   "Возможная причина – файл скрипта отсутствует по тому пути, где он должен находиться.";
    window.Run=function() {
     if (window.msgText) alert(window.msgText);
    };
    var script=document.getElementById("userCmd");
    if(!script)	return;
    script.src="file:///"+path;
    var runResult=Run();
    window.Run=function() {
     if (window.msgText) alert(window.msgText);
    }
    return runResult;
   }
 }
 setTimeout("RunSubscripts()",0);
}
