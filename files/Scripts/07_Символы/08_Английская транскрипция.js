//Таблица символов английской транскрипции
//                                jurgennt™, май 2008 г.
//                          script engene by Sclex v.1.2
//                          таблица: stokber, май 2024

function Run() {
 var dialogWidth="280px"; //ширина окна таблицы символов
 var dialogHeight="170px"; //высота окна таблицы символов
 var dialogLeft="5px";                                   //координата X
 var params=new Object(); 
 params["fbw_body"]=document.getElementById("fbw_body");
 params["document"]=document;
 params["window"]=window;
 var rslt=window.showModelessDialog("HTML/Английская транскрипция.html",params,
      "dialogHeight: "+dialogHeight+"; dialogWidth: "+dialogWidth+"; dialogLeft: "+dialogLeft+"; "+
      "center: Yes; help: No; resizable: Yes; status: No; scroll: No;"); 
}