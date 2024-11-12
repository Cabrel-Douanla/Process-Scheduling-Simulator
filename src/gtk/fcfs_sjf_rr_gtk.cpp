#include <gtk/gtk.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>

// Structure pour représenter un processus
struct Processus {
    int id;
    int temps_arrivee;
    int temps_execution;
    int temps_restant;
    GtkWidget *barre_progression;
    GtkWidget *label;
};

// Données globales
std::vector<Processus> processus;
int temps_actuel = 0;
size_t index_processus_actuel = 0;
int quantum = 2; // Quantum de temps pour Round Robin

// Variables globales pour la simulation
enum Algorithme { FCFS, SJF, RR };
Algorithme algorithme_actuel = FCFS; // Algorithme par défaut
bool sjf_preemptif = false;
std::queue<size_t> file_rr;

// Fonction pour convertir un entier en std::string
std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Fonction pour créer une barre de progression
GtkWidget* CreerBarreProgression() {
    GtkWidget *barre = gtk_progress_bar_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(barre), "progressbar");
    return barre;
}

// Fonction pour créer un label pour chaque processus
GtkWidget* CreerLabel(const std::string& text) {
    GtkWidget *label = gtk_label_new(text.c_str());
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "label");
    return label;
}

// Fonction pour mettre à jour une barre de progression
void MettreAJourBarreProgression(GtkWidget *barre_progression, double fraction) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(barre_progression), fraction);
}

// Gestionnaire de Timer
gboolean GestionTimer(gpointer data) {
    if (algorithme_actuel == RR) { // Round Robin
        if (file_rr.empty()) {
            return FALSE;
        }

        size_t index_courant = file_rr.front();
        Processus& p = processus[index_courant];

        if (p.temps_restant > 0) {
            p.temps_restant--;
            temps_actuel++;

            double fraction = (p.temps_execution - p.temps_restant) / (double)p.temps_execution;
            MettreAJourBarreProgression(p.barre_progression, fraction);

            std::string label_text = "Processus " + intToString(p.id) + ": " + intToString(p.temps_execution - p.temps_restant) + "/" + intToString(p.temps_execution);
            gtk_label_set_text(GTK_LABEL(p.label), label_text.c_str());

            if (p.temps_restant == 0) {
                file_rr.pop();
            } else if ((temps_actuel - (p.temps_execution - p.temps_restant) - 1) % quantum == 0) {  // Vérification du quantum
                file_rr.pop();
                if (p.temps_restant > 0) file_rr.push(index_courant); // Remettre en queue si pas terminé
            }
        }

        // Ajouter les processus arrivés à la file (uniquement pour Round Robin)
        for (size_t i = 0; i < processus.size(); ++i) {
            if (processus[i].temps_arrivee == temps_actuel && processus[i].temps_restant > 0) {
                // Vérifier si le processus est déjà dans la file (en utilisant une copie pour éviter les problèmes d'itération)
                bool deja_present = false;
                std::queue<size_t> file_temp = file_rr;
                while (!file_temp.empty()) {
                    if (file_temp.front() == i) {
                        deja_present = true;
                        break;
                    }
                    file_temp.pop();
                }
                if (!deja_present) file_rr.push(i);
            }
        }

    } else { // FCFS ou SJF
        if (index_processus_actuel >= processus.size()) {
            return FALSE;
        }

        if (sjf_preemptif) { // Logique de préemption SJF
            size_t prochain_processus = index_processus_actuel;
            int temps_restant_min = processus[index_processus_actuel].temps_restant;

            for (size_t i = 0; i < processus.size(); ++i) { // Parcourir tous les processus
                if (processus[i].temps_arrivee <= temps_actuel && processus[i].temps_restant < temps_restant_min && processus[i].temps_restant > 0) {
                    temps_restant_min = processus[i].temps_restant;
                    prochain_processus = i;
                }
            }

            // Changer de processus si un processus plus court est disponible
            index_processus_actuel = prochain_processus;
        }

        Processus& p = processus[index_processus_actuel];

        if (temps_actuel >= p.temps_arrivee) {
            if (p.temps_restant > 0) {
                p.temps_restant--;
                temps_actuel++;
                double fraction = (p.temps_execution - p.temps_restant) / (double)p.temps_execution;
                MettreAJourBarreProgression(p.barre_progression, fraction);
                std::string label_text = "Processus " + intToString(p.id) + ": " + intToString(p.temps_execution - p.temps_restant) + "/" + intToString(p.temps_execution);
                gtk_label_set_text(GTK_LABEL(p.label), label_text.c_str());
            } else {
                index_processus_actuel++; // Passer au processus suivant seulement si le processus actuel est terminé

                if (sjf_preemptif) {
                    // Retrier les processus restants par temps d'exécution restant (uniquement si préemptif)
                    std::sort(processus.begin() + index_processus_actuel, processus.end(), [](const Processus& a, const Processus& b) {
                        if (a.temps_arrivee <= temps_actuel && b.temps_arrivee <= temps_actuel)
                            return a.temps_restant < b.temps_restant;
                        if (a.temps_arrivee <= temps_actuel)
                            return true;
                        return false;
                    });
                }
            }
        } else {
            temps_actuel++;
        }
    }
    return TRUE;
}

// Fonction pour démarrer la simulation (générique)
void DemarrerSimulation(GtkWidget *widget, gpointer data) {
    temps_actuel = 0;
    index_processus_actuel = 0; // Réinitialiser l'index du processus actuel
    while (!file_rr.empty()) file_rr.pop(); // Vider la file Round Robin

    if (algorithme_actuel == RR) {
        // Round Robin: Initialiser la file d'attente avec les processus arrivés au temps 0
        for (size_t i = 0; i < processus.size(); ++i) {
            processus[i].temps_restant = processus[i].temps_execution;
            MettreAJourBarreProgression(processus[i].barre_progression, 0);
            std::string label_text = "Processus " + intToString(processus[i].id) + ": 0/" + intToString(processus[i].temps_execution);
            gtk_label_set_text(GTK_LABEL(processus[i].label), label_text.c_str());
            if (processus[i].temps_arrivee == temps_actuel) {
                file_rr.push(i);
            }
        }
    } else if (algorithme_actuel == SJF) {
        // SJF (non préemptif): Trier par temps d'arrivée, puis par temps d'exécution
        std::sort(processus.begin(), processus.end(), [](const Processus& a, const Processus& b) {
            if (a.temps_arrivee != b.temps_arrivee) {
                return a.temps_arrivee < b.temps_arrivee;
            } else {
                return a.temps_execution < b.temps_execution; // Critère SJF si temps d'arrivée égal
            }
        });
        for (auto& p : processus) {
            p.temps_restant = p.temps_execution;
            MettreAJourBarreProgression(p.barre_progression, 0);
            std::string label_text = "Processus " + intToString(p.id) + ": 0/" + intToString(p.temps_execution);
            gtk_label_set_text(GTK_LABEL(p.label), label_text.c_str());
        }
    } else { // FCFS
        // FCFS: Trier par temps d'arrivée
        std::sort(processus.begin(), processus.end(), [](const Processus& a, const Processus& b) {
            return a.temps_arrivee < b.temps_arrivee;
        });

        // Réinitialiser l'état des processus
        for (auto& p : processus) {
            p.temps_restant = p.temps_execution;
            MettreAJourBarreProgression(p.barre_progression, 0);
            std::string label_text = "Processus " + intToString(p.id) + ": 0/" + intToString(p.temps_execution);
            gtk_label_set_text(GTK_LABEL(p.label), label_text.c_str());
        }
    }

    g_timeout_add(100, GestionTimer, NULL);
}

// Fonction pour gérer le clic sur les boutons
void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = GTK_BUILDER(data);
    if (widget == GTK_WIDGET(gtk_builder_get_object(builder, "button_fcfs"))) {
        algorithme_actuel = FCFS;
    } else if (widget == GTK_WIDGET(gtk_builder_get_object(builder, "button_sjf"))) {
        algorithme_actuel = SJF;
    } else if (widget == GTK_WIDGET(gtk_builder_get_object(builder, "button_rr"))) {
        algorithme_actuel = RR;
        // Récupérer la valeur du quantum depuis le champ éditable
        const gchar *quantumText = gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder, "entry_quantum")));
        quantum = atoi(quantumText);
    }
    DemarrerSimulation(widget, data);
}

// Fonction pour gérer le changement d'état de la checkbox
void on_checkbox_toggled(GtkToggleButton *togglebutton, gpointer data) {
    sjf_preemptif = gtk_toggle_button_get_active(togglebutton);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "fcfs_sjf_rr_gtk.glade", NULL);

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    GtkWidget *button_fcfs = GTK_WIDGET(gtk_builder_get_object(builder, "button_fcfs"));
    GtkWidget *button_sjf = GTK_WIDGET(gtk_builder_get_object(builder, "button_sjf"));
    GtkWidget *button_rr = GTK_WIDGET(gtk_builder_get_object(builder, "button_rr"));
    GtkWidget *checkbox_preemptif = GTK_WIDGET(gtk_builder_get_object(builder, "checkbox_preemptif"));
    GtkWidget *entry_quantum = GTK_WIDGET(gtk_builder_get_object(builder, "entry_quantum"));

    if (!window || !button_fcfs || !button_sjf || !button_rr || !checkbox_preemptif || !entry_quantum) {
        g_printerr("Erreur: Un ou plusieurs objets GTK n'ont pas été trouvés.\n");
        return 1;
    }

    g_signal_connect(button_fcfs, "clicked", G_CALLBACK(on_button_clicked), builder);
    g_signal_connect(button_sjf, "clicked", G_CALLBACK(on_button_clicked), builder);
    g_signal_connect(button_rr, "clicked", G_CALLBACK(on_button_clicked), builder);
    g_signal_connect(checkbox_preemptif, "toggled", G_CALLBACK(on_checkbox_toggled), NULL);

    // Créer des processus exemples avec des barres et des labels
    processus.push_back({ 1, 0, 5, 5, CreerBarreProgression(), CreerLabel("Processus 1: 0/5") });
    processus.push_back({ 2, 1, 3, 3, CreerBarreProgression(), CreerLabel("Processus 2: 0/3") });
    processus.push_back({ 3, 2, 7, 7, CreerBarreProgression(), CreerLabel("Processus 3: 0/7") });

    GtkWidget *grid = GTK_WIDGET(gtk_builder_get_object(builder, "grid"));
    if (!grid) {
        g_printerr("Erreur: La grille GTK n'a pas été trouvée.\n");
        return 1;
    }

    for (size_t i = 0; i < processus.size(); ++i) {
        gtk_grid_attach(GTK_GRID(grid), processus[i].label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), processus[i].barre_progression, 1, i + 1, 1, 1);
    }

    // Charger les styles CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
