# Travelling Salesman Problem Solver (Genetic Algorithm)

Aquest és un programa per optimitzar TSPs. Ha estat desenvolupat mitjançant
la implementació d'un algorisme genètic. El programa s'ha de compilar mitjançant
un compilador de C++ (g++) en sistemes Unix.

### Build

```
g++ tsp.cpp -o tsp
```

### Usage

Per resoldre un mapa TSP cal introduïr
les coordenades del mapa una a una. Per evitar introduïr totes les coordenades
manualment es poden introduir les coordenades en un fitxer i volcar les dades
en el moment d'execució de la següent manera.

```
./tsp < mapa.txt
```

Juntament amb el codi font s'adjunten diferents mapes de prova amb extensió txt
que poden sér emprats a mode de prova. Els fitxers tenen el següent format. La
primera línia conté el nombre de coordenades (nodes) del mapa. Les linies restants
del fitxer contenen els valors dels components X i Y de les coordenades. S'empra
una línia per component.


El programa retorna una solució òptima a mida que en va trobant de millors. Cada
solució és una cadena de dígits, on cada dígit identifica un node del mapa (en
l'ordre seqüencial introduït), juntament amb el fitness, és a dir, la distància
total de la solució.

### Results

Per a donar resultats sobre el meu TSP Solver em basaré en un TSP existent a la web del TSPLIB d'internet. Es tracta d'un TSP que representa 52 llocs de Berlín (Groetschel) situats en les coordenades que es representen en la següent imatge.

![](http://www.lafruitera.com/tsp6.gif)

Mitjançant el TSP Solver proposat s'ha aconseguit una solució amb un fitness de 7680 (distància del recorregut).

![](http://www.lafruitera.com/tsp7.gif)

### License
MIT

