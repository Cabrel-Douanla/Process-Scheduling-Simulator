# Simulateur d'ordonnancement de processus

Ce projet est un simulateur d'ordonnancement de processus écrit en C++ avec l'API Windows 32. Il visualise l'exécution de processus selon trois algorithmes d'ordonnancement : FCFS, SJF (préemptif et non préemptif) et Round Robin.

## Fonctionnalités

* **Algorithmes d'ordonnancement :**
    * **FCFS (First-Come, First-Served) :** Exécute les processus dans l'ordre d'arrivée.
    * **SJF (Shortest Job First) :**
        * **Non préemptif :** Exécute le processus avec le temps d'exécution le plus court parmi ceux qui sont arrivés.
        * **Préemptif :** Exécute le processus avec le temps d'exécution restant le plus court parmi ceux qui sont arrivés. Interrompt le processus en cours si un processus plus court arrive.
    * **Round Robin :** Exécute chaque processus pendant un quantum de temps, puis passe au processus suivant. Le quantum est configurable par l'utilisateur.
* **Interface graphique :** Affiche l'état des processus avec des barres de progression et des étiquettes.
* **Contrôles :** Boutons pour choisir l'algorithme, checkbox pour activer/désactiver la préemption SJF, et champ éditable pour le quantum de Round Robin.


## Versions

Le projet est disponible en 4 versions pour illustrer progressivement les algorithmes :

1. **`src/win32/fcfs.cpp` :** Implémente uniquement l'algorithme FCFS.
2. **`src/win32/fcfs_sjf.cpp` :** Implémente les algorithmes FCFS et SJF (préemptif et non préemptif).
3. **`src/win32/fcfs_sjf_rr.cpp` :** Implémente les algorithmes FCFS, SJF (préemptif et non préemptif) et Round Robin.
3. **`src/gtk/fcfs_sjf_rr.cpp` :** Implémente les algorithmes FCFS, SJF (préemptif et non préemptif) et Round Robin avel la librairie GTK+3.

Les executables des programmes sont disponible dans le repertoire `bin` du projet.

## Compilation et exécution

**Prérequis :**

* Compilateur C++ (g++ recommandé, MinGW ou Cygwin sous Windows).
* Système d'exploitation Windows.

**Compilation :**

Deplacez-vous dans le repertoire `src` et utilisez les commandes suivantes pour compiler les différentes versions :

* **`fcfs.cpp` :**
```bash
g++ fcfs.cpp -o fcfs.exe -lgdi32 -lcomctl32
```
* **`fcfs_sjf.cpp` :**
```bash
g++ fcfs_sjf.cpp -o fcfs_sjf.exe -lgdi32 -lcomctl32
```
* **`fcfs_sjf_rr.cpp` :**
```bash
g++ fcfs_sjf_rr.cpp -o fcfs_sjf_rr.exe -lgdi32 -lcomctl32
```
* **`fcfs_sjf_rr_gtk.cpp` :**
```bash
g++ -o fsfc_sjf_rr_gtk fcfs_sjf_rr_gtk.cpp `pkg-config --cflags --libs gtk+-3.0`
```


**Exécution :**

Après compilation, exécutez le fichier `.exe` correspondant :

```bash
./fcfs.exe  // ou ./fcfs_sjf.exe ou ./fcfs_sjf_rr.exe ou encore ./fcfs_sjf_rr_gtk.exe
```


## Utilisation

L'interface graphique du simulateur comprend :

* **Boutons d'algorithme :** Cliquez sur "Démarrer FCFS", "Démarrer SJF" ou "Démarrer RR" pour choisir l'algorithme.
* **Checkbox "Préemptif" (pour SJF) :** Cochez cette case pour activer la préemption pour l'algorithme SJF.
* **Champ "Quantum" (pour Round Robin) :**  Saisissez la valeur du quantum de temps pour l'algorithme Round Robin.
* **Barres de progression :**  Visualisent la progression de chaque processus.
* **Étiquettes :** Affichent l'état de chaque processus (temps écoulé / temps total).


## Contribution

Les contributions sont les bienvenues !  N'hésitez pas à soumettre des pull requests pour des améliorations, des corrections de bugs ou de nouvelles fonctionnalités.

## Licence

Ce projet est sous licence MIT.  Voir le fichier LICENSE pour plus de détails.


## Auteurs

Cabrel-Douanla (cabrelniamekon@gmail.com)


## Remerciements


