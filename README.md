# Détection des intersections des lignes d’un terrain de Football
ENSEIRB-ENSC 3A Option Robotique
Equipe : Florian BRENOT, Grégoire MELIN, Eric NOUMRI


## Contexte du projet:

La Robocup oppose chaque année des équipes de robot footballeur. Une des équipes participantes et championne du monde l’an dernier, Rhoban, a équipé ses robots de caméras embarquées. Les robots doivent être capables de détecter différents types d’éléments sur le terrain, typiquement le but, le ballon ou encore les lignes du terrain.
Le projet a pour but de détecter, à partir de photos issues de la caméra embarquée des robots de l’équipe Rhoban, les intersections formées par les lignes du terrain de football.

Comment détecter les intersections des lignes du terrain de football perçue par le robot ?

## Fonctionnement de l'algorithme

Pour ce projet, on admet les hypothèses suivantes :
La balle, les buts et autres items indépendants des lignes du terrain ont été précédemment détectés : Ils ne seront donc pas considérés par l’algorithme.
Il en va de même pour la partie qui n’est pas le terrain (gradins, personnes qui marcheraient à cotés du terrain  
L’algorithme fonctionne globalement en 4 phases :
La première est une phase de filtrage : Cette phase équivaut à nettoyer l’image (en lui appliquant quelques corrections) puis à la seuiller dans le but de n’avoir en sortie que les structures d'intérêts dans notre cas les lignes du terrain.
La deuxième phase de l’algorithme consiste à détecter les lignes de l’image seuillée précédemment,
La troisième phase permet de détecter les intersections formées par les lignes détectées dans l’étape précédente,
Enfin, la dernière étape a pour but d’identifier la nature des intersections détectées en d’autres termes à estimer si l’intersection est en forme de “L” comme les coins du terrain, en forme de croix (comme dans la partie centrale du milieu de terrain) ou en forme de T comme l’intersections formés par la surface de réparation et la ligne de but.

## Utilisation

L’algorithme utilise une image en entrée, pour afficher une image indiquant les lignes délimitant le terrain de football, ainsi que leurs intersections.

Les lignes délimitant le terrain de football sont indiqués par des traits rouges.
Les emplacements des intersections sont représentés par des cercles entourant le point d’intersection des lignes du terrain. La couleur des cercles indique la “nature de l’intersection” : un cercle violet indique un coin, un cercle bleu foncé indique une intersection en T (une ligne est interrompue par le tracé d’une autre ligne), un cercle vert indique une intersection en X (les deux lignes se prolongent après l’intersection).

La compilation se fait via les commandes suivantes avec la configuration de l’ENSEIRB-MATMECA (fichier généré à partir de la commande qmake-qt4 “CONFIG+=enseirb” intersections.pro) :
make
L’exécution du programme se fait grâce à la commande suivante :
    ./intersections <input_image>
    où <input_image> décrit le chemin vers l’image que l’on veut traiter (voir exemple)

## Exemple

    cd intersections
    make
    ./intersections $pathToData/data/log2/175-rgb.png

où la variables pathToData représente le chemin permettant d’accéder aux données du projet.

## Tests & Conclusion
### Tests
TODO
### Conclusion
Le projet a été très intéressant et nous a permis de mettre concrètement les compétences acquises en cours de Vision par ordinateur. Le problème le plus long à traiter a été de trouver la bonne approche entre une approche géométrique (basée sur la détection de contours par un filtre de Canny) et une approche colorimétrique. Un deuxième problème a été de trouver les bons paramètres par images. En effet, l’essentiel des traitements étant basés sur des seuillages.
Les perspectives d’évolution à court-termes pour le projet serait de mettre en oeuvre un prétraitement en amont de la phase de filtrage afin de pouvoir ajuster automatiquement le seuil précédant la binarisation de l’image.
La deuxième étape serait également d’appliquer l’algorithme à une vidéo, ce qui impliquerait également des problématiques de calibration de la caméra notamment.
