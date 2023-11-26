# Implementare biblioteca stdio

In aceasta biblioteca sunt implementate functii standard de intrare/iesire, I/O buffering, functii de creare procese si rulare executabile. Implementarea este in limbajul C, folosind functii POSIX.

## Structura fisierelor
- util.h: Contine declaratiile pentru functiile de utilitate;
- util.c: Contine implementarea functiilor de utilitate;
- so_stdio.h: Contine declaratiile pentru functiile si structurile din biblioteca;
- so_stdio.c: Contine implementarea functiilor de biblioteca;

## Functionalitati implementate
- SO_FILE *so_fopen(const char *pathname, const char *mode);
    - modul de deschidere este tradus in flag-uri folosite in functia open;
    - se deschide fisierul specificat cu aceste flag-uri;
    - se aloca memorie pentru structura SO_FILE
    - initializam atributele structurii SO_FILE pe care le vom utilza in alte functionalitati precum flag-urile (sunt importante pentru conditiile de read/write), offset-ul din buffer(important pentru I/O buffering), un pointer la inceputul si la finalul buffer-ului, file descriptor-ul (utilizat in operatiile de read/write/open/close) si un pointer la pozitia din fisier, so_feof (pentru a vedea daca am ajuns la finalul fisierului deschis), so_ferror (pentru eventuale erori), dimensiunea bufferului (utilizat pentru cazurile in care nu citim sau scriem dimensiunea maxima a buffer-ului) si pid-ul procesului copil care foloseste capete ale pipe-ului descris de structura SO_FILE (utilizat pentru pclose);
    - sunt implementate tratari de erori pentru deschidere de fisier sau alocari dinamice;
    - se returneaza un pointer la structura SO_FILE in caz de success si NULL in caz de esec;

- int so_fclose(SO_FILE ​*stream);
    - daca buffer-ul nu este gol si ultima operatie a fost de scriere acesta va trebui golit;
    - in aceasta functie se elibereaza resursele utilizate de structura SO_FILE;
    - se trateaza erorile in cazul inchiderii fisierului si a dezalocarii;

- int so_fgetc(SO_FILE *stream);
    - se verifica daca este alocata memorie pentru structura, daca fisierul a ajuns la final (caz in care nu am mai putea citi), daca flag-urile de citire sunt setate (O_RDONLY sau O_RDWR), daca ultima operatie a fost de scriere si buffer-ul nu este golit (in acest caz va trebui sa se goleasca buffer-ul ca mai apoi sa putem citi in el);
    - pentru I/O buffering incercam sa citim dimensiunea maxima a bufferului (cand este gol), iar daca mai avem octeti in buffer pe care ii putem utiliza vom face asta pana cand ajungem la finalul bufferului dupa care il vom reseta;
    - operatia de citire este setata in structura;
    - pentru a controla pozitia in buffer si a returna octetul corect ne folosim de offset;
    - intoarce caracterul ca unsigned char extins la int sau SO_EOF (-1) in caz de eroare;

- int so_fputc(int c, SO_FILE *stream);
    - se verifica daca este alocata memorie pentru structura si daca flag-urile pentru scriere sunt setate (O_WRONLY sau O_RDWR);
    - pentru I/O buffering vom scrie in buffer pana cand este nevoie sau acesta se umple sau este apelata o alta functie prin care este necesara golirea buffer-ului.
    - operatia de scriere este setata in structura;
    - intoarce caracterul scris sau SO_EOF in caz de eroare;

- size_t so_fread(void ​*ptr, size_t size, size_t nmemb, SO_FILE *stream);
    - se incearca citirea a nmemb elemente, fiecare de dimensiune size, insa este returnat numarul de elemente citite (care poate fi egal sau mai mic);
    - in caz de eroare functia intoarce 0;
    - aceasta functie este implementata cu ajutorul unei bucle conditionata de numarul de octeti necesari - in bucla este apelata functia de so_fgetc ce implementeaza I/O buffering, dar se atribuie valori lui ptr octet cu octet;
    - utilizarea so_fgetc a dus la simplitatea implementarii, fara a ne mai face griji de cazurile in care read nu returneaza un nr de octeti la care ne asteptam sau cazurile citirii a n chunk-uri pentru care ar fi trebuit sa tinem cont de pozitia in fisier (chunk-urile nu s-ar fi potrivit exact dimensiunii maxime a buffer-ului si ar fi trebuit sa tinem cont de partea ramasa mai mica decat dimensiunea chunk-ului)

- size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE ​*stream);
    - scrie nmemb elemente, fiecare de dimensiune size;
    - intoarce numarul de elemente scrise, sau 0 in caz de eroare;
    - aceasta functie este implementata cu ajutorul unei bucle conditionata de numarul de octeti care trebuie scrisi - in bucla este apelata functia de so_putc care are implementat I/O buffering.

- int so_fseek(SO_FILE *stream, long offset, int whence);
    - se verifica daca structura este alocata, daca fisierul este deschis si validitatea pozitiei whence
    - in cazul in care ultima operatie a fost de scriere se apeleaza fflush (pentru a evita incercarea de a citi octeti inexistenti in fisier sau a suprascrierii)
    - este folosita functia lseek ce returneaza pozitia din fisier pe care o si setam in structura (utilizata in fuctia de ftell);
    - intoarce 0 in caz de succes si SO_EOF in caz de eroare;

- long so_ftell(SO_FILE *stream);
    - se verifica alocarea resurselor;
    - se apeleaza so_fflush in cazul in care ultima operatie a fost de scriere (pentru a intoarce pozitia reala a fisierului);
    - se returneaza variabila _file_pointer_pos din structura (aceasta este modificata in functiile de citire/scriere/seek), iar in caz de eroare se returneaza -1;

- int so_fflush(SO_FILE *stream);
    - in cazul in care ultima operatie a fost de scriere iar bufferul este plin sau este necesara operatia de read/schimbare a pozitiei in fisier/returnare a pozitiei din fisier se incearca golirea bufferului;
    - se verifica alocarea resurselor;
    - se apeleaza functia de write cat timp bufferul nu este gol;
    - in urma operatiei se va schimba pozitia din fisier, offsetul si continutul bufferului;

- int so_fileno(SO_FILE *stream);
    - intoarce file descriptorul asociat structurii SO_FILE;

- int so_feof(SO_FILE *stream);
    - intoarce o valoare diferita de 0 daca s-a ajuns la sfarsitul fisierului sau 0 in caz contrar;
    - pentru aceasta functie ne vom folosi de variabila so_feof din structura, variabila pe care o setam in urma operatiilor;

- int so_ferror(SO_FILE *stream);
    - intoarce o valoare diferita de 0 daca s-a intalnit vreo eroare in urma unei operatii cu fisierul sau 0 in caz contrar;
    - pentru aceasta functie ne vom folosi de variabila so_ferror din structura, variabila pe care o setam in urma operatiilor (de exemplu nereusirea citirii sau a scrierii);

- SO_FILE *so_popen(const char *command, const char *type);
    - se foloseste un pipe anonim;
    - se lanseaza un proces nou care va executa comanda specificata, iar pentru redirectarea intrarii/iesirii vom folosi capetele pipe-ului (daca este necesara citirea, procesul copil va scrie in pipe si invers) si apoi se va executa sh -c command;
    - in procesul parinti vom aloca o structura SO_FILE, in care vom seta file descriptor-ul cu un capat al pipe-ului (vom citi ce a scris procesul copil in pipe sau vom scrie pentru acesta);

- int so_pclose(SO_FILE *stream);
    - se apeleaza pentru fisierele deschise cu so_popen, se elibereaza resursele cu so_fclose insa retinem variabil de pid pentru a astepta terminarea executiei de catre copil.

## Generarea bibliotecii dinamice
- Pentru generarea bibliotecii se va utiliza fisierul Makefile prin comanda make
- Pentru a sterge toate fisierele binare, se va folosi comanda make clean