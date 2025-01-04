//======================================
//             «Исключение копий вложенных файлов» v.1.0
// v.1.0 — Создание скрипта — Александр Ка (20.09.2024)
// Скрипт предназначен для замены имен иллюстраций, которые ссылаются на копии файла, на имя оригинального файла.
//    После этого, копии файла становятся "неиспользованными бинарными объектами", и их можно удалить с помощью скрипта "Удалить неиспользуемые вложения..."
//    Однако перед этим желательно проверить правильность работы скрипта*.
//    Для удобства, файлам, которые определены скриптом как "копия", дано имя оригинального файла с выделяющимся дополнительным индексом.
// * При неблагоприятных условиях, например, если книга содержит 100 откалиброванных иллюстраций под одинаковую высоту и ширину, с размерами от 15 до 25 кБ, вероятность ошибочного "исключения" уникальной иллюстрации резко возрастает примерно до 40%
// ** Копии обложек не обрабатываются
//======================================

function Run() {

 var NumerusVersion="1.0";
  var Ts=new Date().getTime();

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------

                 ///  НАСТРОЙКИ

// ---------------------------------------------------------------

       //   •  Демонстрационный режим
//  Включает присмотр всех возможных строк статистики.
//  Удобно для редактирования строк статистики.

var VseStroki_on_off = 0;      // 0 ; 1 //      ("0" — отключить, "1" — включить)

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОБЩИЕ ПЕРЕМЕННЫЕ


 var fbwBody=document.getElementById("fbw_body");   //  Структура текста (аннотация + история + все <body>, иначе говоря, всё это видно в режиме "B"-дизайн)

 var n=0;
 var k=0;
 var j=0;
 var h=0;  //   Локальные переменные для небольших областей

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ПОИСК  :  Разделы "IMG"


 var mImg=fbwBody.getElementsByTagName("IMG");  //  Массив с узлами "IMG"
 var mImgL=mImg.length;         //  Длина этого массива
 var mImgDS=[];  //  Массив с узлами класса "IMG" (внешний раздел)
 var Img;                   //  Одна из картинок (внешний раздел)
 var mImgHref=[];             //  Массив с именами картинок
 var imgName;                      //  Одно из имен

 for (n=0;  n<mImgL;  n++) {              //  Цикл для всех иллюстраций
         Img=mImg[n].parentNode;                       //  Будет читаться только внешний раздел картинок
         while (Img != fbwBody  &&  Img.className != "image") Img=Img.parentNode;   //  * иногда между внешним и внутренним разделом встречаются посторонние разделы
         mImgDS[n]=Img;                                     // добавление внешнего раздела в массив
         imgName=Img.getAttribute("href").replace(/^#/, "");        //  Получение имени картинки
         mImgHref.push(imgName);           //  Добавление имени иллюстрации в соответствующий список
         }

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ПОИСК  :  Обложки


 var mCovValue=[];  //  Массив с именами всех обложек (и обычных, и на языке оригинала)
 var mCovValueL=0;  //  длина этого массива

 var tiCover=document.getElementById("tiCover");      //  Раздел с обычными обложками
 var mTiCov=tiCover.getElementsByTagName("SELECT");  //  Массив с раздельными блоками для обычных обложек

 for (n=0;  n<mTiCov.length;  n++) {              //  Цикл для всех обычных обложек
         imgName=mTiCov[n].value.replace(/^#/, "");   //  Получение имени без префикса "#"
         if (imgName != "")  mCovValue.push(imgName);          //  Добавление имени в список имен обложек
         }

 var stiCover=document.getElementById("stiCover");      //  Раздел с обложками на языке оригинала
 var mStiCov=stiCover.getElementsByTagName("SELECT");  //  Массив с раздельными блоками для обложек на языке оригинала

 for (n=0;  n<mStiCov.length;  n++) {
         imgName=mStiCov[n].value.replace(/^#/, "");
         if (imgName != "")  mCovValue.push(imgName);
         }

 mCovValueL=mCovValue.length;

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ПОИСК  :  Вложенные файлы (бинарные объекты)


 var binobj=document.getElementById("binobj");      //  Раздел с информацией о вложенных файлах
 var mBin=binobj.getElementsByTagName("DIV");  //  Массив с отдельными блоками для файлов
 var mBinL=mBin.length;         //  Длина этого массива

 var mBinName=[];  //  Массив с именами файлов
 var mBinSize=[];  //  Массив с размерами всех файлов
 var mBinDims=[];  //  Массив с габаритами всех изображений

         //  Получение записей о бинарных файлах:  Имя;  Тип;  Размер (в байтах);  Размер  (габариты изображения)
 for (n=0;  n<mBinL;  n++) {
         mBinName[n]=mBin[n].all.id.value;
         mBinSize[n]=mBin[n].all.size.value;
         if (mBin[n].all.dims)  mBinDims[n]=mBin[n].all.dims.value;
             else  mBinDims[n]="х";          //  Для невалидных файлов нет записей о размере изображения
         }

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОБРАБОТКА  :  Поиск копий файлов ; переименования

window.external.BeginUndoUnit(document,"«Исключение копий вложенных файлов» v."+NumerusVersion);           // начало записи в систему отмен

 var mNumRepit=[];  //  Массив с номерами повторных файлов
 var count_reName=0;  //  Счетчик измененных имен иллюстраций
 var idx;
 var idx2;
 var reName_;
 var found;
 var newName;       // новое имя для копии файла

aaa:
 for (n=0;  n<mBinL;  n++) {
         for (k=0; k<mNumRepit.length; k++)
                 if (n==mNumRepit[k])  continue aaa;   // пропуск уже найденных копий
         for (j=0; j<mCovValueL; j++)
                 if (mBinName[n]==mCovValue[j])  continue aaa;  //  исключение обложек
         idx = 1;
ddd:
         for (k=n+1;  k<mBinL;  k++)  {                   //  * вложенный цикл уменьшен, чтобы файл не сравнивался сам с собой
                 for (j=0; j<mCovValueL; j++)
                         if (mBinName[k]==mCovValue[j])  continue ddd;           //  исключение обложек
                 if (mBinDims[n] != "х"  &&  mBinDims[n] == mBinDims[k]  &&  mBinSize[n] == mBinSize[k]) {      // проверка на копию
                         mNumRepit.push(k);             //  сохранение найденного номера
                         reName_=false;
                         for (j=0; j<mImgL; j++)              // поиск имени копии в илллюстрациях
                                 if (mBinName[k]==mImgHref[j]) {               //  при нахождении...
                                         mImgDS[j].setAttribute("href","#"+mBinName[n]);           //  замена имени копии на имя оригинала (во внешнем разделе)
                                         mImg[j].src = "fbw-internal:#"+mBinName[n];     //  замена имени копии на имя оригинала (во внутреннем разделе)
                                         mImg[j].outerHTML=mImg[j].outerHTML;         //  для обновления имени в дереве структуры FBE
                                         count_reName++;
                                         reName_=true;
                                         }
                         // далее изменение имени для вложенного файла (лучше это заменить на удаление, но удалять нельзя -- поиск копий неточный)
                         if (reName_) {               //  Если имена иллюстраций изменились
                                 found = true;
                                 while (found) {           //  подборка уникального индекса для суффикса в новом имени файла
                                         found = false;
                                         newName = mBinName[n].replace(/(.)(\.[^.]+)?$/, "$1_COPY_"+("0"+idx).replace(/0?(.{2,})/, "$1")+"$2");  // пробное добавление суффикса
                                         for (h = 0; h < mBinL; h++)                                 // проверка уникальности нового имени
                                                 if (mBinName[h] == newName)  { found = true;  idx++;  break; }
                                         }
                                 idx2 = -1;
                                 for(h=0; h < ImagesInfo.length; h++)            // поиск нужного элемента в "ImagesInfo"
                                         if(ImagesInfo[h].id == mBinName[k])  { idx2 = h;  break; }
                                 if (idx2 != -1) {                            //  если найден...
                                         ImagesInfo[idx2].src = "fbw-internal:#" + newName;   //  изменение имени в двух местах "ImagesInfo"
                                         ImagesInfo[idx2].id = newName;
                                         }
                                 mBin[k].all.id.value = newName;    //  изменение имени в трех местах раздела "binobj"
                                 mBin[k].all.id.setAttribute("oldId",newName);
                                 mBin[k].all.save.setAttribute("onclick","SaveImage(\'fbw-internal:#"+newName+"\');");
                                 mBin[k].all.save.outerHTML=mBin[k].all.save.outerHTML;     // для обновления кнопок сохранения иллюстрации (в "описании документа")
                                 mBinName[k] = newName;        //  изменение имени в рабочем массиве
                                 }
                         }
                 }
         }

window.external.EndUndoUnit(document);    // конец записи в систему отмен

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Подсчет чистого компьютерного времени, потраченного на обработку текста


 var Tf=new Date().getTime();    //  фиксация времени окончания работы скрипта
  var tempus=0;

 var T2=Tf-Ts;                //  время работы скрипта  (в миллисекундах)
 var Tmin  = Math.floor((T2)/60000);
 var TsecD = ((T2)%60000)/1000;
 var Tsec = Math.floor(TsecD);

 if (Tmin ==0)
         tempus = (TsecD+"").replace(/(.{1,5}).*/g, "$1").replace(".", ",")+" сек";
     else {
             tempus = Tmin+" мин";
             if (Tsec !=0)
                     tempus += " " + Tsec+ " с" }

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Текущее время и дата


 var currentFullDate = new Date();

 var currentHours = currentFullDate.getHours();
 var currentMinutes = currentFullDate.getMinutes();
 var currentSeconds = currentFullDate.getSeconds();

 if (currentMinutes<10) currentMinutes = "0" + currentMinutes;
 if (currentSeconds<10) currentSeconds = "0" + currentSeconds;

 var currentDay = currentFullDate.getDate();
 var currentMonth = 1+currentFullDate.getMonth();
 var currentYear = currentFullDate.getFullYear();

 var Day_ = "";
 if (currentDay<10) Day_ = "  ";
 if (currentMonth<10) currentMonth = "0" + currentMonth;
currentYear = (currentYear+"").replace(/^.*?(\d{1,2})$/g, "$1");

 var currentTime = currentHours + ":" + currentMinutes + ":" + currentSeconds;
 var currentDate = currentDay + "." + currentMonth + "." + currentYear;

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Демонстрационный режим "Показать все строки"


 var d=0;
 if (VseStroki_on_off == 1)  d="показать нули";

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Сборка массива с результатами обработки


 var mSt=[];       //  Массив со всеми строками статистики
 var ind=0;        //  Номер строки


                                                 mSt[ind]="• Время выполнения  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .	"+tempus;  ind++;
                                                 mSt[ind]="";  ind++;
                                                 mSt[ind]="Всего вложенных файлов .  .  .  .  .  .  .  .  .  .  .  .  .	"+mBinL;  ind++;
 if (mCovValueL!=d)      { mSt[ind]="Всего обложек  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .	"+mCovValueL;  ind++; }
 if (mImgL!=d)                 { mSt[ind]="Всего иллюстраций .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .	"+mImgL;  ind++; }
                                                 mSt[ind]="";  ind++;
 if (count_reName!=d)  { mSt[ind]="• Изменено имен иллюстраций* .  .  .  .  .  .  .  .  .  .	"+count_reName;  ind++;
                                                 mSt[ind]="________";  ind++;
         //  Примечание (разделение на строки)
         var primechanie="* В скрипте используется неточный поиск копий. Сравнивается размер в байтах, а также высота и ширина изображения. Поэтому есть небольшая вероятность идентификации уникальной иллюстрации как копии. И поэтому, перед окончательным удалением повторных файлов, рекомендуется \"Сохранить вложения на накопитель\" и на всякий случай проверить результат обработки.";
         var reF02 = new RegExp(" ","g");
         var reF02_ = "  ";
         var reZit = new RegExp("^((  (?!—)|.){0,50})(\\\s\\\s|$)(.{0,})$","g");   // Разделение на строки (50 - длина строки)
         var reZit_1 = "$1";
         var reZit_2 = "$4";
         var fragment = "";
         var otstup = "    ";
         fragment=primechanie.replace(reF02, reF02_);
         while (fragment != otstup) {
                 mSt[ind]=fragment.replace(reZit, reZit_1);
                 fragment=otstup+fragment.replace(reZit, reZit_2);
                 ind++ }     }
     else                                  { mSt[ind]="";  ind++;
                                                 mSt[ind]="                    Изменения имён иллюстраций";  ind++;
                                                 mSt[ind]="                              не производились";  ind++; }

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Сборка таблицы результатов


 var stT="";  //  Вывод результатов

 for (n=0; n<ind; n++)
         stT += "\n      "+mSt[n];

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



                 /// ОКНО РЕЗУЛЬТАТОВ  :  Вывод окна результатов на экран


        //  Вывод окна результатов
 MsgBox ("                                            –= ◊ =–\n"+
                   "    .:::: «Исключение копий вложенных файлов» v."+NumerusVersion+" ::::.                      \n"+
                   "̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍ ̍"+
                   stT+"\n\n"+
                   Day_+"           .::::::::::: "+currentDate+" :::::::::::: "+currentTime+" :::::::::::.");

// ---------------------------------------------------------------
// ----------------------------------------------
// -----------------------------



}







