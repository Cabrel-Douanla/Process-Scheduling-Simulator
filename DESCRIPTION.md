Ce programme est un simulateur de planification de processus qui visualise l'exécution de processus selon trois algorithmes d'ordonnancement :  **FCFS (First-Come, First-Served), SJF (Shortest Job First) (préemptif et non préemptif), et Round Robin**. Il utilise l'API Windows 32 pour créer une interface graphique avec des barres de progression et des étiquettes pour afficher l'état de chaque processus.

**Structure du programme :**

Le programme est structuré autour de plusieurs fonctions clés :

* **`WinMain` :**  Point d'entrée principal du programme. Initialise l'application Windows, crée la fenêtre principale et lance la boucle de messages.
* **`WindowProc` :**  Fonction de rappel de la fenêtre. Gère les messages Windows, tels que la création de la fenêtre, les clics sur les boutons, le timer et la fermeture de la fenêtre.
* **`GestionTimer` :**  Fonction appelée par le timer à intervalles réguliers.  C'est le cœur du simulateur, où la logique de chaque algorithme d'ordonnancement est implémentée.
* **`DemarrerSimulation` :**  Fonction appelée lorsqu'un bouton d'algorithme est cliqué. Initialise les variables de simulation, trie les processus (si nécessaire) et démarre le timer.
* **`CreerBarreProgression` :**  Crée une barre de progression pour un processus donné.
* **`CreerLabel` :** Crée une étiquette pour afficher l'état d'un processus.
* **`MettreAJourBarreProgression` :**  Met à jour la position d'une barre de progression.
* **`intToWstring` :**  Fonction utilitaire pour convertir un entier en une chaîne de caractères larges (wstring).


**Fonctionnement du simulateur :**

1. **Initialisation :**  `WinMain`  initialise l'application Windows et crée la fenêtre principale.  `WindowProc`  gère le message  `WM_CREATE`  pour créer les boutons, le checkbox, le champ éditable pour le quantum de Round Robin, les barres de progression et les étiquettes pour chaque processus.

2. **Sélection de l'algorithme :**  L'utilisateur choisit un algorithme d'ordonnancement en cliquant sur le bouton correspondant (FCFS, SJF ou Round Robin).  Pour SJF, l'utilisateur peut également activer ou désactiver la préemption à l'aide du checkbox. Pour Round Robin, l'utilisateur peut définir le quantum de temps.

3. **Démarrage de la simulation :**  Lorsque l'utilisateur clique sur un bouton d'algorithme, la fonction  `DemarrerSimulation`  est appelée. Cette fonction initialise les variables de simulation (temps actuel, index du processus actuel, file d'attente pour Round Robin), trie les processus selon l'algorithme choisi (sauf pour Round Robin qui utilise une file d'attente), et démarre le timer.

4. **Simulation par le timer :**  La fonction  `GestionTimer`  est appelée à chaque tick du timer.  Elle implémente la logique de chaque algorithme d'ordonnancement :
    * **FCFS :**  Exécute les processus dans l'ordre d'arrivée.
    * **SJF (non préemptif) :**  Exécute le processus avec le temps d'exécution le plus court parmi ceux qui sont arrivés.
    * **SJF (préemptif) :**  Exécute le processus avec le temps d'exécution restant le plus court parmi ceux qui sont arrivés. Interrompt le processus en cours si un processus plus court arrive.
    * **Round Robin :**  Exécute chaque processus pendant un quantum de temps, puis passe au processus suivant dans la file d'attente.

5. **Mise à jour de l'interface graphique :**  `GestionTimer`  met à jour les barres de progression et les étiquettes pour refléter l'état actuel des processus.

6. **Fin de la simulation :**  La simulation se termine lorsque tous les processus ont terminé leur exécution.  Le timer est alors arrêté.


**Gestion des processus :**

Les processus sont représentés par la structure  `Processus`, qui contient les informations suivantes : ID, temps d'arrivée, temps d'exécution, temps restant, handle de la barre de progression et handle de l'étiquette.  Les processus sont stockés dans un vecteur  `processus`.


**Algorithmes d'ordonnancement :**

* **FCFS :**  Simple et facile à implémenter, mais peut entraîner des temps d'attente longs pour les processus courts si des processus longs arrivent en premier.
* **SJF :**  Minimise le temps d'attente moyen, mais nécessite de connaître la durée d'exécution de chaque processus à l'avance. La version préemptive est plus efficace mais plus complexe à implémenter.
* **Round Robin :**  Equitable pour tous les processus, mais le temps de réponse dépend du quantum de temps.


**Conclusion :**

Ce programme fournit une illustration visuelle du fonctionnement des différents algorithmes d'ordonnancement de processus. Il permet de comprendre comment chaque algorithme affecte le temps d'attente, le temps de réponse et l'utilisation du processeur.  L'interface graphique facilite la comparaison des performances des différents algorithmes.
