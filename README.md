# Labo-PCAD
Spettacolari esempi di multithreading in C.

Per eseguire: `make all`
Per cambiare le dimensioni delle matrici modificare le costanti `M`, `N`, `P`nel file `matrix.c`.

Lo scopo dell’esercizio è quello di scrivere un programma C con la libreria pthread per
applicare la decomposizione per righe delegando la moltiplicazione blocco per blocco a
diversi thread.
Più precisamente il programma deve effettuare la moltiplicazione C * (A * B) tra matrici di
float dove
Markup : * A è una matrice MxN
* B è una matrice NxP
* C è una matrice PxM
Per sfruttare il multithreading utilizzare la decomposizione per righe per calcolare sia R=A * B
che C * R (il risultato è una matrice PxP).
Utilizzate gli stessi thread nelle due fasi di calcolo applicando una barriera per sincronizzare
le due fasi di calcolo: i thread devono attendere in barriera il completamento della prima
parte di calcolo e dopo la seconda decomposizione completare il calcolo.
