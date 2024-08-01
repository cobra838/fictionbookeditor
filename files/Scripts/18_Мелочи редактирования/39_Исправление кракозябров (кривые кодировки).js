
// Скрипт для исправления крякозябров в выделенном фрагменте текста.
//======================================
 
function Run()
{
 var rng=document.selection.createRange();
 if(rng.compareEndPoints("StartToEnd", rng)==0){ MsgBox("Выделите текст! "); return false;}

 var from = rng.htmlText; //new String
 var kriakoZiabru = "ÀàÁáÂâÃãÄäÅå¨¸ÆæÇçÈèÉéÊêËëÌìÍíÎîÏïÐðÑñÒòÓóÔôÕõÖöØøÙùÚúÛûÜüÝýÞþßÿ×÷";
 var ziabru = new Array('А','а','Б','б','В','в','Г','г','Д','д','Е','е','Ё','ё','Ж','ж','З','з','И','и','Й','й','К','к','Л','л','М','м','Н','н','О','о','П','п','Р','р','С','с','Т','т','У','у','Ф','ф','Х','х','Ц','ц','Ш','ш','Щ','щ','Ъ','ъ','Ы','ы','Ь','ь','Э','э','Ю','ю','Я','я','Ч','ч');
 var to = "";


 for(i=0; i<from.length; i++)
 {
  var character = from.charAt(i,1);
  var pos = kriakoZiabru.indexOf(character);

  if(pos!=-1) to+=ziabru[pos]; else to+=character;
 }

 window.external.BeginUndoUnit(document,"transliteration");

 rng.pasteHTML(to);
 

 window.external.EndUndoUnit(document);
}