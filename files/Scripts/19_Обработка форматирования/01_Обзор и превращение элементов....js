// Скрипт "Обзор и превращение элементов"
// Автор Sclex
// Сайт скриптов: http://scripts.fictionbook.org

function Run() {
 var elementBrowser_versionNum="3.2";
 var dialogWidth="520px";
 var dialogHeight="540px";
 var fbwBody=document.getElementById("fbw_body");
 var coll=new Object();
 coll["fbwBody"]=fbwBody;
 coll["mainDocument"]=document;
 coll["window"]=window;
 coll["versionNum"]=elementBrowser_versionNum;
 
 // Cписок коротких слов, которые будут делаться маленькими буквами,
 // когда будет запущен пункт меню «Первую букву каждого слова
 // (кроме исключений) сделать большой, остальные – маленькими»
 coll["shortWords"]="им.|без|в|вне|во|для|до|за|из|из-за|из-под|к|ко|на|над|о|об|обо|от|по|под|при|про|с|со|у|а|да|и|или|не|ни|но|б|бы|ж|же|ли";
 
 // Cписок аббревиатур, которые будут делаться большими буквами,
 // когда будет запущен пункт меню «Первую букву каждого слова
 // (кроме исключений) сделать большой, остальные – маленькими»
 coll["abbreviatures"]="АБС|АЗЛК|АЗС|АКБ|АКМ|АНБ|АССР|АСТ|АЭС|БАД|БАМ|ББК|БИК|БМП|БПЛА|БССР|БТР"+
                       "|ВАЗ|ВАСХНИЛ|ВАСХНИЛ|ВВП|ВВС|ВГИК|ВДВ|ВДНХ|ВИЧ" /*+|ВКП(б)*/ +"|ВЛКСМ|ВМС|ВМФ|ВНД"+
                       "|ВОЗ|ВПК|ВУЗ|ВЦИК|ВЦСПС|ГАЗ|ГАИ|ГДР|ГИБДД|ГК|ГОСТ|ГТО|ГУЛАГ|ГЭС|ГЭС|ДВС"+
                       "|ДНК|ДОСААФ|ДЮСШ|ЕГЭ|ЕС|ЕЭС|ЖБИ|ЖЭК|ЗАГС|ЗАО|ЗИЛ|ЗКС|ЗРК|ИБП|ИНН|ИП|КАСКО"+
                       "|КГБ|КНДР|КНР|КоАП|КПД|КПРФ|КПСС|ЛГБТ|ЛДПР|ЛЭП|МАГАТЭ|МАЗ|МБР|МВД|МВФ|МГИМО"+
                       "|МГУ|МДФ|МИД|МИСИ|МИФИ|МКАД|МКС|ММВБ|МТС|МФЦ|МЧС|НАТО|НДФЛ|НИИ|НИОКР|НКВД"+
                       "|НКО|НЭП|ОБСЕ|ОБСЕ|ОБХСС|ОБЭП|ОКАТО|ОКВЭД|ОКД|ОКП|ОКПО|ОМОН|ООН|ООО|ОПГ"+
                       "|ОСАГО|ОТК|ПВО|ПВХ|ПДД|ПНД|ППС|ПТС|ПТУ|ПУЭ|РАМН|РАН|РБК|РВСН|РГБ|РЖД|РККА"+
                       "|РНК|РПГ|РПЦ|РСДРП" /*+|РСДРП(б) */ +"|РУВД|РФ|РЭБ|РЭР|СВ|СВР|СВЧ|СЗ|СИЗ|СИЗО|СКА"+
                       "|СМЕРШ|СМИ|СНГ|СНиП|СНТ|СПбГУ|СС|ССР|СССР|СТС|США|СЭВ|СЭС|ТАСС|ТВД|ТТН"+
                       "|ТТХ|ТЭЦ|УАЗ|УВД|УДК|УЗО|УПК|УССР|ФБР|ФМС|ФРГ|ФСБ|ФСБ|ФСИН|ФССП|ЦК|ЦНС"+
                       "|ЦРУ|ЦСКА|ЧК|ЧМ|ЧП|ЭВМ|ЭКГ|ЮАР|ЮВ|ЮЗ|ЮНЕСКО";
 
 window.showModalDialog("HTML/Обзор и превращение элементов - набор фреймов.html",coll,
   "dialogHeight: "+dialogHeight+"; dialogWidth: "+dialogWidth+"; "+
   "center: Yes; help: No; resizable: Yes; status: No;"); 
}