# Introducere
In momentul accesarii unei pagini web este lansat un DNS QUERY catre un server DNS,
pentru a obtine IP-ul paginii web ce se doreste a fi accesata, urmand ca ulterior sa se initieze o
conexiune care va aduce toata informatia paginii respective. Cand cererea ajunge la serverul
DNS, acesta va cauta in memorie o intrare de tipul (Name, IP), unde "Name" este adresa paginii
web, iar IP este IP-ul asociat. 
Din cauza faptului ca aceste servere sunt publice fiind accesibile tuturor clientilor, apare
o probabilitate foarte mare ca serverul sa primeasca aceleasi cereri dar de la clienti diferiti. Astfel
pentru a usura operatia de cerere pentru serverele DNS, s-a convenit crearea unor servere de
cache ce vor retine tupluri de forma (url, ip) pe baza frecventei cererilor deoarece daca un
utilizator dintr-un anumit grup a facut o cerere DNS este foarte probabil ca un alt utilizator din
acelasi grup sa faca aceeasi cerere.

# Cerinta
Pentru implementarea backend-ului unui server DNS se poate utiliza o structura de date
ce se pliaza foarte bine pe nevoile de mai sus. Aceasta structura porneste de la metoda LRUCache (Least Recently Used Cache) care retine in memorie doar cele mai recent folosite
inregistrari. Aceasta structura utilizeaza o tabela hash.
Tabela hash este o structura de date optimizata pentru functia de cautare. Acest lucru se
realizeaza transformand cheia intr-o intrare in tabela hash (folosind o functie hash). Tabelele
hash sunt foarte utile in cazul in care se stocheaza cantitati mari de date, a caror dimensiune
(marime a volumului de date) poate fi anticipat.
Functia hash trebuie aleasa astfel incat sa se minimizeze numarul coliziunilor (valori
diferite care produc aceleasi intrari in tabela hash). Coliziunile apar in mod inerent, deoarece
dimensiunea (numarul de intrari) tabelei hash este fixa, iar obiectele de stocare pot avea lungimi
și continut arbitrare. In cazul aparitiei unei coliziuni, valorile se stocheaza pe aceeasi pozitie (in
aceeasi lista). In acest caz, cautarea se va reduce la compararea valorilor efective in cadrul listei
respectiintr-un vector cu M elemente – fiecare element fiind reprezentat printr-o lista dublu
inlantuita circulara generica. Elementele din liste vor fi de forma (Key, Value, Frequency), unde
Key si Value sunt siruri de caractere alocate dinamic, iar Frequency este o valoare intreaga ce
reprezinta numarul de accesari ale cheii Key. Functia hash va calcula restul impartirii sumei
caracterelor ce fac parte din cheia (Key) stocata in tabela hash la numarul maxim de liste ce pot fi
stocate inve.

# Implementare

Structura de date LRUCache va fi reprezentata pe baza unei tabele hash. Aceasta este
formata d tabela hash (M). Fiecare lista va fi sortata descrescator in functie de Frequency, in
cazul elementelor cu aceeasi valoare pentru Frequency, elementele vor fi sortate alfabetic dupa
Key.

Operatiile efectuate in tabela hash sunt:\
•set <Key, Value>\
-daca Key nu exista in tabela hash, aceasta va fi adaugata in lista corespunzatoare din
tabela hash avand Frequency = 0; in cazul in care cheia Key exista in tabela hash,
aceasta nu va mai fi inserata.\
-in cazul in care Key nu exista in tabela hash:\
o daca numarul de elemente din lista corespunzatoare in care trebuie inserat
elemental curent este mai mare sau egal decat M, se va sterge utlimul
element din lista si se va realiza inserarea;\
o daca numarul total de elemente din tabela hash este mai mare decat M * 2 se
va realoca tabela hash la o noua dimensiune de M * 2 intrari. In cazul in care
realocarea nu poate fi realizata, tabela hash va ramane nemodificata.\
• get <Key >

-daca in tabela hash exista elementul cu cheia Key, atunci intoarce valoarea (Value)
corespunzatoare cheii Key, incrementeaza Frequency si reordoneaza in mod
corespunzator lista din care face parte cheia Key cautata; daca cheia Key nu exista in
tabela hash, se va intoarce NULL.\
• remove <Key>\
-sterge elementul (Key, Value, Frequency) din tabela hash (in cazul in care aceasta
exista).\
• print\
-afiseaza toate valorile (Value) din tabela hash;\
-pentru fiecare lista nevida se va afica indicele acesteia si toate elementele acesteia,
sub forma:\
i: (Valoare 1 ) (valoare 2 ) ... (valoare n )\
-elementele listei sunt afisate pe aceeasi linie, separate de un spatiu; indicii listelor
sunt numerotati de la 0.\
• print_list <index >\
-afiseaza valorile (Value) din lista cu indicele index, pe o singura linie; indicii listelor
sunt numerotati de la 0;\
-separarea elementelor afisate se face numai prin spatii;\
-daca lista asociata intrarii index este vida se va afisa VIDA.

# Rulare
Programul va fi rulat:
./tema1 M hash i .in hash i .out
unde:
- M – reprezinta numarul de intrari din tabela hash
- hash i .in – fisierul de date de intrare
- hash i .in – fisierul de date de iesire
- Fisierul de intrare va contine fiecare comanda pe o linie noua
- Tema se va implementa folosind liste dublu inlantuite circulare generice.
