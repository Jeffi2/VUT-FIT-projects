### NAME
* ftrestd - REST api server, zpracovává požadavky zaslané programem ftrest
### SYNOPIS
* ftrestd [-r ROOT-FOLDER] [-p PORT]
### DESCRIPTION
*  ftrestd je souborový server nabízející REST API rozhraní, pomocí kterého umožňuje zpracovávat požadavky od klienta. K dispozici jsou operace nahrání/stažení nebo smazání souboru, vytvoření/smazání složky a získání obsahu adresáře na serveru. Server je schopný obsloužit pouze jednoho klienta připojeného k serveru. Ostatní musí počkat, dokud není předchozí komunikace ukončena.
Následují možné přepínače:
* -r     ROOT-FOLDER specifikuje kořenovou složku serveru (implicitně ./)
* -p     PORT specifikuje číslo portu (implicitně 6677)
### EXIT STATUS
*  1 - špatně zadané argumenty programu
*  2 - problém s připojením soketu 
*  3 - kořenová složka neexistuje
*  Jinak běží server bez přerušení neustále. Při problému s nastavením komunikace vypíše chybové hlášení na standartní chybový výstup.
### IMPLEMENTATION
*  Iterativní server po spuštění zkontoluje zda byly zadány správné parametry, případně nastaví výchozí hodnotu. Potom je nainicializován na příslušný port, na kterém čeká na připojení klienta a následně zaslaný HTTP dotaz. Po přijetí zprávy, pomocí funkce requestTypeHandler() zjistí, zda je HTTP dotaz validní a podporován. Pokud ne, je odeslána odpověď "Bad request.". Funkce následně rozparsuje hlavičku HTTP dotazu a všechny potřebné hodnoty uloží pro další zpracování. Pokud zpráva obsahuje datovou část (Content-Length > 0), celý obsah body je uložen do unsigned char vectoru pro další zpracování (upload souboru). Na základě typu požadované operace jsou volány funkce pro jejich zpracování. Každá funkce následně kontroluje, zda je požadovaná operace přípustná. Po dokončení operací a vyhodnocení (ne)úspěchu, je pomocí funkce createResponse() vygenerována příslušná JSON nebo application/octet-stream odpoveď a pomocí funkce sendMessage(), zaspána zpět klientovi, text odpovědi je obsažen v body. Ostatní pomocné funce jsou podrobně zadokumentovány ve zdrojovém kódu. Nejzajímavější proces upload/download je stejný jako na klientské části. Detailní popis je rozepsán v popisu programu ftrest.
Podporované odpovědi:
* 200 OK
* 400 Bad request - Unknown error.
* 400 Bad request - Already exists.
* 400 Bad request - Not a file.
* 400 Bad request - Not a directory.
* 400 Bad request - Directory not empty.
* 404 Not found - User Account Not Found.
* 404 Not found - File not found.
* 404 Not found - Directory not found.

###NAME	
* ftrest - Client provádí požadované operace na vzdáleném serveru ftrestd
### SYNOPIS
* ftrest COMMAND REMOTE-PATH [LOCAL-PATH]
* COMMAND specifikuje požadovanou operaci. Podporovány jsou operace put(upload file), get(download file), del(delete file), mkd(create directory), rmd(delete directory), lst(list directory content)
* REMOTE-PATH je URI adresa souboru/složky na serveru. Podporovaný tvar: http://server(:port)/user/remote-path(.file or /)
* [LOCAL-PATH] volitelný parametr, specifikuje lokální adresu pro ukládání/nahrávání dat. (výchozí adresa je ./)
### DESCRIPTION
*  ftrest je klient umožňující provádění operací na vzdáleném souborovém serveru ftrestd. Pomocí tohoto programu je možné na server ukládat, případně stahovat nebo mazat soubory a vytvářet adresářovou strukturu.
### EXAMPLE USE
* nahrání souboru na server: ftrest put http://localhost/user/file.name ./file (Při nahrávání souboru musí být na serveru uvedená celá cesta i s názvem souboru, není možné uvést pouze složku. Je tak zajištěna možnost přejmenování souboru na straně serveru. Lokální cesta k souboru může být zadána relativně i absolutně.)
* stažení soubrou ze serveru: ftrest get htttp://localhost:port/user/file.name (./file) Při stahování souboru musí být uvedena celá cesta k souboru na straně serveru. Bez zadání lokálního parametru se soubor bez přejmenování stáhne do aktuální složky. Pro přejmenování souboru použijte volitelný parametr LOCAL-PATH. (./prejmenovany.soubor)
* smazání souboru ze serveru: ftrest del http://localhost/user/file.name 
* vytvoření složky na serveru: ftrest mkd htttp://localhost/user/folder/create/ Server nepodporuje vytváření zanořených struktur složek, předpokládá se, že před voláním příkazu byly vytvořena složka folder. (mkd http://localhost/user/folder) viz. standardní chování mkdir(). Pomocí mkd nelze vytvořit uživatele.
* smazání složky na serveru: ftrest rmd http://localhost/user/folder Složku lze smazat, pouze pokud je prázdná. Pomocí příkazu rmd nelze smazat uživatele.
* vylistování obsahu složky na serveru: ftrest lst http://localhost/user/folder Obsah složky je vypsán na standartní výstup oddělen znakem pro ukončení řádku.
### EXIT STATUS
* 0 - operace skončila úspěchem
* 1 - špatně zadané argumenty programu
* 2 - problém s připojením soketu / k serveru
* 3 - operace put skončila neúspěchem
* 4 - operace get skončila neúspěchem
* 5 - operace del skončila neúspěchem
* 6 - operace lst skončila neúspěchem
* 7 - operace mkd skončila neúspěchem
* 8 - operace rmd skončila neúspěchem
* 9 - zadaná cestla LOCAL-PATH není validní
* 10 - špatná (nevalidní) odpověď od serveru
### IMPLEMENTATION
* Klient po spuštění ověří, že zadané parametry jsou validní. Potom provede pokus o navázání spojení se zadanou adresou serveru. Pokud je spojení úspěšně navázáno, na základě zvolené operace vygeneruje pomocí funkce createRestHeader() odpovídající HTTP dotaz, který odešle na server. Následně čeká na výslednou odpověď od serveru. Tu po přijetí pomocí funkce reciveMessage() příjme. Následně pomocí funkce parseReturnMessage() nastaví všechny potřebné hodnoty z hlavičky odpovědi. Pokud byla operace úspěšná klient se ukončí s návratovým kódem 0. Pokud nebylo možné operaci provést, je na standartní chybový výstup vypsána zpráva ze serveru. Detailní popis procedury upload/download: Za nejzajímavější a zároveň nejsložitější funkci programu považuji možnost stahovat a nahrávat soubory. V průběhu vývoje bylo nutné vzít v úvahu několik možných problémů, proto bych se zde podrobně zaměřil na výsledné řešení těchto funkcí. Celý proces nahrání souboru začíná navázáním spojení a otevřením vstupního souboru pro binární čtení na straně klienta. Po načtení celkové velikosti souboru je vygenerována HTTP hlavička obsahující i ostatní potřebné informace. Následně je odeslána na server, ten ji zpracuje a nastaví očekávanou délku příjimaných dat. Klient přes buffer char[1024] čte binární data ze souboru a odesílá je na server dokud není odeslán celý soubor. Server data postupně příjimá a ukládá do vektoru<unsigned char>, tak aby nedocházelo k poškození jednotlivých bytů. Po ukončení přenosu je soubor držen v paměti a následně je ověřeno, zda došel celý v pořádku a zda je možné jej zapsat na disk. Pokud je soubor poškozen nebo již exituje, je zahozen a odeslána patřičná HTTP odpověď. Toto řešení by nebylo ideální pro realizaci přenosu velkých souborů (řádově GB dat) z důvodu možného zaplnění paměti. Pro přenos takto velkých souboru by bylo vhodné soubor dočastně buffrovat a ukládat na disk již v průběhu příjmání, případně později smazat. Z důvodu dodržení požadavků na REST API a rozsahu projektu, jsem se rozhodl pro postup uchovávání dat in-memory. Download dat ze serveru pak funguje symetricky.
