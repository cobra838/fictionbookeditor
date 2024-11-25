// Поиск некорректных маркеров концевых сносок v.1.3
// Находит ошибки нумерации маркеров концевых сносок
// Автор stokber

function Run() {

var name="Поиск некорректных маркеров концевых сносок";
var version="1.3";
// Вывод кода или текста документа.
var sel=document.getElementById("fbw_body");
var fromHTML = sel.innerHTML;  // код документа
var fromText = sel.innerText; // текст документа
var report = fromHTML;
var count;
var marker;
var re;
var res;
var dlina;
var page;
var result;
var otkrSk;
var zakrSk;
var z;
var t;
var no = "В этом документе не найдено маркеров сносок ";

// окно выбора вида маркеров сносок:
 var dialogWidth="320px";
 var dialogHeight;
  // проверка версии IE: 
var isIE6 = false;
if (navigator.appName === 'Microsoft Internet Explorer') {
  var userAgent = navigator.userAgent;
  if (userAgent.indexOf('MSIE 6.0') !== -1) {
    isIE6 = true;
  }
}
if (isIE6) {
  // alert('Используется Internet Explorer 6');
  dialogHeight="250px";
} else {
  // alert('Используется другая версия Internet Explorer или другой браузер');
  dialogHeight="215px";
}

 var fbwBody=document.getElementById("fbw_body");
 var coll=new Object();
 coll["fbwBody"]=fbwBody;
 coll["mainDocument"]=document;
 coll["window"]=window;
 // coll["versionNum"]=listMarker_versionNum;
 var markSign=window.showModalDialog("HTML/Поиск некорректных маркеров сносок - выбор из вариантов.htm",coll,
     "dialogHeight: "+dialogHeight+"; dialogWidth: "+dialogWidth+"; "+
     "center: Yes; help: No; resizable: Yes; status: No;");
if (!markSign) return;

   report = report.replace(/[☺☻]/g, "?"); // заменяем не знамо откуда вдруг могущие появиться символы ☺ и ☻, которые мы планируем использовать в дальнейшем в качестве временных меток.

/*  // Проверяем случайное наличие в документе символов ☺ и ☻, которые 
// будут использоваться скриптом как временные метки маркеров сносок 
// в  переменной report: 
var Metk = (report.match(/☺|☻/g) || []).length;
if (Metk > 0) {
   window.clipboardData.setData("text","[☻☺]+");
   alert("В вашем документе имеются символ(ы) ☺ и(ли) ☻, которые используются скриптом как временные метки. Для корректной работы рекомендуем на время работы скрипта заменить их на другие символы.\nНайти в документе их можно перейдя в режим Кода и вставив регулярное выражение из буфера обмена в строку поиска окна Найти (Ctrl+F)")
   return true;
} */

// выполнение скрипта в зависимости от выбранных маркеров:
if (markSign == "надстрочным текстом") {
   FMarkSup() ; // приводим маркеры к общему типу.
   Report() ; // составляем постраничный список маркеров и преобразуем текст документа.
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ; // ищем ошибки.
   if (re == undefined) {return true} // если ошибок нет — выходим, или
else //
   PosAndLengthMarkSup() ; // вычисляем позицию и длину совпадения.
   PerehodFinal() ; // переходим и выделяем.
}

else if (markSign == "вида *") {
   FMarkAsteriks() ;
   Report() ;
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ;
   if (re == undefined) {return true}
else
   PosAndLengthMarkAsteriks() ;
   PerehodFinal() ;
}

else if (markSign == "вида [1]") {
   FMarkKvSk() ;
   Report() ;
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ;
   if (re == undefined) {return true}
else
   PosAndLengthMarkKvSk() ;
   PerehodFinal() ;
}

else if (markSign == "вида {1}") {
   FMarkFigSk() ;
   Report() ;
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ;
   if (re == undefined) {return true}
else
   PosAndLengthMarkFigSk() ;
   PerehodFinal() ;
}

else if (markSign == "вида [~1~]") {
   FMarkKvSkTilde() ;
   Report() ;
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ;
   if (re == undefined) {return true}
else
   PosAndLengthMarkKvSkTilde() ;
   PerehodFinal() ;
}

if (markSign == "вида {~1~}") {
   FMarkFigSkTilde() ;
   Report() ;
   if (z == 0 && t ==0) {MsgBox(no+marker+".");return;}
   RegExPostrPostr() ;
   if (re == undefined) {return true}
else
   PosAndLengthMarkFigSkTilde() ;
   PerehodFinal() ;
}

// создаём текст отчета  в зависимости от выбранных меток:
function FMarkSup() {
   marker = "числами надстрочным текстом" // фрагмент текста для сообщения о результатах поиска.
   // report = fromHTML;
   report = report.replace(/<SUP>/ig, "☺"); //подменяем теги SUP на метки
   report = report.replace(/<\/SUP>/ig, "☻"); 
   
   report = report.replace(/[¹²³⁴⁵⁶⁷⁸⁹⁰]+/g, "☺$&☻"); //подменяем символы цифр надстрочным  текстом на цифры.
   report = report.replace(/¹/g, "1"); //
   report = report.replace(/²/g, "2"); //
   report = report.replace(/³/g, "3"); //
   report = report.replace(/⁴/g, "4"); //
   report = report.replace(/⁵/g, "5"); //
   report = report.replace(/⁶/g, "6"); //
   report = report.replace(/⁷/g, "7"); //
   report = report.replace(/⁸/g, "8"); //
   report = report.replace(/⁹/g, "9"); //
   report = report.replace(/⁰/g, "0"); //
   
   // report = report.replace(/<SUP>(?!(\[|\{))/ig, "☺"); //подменяем теги SUP на метки игнорируя метки в виде скобок с числами в верхнем регистре, в том числе уже готовые комментарии. !!!
   // report = report.replace(/([^\]}])<\/SUP>/ig, "$1☻"); !!!
}

function FMarkAsteriks() {
// (звездочек — не более двенадцати в одном маркере)
   marker = "звёздочками";
   // report = fromHTML;
   report = report.replace(/<\/?(STRONG|EM|SUP|STRIKE|CODE)>/ig, ""); 
   report = report.replace(/<SPAN class=code>(.+?)<\/SPAN>/ig, "$1"); // 
   report = report.replace(/&nbsp;(&nbsp;| |\*)/g, " $1"); // 
   report = report.replace(/(&nbsp;| |\*)&nbsp;/g, "$1 "); // 
   report = report.replace(/[ □▫◦]/g, " "); // 
      for (; countAst != 0;) { // заменяем звездочки в пустых строках другими символами (@).
         var ast = new RegExp("^((?:<\\/?[^>]+>)*)([ @]*)([*])([ *]*)((?:<\\/?[^>]+>)*)$","gm");
         var ast_ = "$1$2@$4$5";
         var countAst = 0;
             if (report.search(ast)!=-1)         { report=report.replace(ast, ast_); countAst++}
}

   report = report.replace(/\*+/g, "☺$&☻"); //обрамляем звёздочки метками.
   report = report.replace(/☺[*]{12}☻/g, "☺12☻..........");
   report = report.replace(/☺[*]{11}☻/g, "☺11☻.........");
   report = report.replace(/☺[*]{10}☻/g, "☺10☻........");
   report = report.replace(/☺[*]{9}☻/g, "☺9☻........");
   report = report.replace(/☺[*]{8}☻/g, "☺8☻.......");
   report = report.replace(/☺[*]{7}☻/g, "☺7☻......");
   report = report.replace(/☺[*]{6}☻/g, "☺6☻....."); 
   report = report.replace(/☺[*]{5}☻/g, "☺5☻....");
   report = report.replace(/☺[*]{4}☻/g, "☺4☻...");
   report = report.replace(/☺[*]{3}☻/g, "☺3☻..");
   report = report.replace(/☺[*]{2}☻/g, "☺2☻.");
   report = report.replace(/☺[*]{1}☻/g, "☺1☻");
}

function FMarkKvSk() {
   marker = "в виде скобок [1]";
   report = report.replace(/<\/?(STRONG|EM|SUP|SUB|STRIKE)>/ig, ""); 
   report = report.replace(/<SPAN class=code>(.+?)<\/SPAN>/ig, "$1"); // !!!
   report = report.replace(/(<A class=note [^<]+?>)\[(\d+)\](<\/A>)/ig, "$1($2)$3"); //  !!!
   report = report.replace(/\[/g, "☺");
   report = report.replace(/\]/g, "☻");
   report = report.replace(/(<A class=note [^<]+?>)\((\d+)\)(<\/A>)/ig, "$1[$2]$3"); //  !!!
}

 function FMarkFigSk() {
   marker = "в виде скобок {1}";
   report = report.replace(/<\/?(STRONG|EM|SUP|SUB|STRIKE)>/ig, ""); 
   report = report.replace(/<SPAN class=code>(.+?)<\/SPAN>/ig, "$1"); //  !!!
   report = report.replace(/(<A href=[^<]+?>)\{(\d+)\}(<\/A>)/ig, "$1($2)$3"); //  !!!
   report = report.replace(/\{/g, "☺");
   report = report.replace(/\}/g, "☻");
   report = report.replace(/(<A href=[^<]+?>)\((\d+)\)(<\/A>)/ig, "$1{$2}$3"); //  !!!
   report = report.replace( /☻☺/mg, "☻ ☺") ; // чтобы были видны два маркера знака сносок подряд без пробелов между. 17.09.2022. !?!?!?!?
}

function FMarkKvSkTilde() {
   marker = "в виде скобок [~1~]";
   report = report.replace(/\[~/g, "☺");
   report = report.replace(/\~]/g, "☻");
}

 function FMarkFigSkTilde() {
   marker = "в виде скобок {~1~}";
   report = report.replace(/\{~/g, "☺");
   report = report.replace(/\~}/g, "☻");
}

function Report() {
   txt = report;
   // преобразуем "txt" для более точного позиционирования при наличии в документе 
   // изображений, таблиц и проблемных символов:
   txt = txt.replace(/(<P class=t[dh]>)&nbsp;(<\/P>)/ig, "$1☻☺$2"); // 
   txt = txt.replace(/<P>&nbsp;<\/P>/ig, "<P>☻☺</P>"); // 
   
   txt = txt.replace(/<\/?(STRONG|EM|SUP|SUB|STRIKE|CODE)>/ig, ""); // 
   txt = txt.replace(/<SPAN class=code>(.+?)<\/SPAN>/ig, "$1"); 
   //  нормализуем двойные метки, если вдруг такие имеются:
   txt = txt.replace(/☻☻/ig, "☻"); // 
   txt = txt.replace(/☺☺/ig, "☺"); // 
   
   
   txt = txt.replace(/<P class=td><SPAN(( [^>]+)? class=image( [^>]+)?)>(<IMG( [^>]*)?>)<\/SPAN><\/P>/ig, "IM"); // 
   txt = txt.replace(/<P class=td><SPAN(( [^>]+)? class=image( [^>]+)?)>(<IMG( [^>]*)?>)<\/SPAN>/ig, "IMG"); // 
   txt = txt.replace(/<DIV(( [^>]+)? class=image( [^>]+)?)>(<IMG( [^>]*)?>)<\/DIV>/ig, "IMG"); // 
   txt = txt.replace(/<SPAN(( [^>]+)? class=image( [^>]+)?)>(<IMG( [^>]*)?>)<\/SPAN><\/P>/ig, "IM"); // 
   txt = txt.replace(/<SPAN(( [^>]+)? class=image( [^>]+)?)>(<IMG( [^>]*)?>)<\/SPAN>/ig, "IMG"); // 
   txt = txt.replace(/^((?:IMG)+)\r\n(?=IMG)/igm, "$1"); //
   txt = txt.replace(/<\/?[^<>]+>/g, ""); // удаляем все теги
   txt = txt.replace(/\r\n(\r\n)+/gm, "\r\n"); //
   txt = txt.replace(/☻☺/g, ""); // ??? закомментил 13.09.2023. !?!?!?!?
   
   txt = txt.replace(/[ □▫◦]/g, " "); // 
   txt = txt.replace(/&lt;/g, "<"); // 
   txt = txt.replace(/&gt;/g, ">"); // 
   txt = txt.replace(/&amp;/g, "&"); // 
   txt = txt.replace(/&nbsp;/g, " "); // 
   txt = txt.replace(/&shy;/g, " "); // мягкий перенос.23.11.2024
   txt = txt.replace(/\r\n/g, "\n");  // 
   
   // txt = txt.replace(/☻([ ]*)☺/g, "$1"); // два маркера подряд или только с пробелами между считать за один ошибочный.


// создаём постраничный список маркеров сносок:

   report = report.replace(/&nbsp;/g, " "); // заменяем неразрывные пробелы на обычные.
   
      report = report.replace(/☻([ ]*)☺/g, "$1"); // // два маркера подряд или только с пробелами между считать за один ошибочный.13.09.2023  !?!?!?!?
   
   report = report.replace(/<\/?[^<>]+>/g, ""); // удаляем все теги
   report = report.replace(/(\r\n)(\r\n)+/g, "$1"); // удалить пустые строки.
   
   // заменяем все символы "z" и "t", если такие вдруг оказались между метками сносок.
   var colZT = 1
   for (var i = 0; colZT > 0; i++) { 
      var colZT= (report.match(/(☺.+?)[ztZT](.*?☻)/g) || []).length; // 
      report = report.replace(/(☺.+?)[ztZT](.*?☻)/g, "$1?$2"); //
   }
   
   report = report.replace(/^☺(.+?)☻/mg, "☺t$1☻"); // пометить меткой "t" маркеры текстов сносок.
   report = report.replace(/([^\n])☺(.+?)☻/gm, "$1☺z$2☻"); // пометить меткой "z" маркеры знаков сносок.
   report = report.replace(/^(☺t\d+☻[^\r\n☺☻]+?☺)z(\d+☻)/gm, "$1Z$2"); // пометить меткой "Z" маркер знака сноски в тексте сноски.
   
     report = report.replace(/(☺z\d+☻[ ]*☺)z(\d+☻)/gm, "$1Z$2"); // пометить меткой "Z" два знака сноски подряд или через пробел(ы). ?????? 16.09.2023.
      // report = report.replace(/(☺z\d+☻[ ]*☺)z(\d+☻)/gm, "$1T$2"); // пометить меткой "Z" два знака сноски подряд или через пробел(ы). ?????? 16.09.2023.
   // alert(report);
   report = report.replace(/^[^☺☻]+$/gm, ""); // удалить строки без меток. 
   
   report = report.replace(/^([ ]+)(☺.+?)☻/gm, "$2$1☻"); // начальные пробелы перед маркерами текстов сносок >
// переместить в границы маркеров для лучшей локализации.
   report = report.replace(/^(☺.+?)☻[ ]*$/gm, "$1 ☻"); // маркер сноски в пустой строке сделать видимым.
   report = report.replace(/(^|☻)[^☺☻]+?(☺|$)/g, "$1$2"); // удалить всё кроме меток и их номеров.
   report = report.replace(/[☻☺]/g, ""); // удалить метки тегов бывшего верхнего индекса.
   report = report.replace(/(t.+?)(z.+?)/gm, "$1\r\n$2"); // делим на строки-"страницы".

// alert(report);
// window.clipboardData.setData("text",report);

// подбиваем итоги по списку маркеров:
z = (report.match(/z/g) || []).length; // маркеров знаков сносок всего.
t = (report.match(/t/g) || []).length; // маркеров текстов сносок всего.
page = (report.match(/^.+$/gm) || []).length; // условных "страниц" всего.
// MsgBox("Найдено:\nмаркеров знаков сносок: "+z+" \nтекстов сносок : "+t+" \nусловных страниц: "+page+" ");
marker = marker.toUpperCase();
}

// ищем ошибки нумерации маркеров сносок:
function RegExPostrPostr() {

// 1. Более 12-ти звёздочек.
if (markSign == "вида *") {
   var RX = new RegExp("[zZt][*]{12,}","g");
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Курсор (без выделения) остановился на маркере из более чем двенадцати звездочек. Скрипт не работает с маркерами с таким количеством звездочек. Извините." // #1";
// alert("Найдено: \nПозиция: "+result.index+"\nСовпадение: "+result[0]+"\nДлина: "+result.length);
// результат вывести в статусбар:
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
     }
}

 // 1a. Поиск знака сноски после маркеров текстов сносок: только для концевых сносок
if (result == null) {
   var RX = new RegExp("^z\\d+","gm");
   // var RX = new RegExp("t\\dZ+","");
   RX.lastIndex = 1; // ищем с 1-й позиции
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Маркер знака сноски после текстов сносок, что не характерно для концевых сносок." // #2";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

 // 2. Поиск знака сноски в тексте сноски: // перенёс в первую очередь.
if (result == null) {
   var RX = new RegExp("Z\\d+","");
   // var RX = new RegExp("t\\dZ+",""); // ?????? 24/10/2023
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Маркер знака сноски в тексте сноски. Возможно склеенные абзацы или тексты сносок в подбор." // #2";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

 // 3. Пробел перед маркером текста сноски:
if (result == null) {
   var RX = new RegExp("z\\d+[ ]+","m");
   // var RX = new RegExp("z\\d+T+","m");
   result = RX.exec(report);
   if (result !== null) {
      // var textMsg = "Пробел после маркера знака сноски или перед маркером текста сноски. Возможно маркер знака сноски (с пробелом) в тексте сноски" // #3";
      var textMsg = "Кажется, в границы маркера попал пробел. Или начальный пробел перед маркером текста сноски. Или два маркера подряд с пробелом/пробелами между ними."// #3";
// alert("Найдено: \nПозиция: "+result.index+"\nСовпадение: "+result[0]+"\nДлина: "+result.length);
// результат вывести в статусбар:
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

 // 4. Поиск знака сноски-не числа:
 if (result == null) {
   var RX = new RegExp("z[zt]|z\\d*[^\\dzt\\n\\r]+|z0","m"); // !!!!! 2023.08.20
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Маркер знака сноски — не число. Возможно, в границы маркера попал пробел." // #4";
// alert(Найдено: \nПозиция: "+result.index+"\nСовпадение: "+result[0]+"\nДлина: "+result.length);
// результат вывести в статусбар:
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

// 10. Поиск нарушений порядка номеров маркеров знаков сносок:
if (result == null) {
   var RX = new RegExp("z(\\d+)(?=z(\\d+))","g");
   while ((result = RX.exec(report)) != null) {
     var razniza = (result[2] - result[1]);
     if (razniza != 1) {
         var textMsg = "После выделенного маркера знака сноски "+result[1]+" следует маркер знака сноски "+result[2]+", что нарушает порядок нумерации. Возможно пропущены или неверно пронумерованы маркеры знака сноски ниже выделенного места или склеенные абзацы. Или (невыделенный скриптом) пробел перед маркером текста сноски." // #10";
        window.external.SetStatusBarText(textMsg); 
        result.index += result.length - 1;
        Perehod();
        break
        }
   }
}

 // 5. Поиск текста сноски-не числа:
if (result == null) {
   var RX = new RegExp("t[zZt]|t\\d*[^\\dzt\\r\\n]+|t0","m"); // !!!!! 2023.08.20
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Маркер текста сноски — не число. Возможно, в границы маркера попал пробел. Как вариант — маркер в пустой строке." // #5";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      result.index++
      Perehod();
      }
}

// 11. Поиск нарушений порядка номеров маркеров текстов сносок:
if (result == null) {
   var RX = new RegExp("t(\\d+)(?=t(\\d+))","g");
   while ((result = RX.exec(report)) != null) {
   var razniza = (result[2] - result[1]);
   if (razniza != 1) {
      var textMsg = "После выделенного маркера текста сноски "+result[1]+" следует маркер текста сноски "+result[2]+", что нарушает порядок нумерации. Возможно пропущены или неверно пронумерованы маркеры текста сноски ниже выделенного места." // #11";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length - 1;
      Perehod();
      break
      }
   }
}

// 6. Проверка отсутствия самого первого в документе маркера знака сноски:
if (result == null) {
   var RX = new RegExp("^t","m");
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Отсутствует самый первый в документе маркер знака сноски 1. Ищите выше выделенного места." // #6";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

 // 7. Проверка маркера знака сноски 1:
if (result == null) {
   var RX = new RegExp("^z([^1]|1\\d)","m");
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Некорректный или отсутствует маркер знака сноски 1. Пропущенный маркер ищите выше выделенного места. Если выделен маркер текста сноски, возможно перед ним оказался пробел." // #7";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

 // 8. Проверка маркера текста сноски 1:
if (result == null) {
   var RX = new RegExp("(z\\d+)t([^1]|1\\d)","m");
   result = RX.exec(report);
   if (result !== null) {
     var textMsg = "Некорректный или отсутствующий маркер ТЕКСТА сноски 1. Пропущенный маркер ищите выше или ниже выделенного места. Возможно, он склеился с предыдущим абзацем." // #8";
     window.external.SetStatusBarText(textMsg); 
     result.index += result.length;
     Perehod();
     }
}

 // 9. Проверка маркера текста сноски 1:
if (result == null) {
   var RX = new RegExp("z\\d+$","m");
   result = RX.exec(report);
   if (result !== null) {
      var textMsg = "Маркер знака сноски без соответствующего ему маркера текста сноски. Последний маркер должен быть маркером текста сноски." // #9";
      window.external.SetStatusBarText(textMsg); 
      result.index += result.length;
      Perehod();
      }
}

// 12. Поиск разного количества знаков и текстов сносок на одной странице:
if (result == null)  {
   var countSupDo = 0;
   for (var i = 0; i < page; i++) {
      var str = report.split(/\r\n/mg) ; 
      // alert(str[i]);
      var zNaStr = (str[i].match(/z/g)).length; // маркеров знаков сносок на "странице".
      var tNaStr = (str[i].match(/t/g)).length; // маркеров текстов сносок всего на "странице".
      var mNaStr = (str[i].match(/z|t/g)).length; // маркеров сносок всего на "странице".
      // alert("На странице "+zNaStr+" знаков и "+tNaStr+" текстов сносок.\nВсего "+mNaStr+".");
      countSupDo = (countSupDo + mNaStr); // количество маркеров до следующей "страницы".
      
      // если номеров и текстов не поровну, то:
      var markSnoski;
      if (zNaStr != tNaStr) {
      var mZ = (countSupDo - tNaStr) ; // вычисляем количество верхних индексов до последнего корректного маркера знака сноски.
      var mT = countSupDo ; // вычисляем количество верхних индексов до последнего корректного маркера текста сноски.
      
      if (zNaStr < tNaStr) {
          var countS = confirm("Количество  маркеров знаков и текстов сносок на «странице»* не совпадают ("+zNaStr+" и "+tNaStr+" соответственно).\nПредлагаем перейти на: \n\n• Последний корректный МАРКЕР ЗНАКА сноски.\n(Нажмите кнопку OK).\n\n• Последний корректный МАРКЕР ТЕКСТА сноски.\n(Нажмите кнопку Отмена).\n\nВ большинстве случаев рекомендуется выбирать первый вариант (OK). Если вам не повезет, запустИте скрипт ещё раз и выберите второй вариант (Отмена). В случае реальных постраничных сносок можно поискать некорректный маркер рядом с выделенным местом.\n\n* Под «страницей» скрипт подразумевает: при проверке постраничных сносок — страницу; поглавных — главу.");
          if (countS == true) {
             count = mZ;
             markSnoski = "знака";
             };
             if (countS == false) {
               count = mT - (tNaStr -zNaStr);
               markSnoski = "текста";
               };
			   	  var numNote = ((mZ-zNaStr)/2+zNaStr); // номер будущей сноски.
             }

      if (zNaStr > tNaStr) {
      var countS = confirm("Количество маркеров знаков и текстов сносок на «странице»* не совпадают ("+zNaStr+" и "+tNaStr+" соответственно).\nПредлагаем перейти на: \n\n• Последний корректный МАРКЕР ТЕКСТА сноски.\n(Нажмите кнопку OK).\n\n• Последний корректный МАРКЕР ЗНАКА сноски.\n(Нажмите кнопку Отмена).\n\nВ большинстве случаев рекомендуется выбирать первый вариант (OK). Если вам не повезет, запустИте скрипт ещё раз и выберите второй вариант (Отмена). В случае реальных постраничных сносок можно поискать некорректный маркер рядом с выделенным местом.\n\n* Под «страницей» скрипт подразумевает: при проверке постраничных сносок — страницу; поглавных — главу.");
      if (countS == true) {
      count = mT;
      markSnoski = "текста";
      };
      if (countS == false) {
        count = mZ - (zNaStr - tNaStr);
        markSnoski = "знака";
        };
 var numNote = ((mT-zNaStr-tNaStr)/2+tNaStr); // номер будущей сноски.
      }

      var textMsg = "Количество маркеров знаков и текстов сносок на «странице» не совпадают. Вы перешли на последний выбранный вами корректный маркер " +markSnoski+ " БУДУЩЕЙ сноски № " +numNote+"." // #9";
      window.external.SetStatusBarText(textMsg); 
      // переходим к найденному:
       PerehodNaSlSup();
      break; // закомментил 05.09.2023.
      }
   }
}

if (zNaStr == tNaStr && result == null) {
   alert("Ошибок нумерации маркеров сносок не найдено.\n\n=================\n\nСкрипт '"+name+"' v."+version);
   // alert("Ну не шмогла я…");
   // return ;
   }
}

// функция вычисления кол. маркеров до совпадений, найденных по первым восьми регекспам:
function Perehod() {
// alert(report);
// отсекаем все маркеры после совпадения и находим количество маркеров до совпадения:
   var re03 = new RegExp("^((.|\\r|\\n){"+result.index+"})(.|\\r|\\n)+$", "m" );
   var re03_ = "$1";
   if (report.search(re03)!=-1)         { report=report.replace(re03, re03_); }
   count = (report.match(/z|Z|t/mg)).length; // маркеров // !!!!! 2023.08.20
   // alert("Маркеров до совпадения: " +count);
   
   PerehodNaSlSup();
 }

// функция для определения координат и длины совпадения:
function PerehodNaSlSup() {
   re = /☺(.+?)☻/gm;
   // alert("Маркеров до совпадения: " +count);
   for (i = 0; i < count; i++) {  
     ((res = re.exec(txt)) != null)
      }
   // alert("Найдено: " + res[0] + "\nДлина: "+res.length+"\nПозиция: "+res.index+"\nСледующий поиск c позиции: "+re.lastIndex);
} 

// поправляем координаты и длину совпадения для выбранного вида маркеров сносок:
function PosAndLengthMarkSup() {
   dlina = re.lastIndex - res.index - 2; // позиция.
   res.index--
   res.index = res.index - 2 * (count -1); // длина
}

function PosAndLengthMarkAsteriks() {
   dlina = res[0].replace(/[☻☺]/g, ""); // длина.
   res.index--
   res.index = res.index - 2 * (count -1); // позиция 
}

function PosAndLengthMarkKvSk()  {
   dlina = re.lastIndex - res.index - 2; // длина.
}

function PosAndLengthMarkFigSk()  {
   dlina = re.lastIndex - res.index - 2; // длина.
}

function PosAndLengthMarkKvSkTilde()  {
   dlina = re.lastIndex - res.index - 2; // длина.
   res.index = res.index +2 * (count -1) +1; // позиция.
}

function PosAndLengthMarkFigSkTilde()  {
   dlina = re.lastIndex - res.index - 2; // длина.
   res.index = res.index +2 * (count -1) +1; // позиция.
}

// функция для выделения найденного:
function PerehodFinal() {
// сбрасываем курсор в начало документа:
   var fbwBody=document.getElementById("fbw_body");
    var tr=document.body.createTextRange();
   tr.moveToElementText(fbwBody);
   tr.collapse(true);
   tr.select();
   window.scrollTo(0,0);
   
    // переходим и выделяем найденное совпадение.
   var myRange = document.selection.createRange ();
   var fromText = myRange.text;
   
   myRange.moveStart ("character", res.index); // переместить курсор правее.
   myRange.moveEnd ("character", dlina); // выделить символы правее курсора.
   myRange.select ();
}
}