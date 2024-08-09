// Скрипт «Перейти на предыдущий эпиграф» (epigraph)
// Автор Sclex, сборка TaKir
// Версия 1.2

function Run() {

 var blockElementClass="epigraph"; // класс блочного элемента, который (элемент) нужно искать
 var undoMsg="переход на предыдущий эпиграф";
 var statusBarMsg="Переходим на предыдущий эпиграф…";
 var notFoundMsg="До начала документа не нашлось ни одного эпиграфа.";
 
 try { var nbspChar=window.external.GetNBSP(); var nbspEntity; if (nbspChar.charCodeAt(0)==160) nbspEntity="&nbsp;"; else nbspEntity=nbspChar; }
 catch(e) { var nbspChar=String.fromCharCode(160); var nbspEntity="&nbsp;";};
 var re2=new RegExp(" |&nbsp;|"+nbspChar,"g");
 
 function checkP(elem1) {
  //if (elem1.className=="text-author") return true;
  if (hasAmongParents(elem1,"epigraph")) return true;
  return false;
 }

 function hasAmongParents(elem2,nameOfClass) {
  while (elem2 && elem2.nodeName!="BODY") {
    if (elem2.nodeName=="DIV" && elem2.className==nameOfClass) return true;
    elem2=elem2.parentNode;
  }
  return false;
 }
 
 function getParentWithClass(elem3,nameOfClass) {
  while (elem3 && elem3.nodeName!="BODY") {
    if (elem3.nodeName=="DIV" && elem3.className==nameOfClass) return elem3;
    elem3=elem3.parentNode;
  }
  return null;
 }
 
 var emptyLineRegExp=new RegExp("^( | |&nbsp;|"+nbspChar+")*?$","i");
 
 function isLineEmpty(ptr) {
  return emptyLineRegExp.test(ptr.innerHTML.replace(/<[^>]*?>/gi,""));
 }
    
 function getPreviousNode(el) {
  //alert("Вошли в getPreviousNode.");
  if (el.lastChild && el.nodeName!="P")
   el=el.lastChild;
  else {
   while (el && !el.previousSibling)
    el=el.parentNode;
   if (el && el.previousSibling) el=el.previousSibling; 
  }
  return el;
 }
 
 function getPreviousP(el) {
  var savedEl=el;
  while (el && (el.nodeName!="P" || el==savedEl))
   el=getPreviousNode(el);
  return el;
 }
 
 function scrollIfItNeeds() { 
  var selection = document.selection;
  if (selection) {
    var range = selection.createRange();
    var rect = range.getBoundingClientRect();
    // var correction = (rect.bottom - document.documentElement.clientHeight/2); // центр
   var correction = (rect.bottom - document.documentElement.clientHeight/2); // верх
   // var popravka = (rect.bottom - document.documentElement.clientHeight/8* 6); // низ
   window.scrollBy(0, correction);
  }
 }
 
 function scrollIfItNeeds() { 
  var selection = document.selection;
  if (selection) {
    var range = selection.createRange();
    var rect = range.getBoundingClientRect();
    // var correction = (rect.bottom - document.documentElement.clientHeight/2); // центр
   var correction = (rect.bottom - document.documentElement.clientHeight/2); // верх
   // var popravka = (rect.bottom - document.documentElement.clientHeight/8* 6); // низ
   window.scrollBy(0, correction);
  }
 }
 
 var s;
 var tr,el,prv,pm,savePrevious,saveFirstEmpty,previousPtr;
 var errMsg="Нет выделения.\n\nПеред запуском скрипта нужно выделить абзацы, которые будут обработаны.";
 tr=document.selection.createRange();

 if (tr.parentElement().nodeName=="TEXTAREA" || tr.parentElement().nodeName=="INPUT") {
  MsgBox("Ошибка. Должно быть выделение в тексте книги, а не в поле ввода.");
  return;
 }
 window.external.BeginUndoUnit(document,undoMsg);
 try { window.external.SetStatusBarText(statusBarMsg); }
 catch(e) {}
 var fbwBody=document.getElementById("fbw_body");

 var tr3=document.selection.createRange();
 tr3.collapse(false);
 var ptr=tr3.parentElement();
 var blockElementAtStart=getParentWithClass(ptr,blockElementClass);
 //alert("blockEndNode: "+ptr.outerHTML);
 ptr=getPreviousP(ptr);
 
 while (ptr && fbwBody.contains(ptr)) {
  if (checkP(ptr)&& getParentWithClass(ptr,blockElementClass)!=blockElementAtStart) break;
  ptr=getPreviousP(ptr);
 }
 
 if (ptr && fbwBody.contains(ptr)) {
  var tr1=document.body.createTextRange();
  var parentElem=getParentWithClass(ptr,blockElementClass);
  if (parentElem) {
   tr1.moveToElementText(parentElem);
   if (tr1.moveStart("character",1)==1)
    tr1.moveStart("character",-1);
   tr1.select();
   scrollIfItNeeds();
   var scriptResult="Found";
  }
 }
 else {
  var scriptResult="NotFound";
  MsgBox(notFoundMsg);
 }

 try { window.external.SetStatusBarText("ОК"); }
 catch(e) {} 
 window.external.EndUndoUnit(document);
 return scriptResult;
}