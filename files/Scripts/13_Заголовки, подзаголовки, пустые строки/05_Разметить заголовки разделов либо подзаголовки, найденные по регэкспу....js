//Скрипт «Разметить заголовки разделов либо подзаголовки, найденные по регэкспу»
//Автор Sclex
//http://www.fictionbook.org/forum/viewtopic.php?t=4412

var markupTitlesOrSubtitles_versionNum="4.1";

function Run() {
 var dialogWidth="700px";
 var dialogHeight="335px";
 
 // Нижеследующая команда задает список необычных пробелов,
 // которые в заголовках и подзаголовках должны обрабатываться
 // наравне с обычными пробелами:
 var unusualSpaces=String.fromCharCode(8194)+
  String.fromCharCode(8195)+
  String.fromCharCode(8196)+
  String.fromCharCode(8197)+
  String.fromCharCode(8198)+
  String.fromCharCode(8239)+
  String.fromCharCode(8201)+
  String.fromCharCode(8202);
  
 var fbwBody=document.getElementById("fbw_body");
 var coll=new Object();
 coll["fbwBody"]=fbwBody;
 coll["mainDocument"]=document;
 coll["window"]=window;
 coll["unusualSpaces"]=unusualSpaces;
 coll["versionNum"]=markupTitlesOrSubtitles_versionNum;
 var modes=window.showModelessDialog("HTML/Разметить заголовки разделов либо подзаголовки, найденные по регэкспу - задание параметров.htm",coll,
     "dialogHeight: "+dialogHeight+"; dialogWidth: "+dialogWidth+"; "+
     "center: Yes; help: No; resizable: Yes; status: No;");
}
