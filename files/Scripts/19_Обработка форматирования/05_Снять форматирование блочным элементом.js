function Run() {

 var versionNum="1.9";

 function thereIsParentBodyDiv(ptr) {
  while (ptr && ptr.nodeName && ptr.nodeName!="BODY" &&
   !(ptr.nodeName=="DIV" && ptr.className && ptr.className=="body"))
   ptr=ptr.parentNode;
  if (ptr.nodeName=="DIV" && ptr.className && ptr.className=="body")
   return true;
  else
   return false;
 }

 var range,el,el2,saveNextAfterEl,saveNextAfterEl2;
 var elParent,i,j,divs,ps,saveClassName;
 var randomNum=Math.floor((Math.random()*9)).toString()+Math.floor((Math.random()*9)).toString()+
         Math.floor((Math.random()*9)).toString()+Math.floor((Math.random()*9)).toString()+
      Math.floor((Math.random()*9)).toString()+Math.floor((Math.random()*9)).toString();
 var selectionBeginId="removePoemOrCiteBeginId_"+randomNum;
 var selectionEndId="removePoemOrCiteEndId"+randomNum;
 var fbw_body=document.getElementById("fbw_body");
 if (!fbw_body) return;
 if (document.selection.type.toLowerCase()!="none") {
//  alert("При запуске скрипта не должно быть выделения. Должен быть просто курсор.");
  return;
 } 
 var tr=document.selection.createRange();
 window.external.BeginUndoUnit(document,"снятие форматирования блочным элементом (v"+versionNum+")"); 
 tr.pasteHTML("<B id="+selectionBeginId+"></B>");
 el=document.getElementById(selectionBeginId);
 var selectionIsNone=tr.compareEndPoints("StartToEnd",tr)==0;
 if (selectionIsNone && fbw_body.contains(el)) {
  while (!(el.nodeName=="BODY" ||
           (el.nodeName=="DIV" &&
            (el.className=="poem" || el.className=="epigraph" || el.className=="cite" ||
             el.className=="title" || el.className=="annotation" || el.className=="table")
           )
         )) el=el.parentNode;
  if (el.nodeName!="BODY") {
   saveClassName=el.className;
   elParent=el.parentNode;
   if (el.className=="poem") {
    el2=el.firstChild;
    while (el2!=el) {
     saveNextAfterEl2=el2;
     if (saveNextAfterEl2.firstChild && saveNextAfterEl2.nodeName!="P") 
      saveNextAfterEl2=saveNextAfterEl2.firstChild;
     else {
      while (saveNextAfterEl2!=el && saveNextAfterEl2.nextSibling==null)
       saveNextAfterEl2=saveNextAfterEl2.parentNode;
      if (saveNextAfterEl2!=el) saveNextAfterEl2=saveNextAfterEl2.nextSibling;
     } 
     if (el2.nodeName=="DIV" && el2.className=="stanza") {
      if (el2.nextSibling)
        if (el2.nextSibling.nodeName=="DIV" && el2.nextSibling.className=="stanza") {
         el2.insertAdjacentHTML("beforeEnd","<P>&nbsp;</P>");
         InflateIt(el2);
        }
      el2.removeNode(false);
     }
     else if (el2.nodeName=="DIV" && el2.className=="epigraph") {
       el2.removeNode(false);
     }
     else if (el2.nodeName=="P" && el2.className=="text-author" && el2.parentNode==el) {
      el2.removeAttribute("class");
      el2.removeAttribute("className");
     } 
     el2=saveNextAfterEl2;
    }
   } else if (el.className=="cite" || el.className=="epigraph") {
    el2=el.firstChild;
    while (el2!=null) {
     if (el2.nodeName=="P" && (el2.className=="text-author" || el2.className=="subtitle")) {
      el2.removeAttribute("class");
      el2.removeAttribute("className");
     }
     el2=el2.nextSibling;
    }
   } else if (el.className=="title") {
    divs=el.parentNode.getElementsByTagName("DIV");
    for (var i=divs.length-1; i>=0; i--)
      if (divs[i].className && divs[i].className=="epigraph") {
       ps=divs[i].getElementsByTagName("P");
       for (j=0; j<ps.length; j++)
        if (ps[j].className && ps[j].className=="text-author")
         ps[j].removeAttribute("className");
       divs[i].removeNode(false);
      } else if (divs[i].className && divs[i].className=="annotation") {
       divs[i].removeNode(false);
      }
   } else if (el.className=="table") {
    divs=el.getElementsByTagName("DIV");
    for (var i=divs.length-1; i>=0; i--)
     if (divs[i].nodeName=="DIV" && divs[i].className && divs[i].className=="tr")
      divs[i].removeNode(false);
     ps=el.getElementsByTagName("P");
     for (j=0; j<ps.length; j++)
      if (ps[j].className && (ps[j].className=="td" || ps[j].className=="th")) {
       ps[j].removeAttribute("className");
       ps[j].removeAttribute("fbalign");
       ps[j].removeAttribute("fbvalign");
       ps[j].removeAttribute("fbcolspan");
       ps[j].removeAttribute("fbrowspan");
       ps[j].removeAttribute("id");
       ps[j].removeAttribute("fbstyle");
      }
   }
   saveNextAfterEl=el.nextSibling;
   if (el.className!="annotation")
    el.removeNode(false);
   else
     if (thereIsParentBodyDiv(el))
      el.removeNode(false);
   if (saveNextAfterEl==null)
    InflateIt(elParent.lastChild);
   else
    InflateIt(saveNextAfterEl.previousSibling);     
   InflateIt(elParent);
  } 
  document.getElementById(selectionBeginId).removeNode(true);
 }
 else {
  alert("При запуске скрипта не должно быть выделения. Должен быть просто курсор.");
  el.removeNode(false);
 }
 window.external.EndUndoUnit(document); 
}
