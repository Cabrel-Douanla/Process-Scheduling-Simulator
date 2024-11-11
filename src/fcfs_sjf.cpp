#define UNICODE
#define _WIN32_IE 0x0500
#include <windows.h>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <string>
#include <sstream>


#define ID_TIMER 1
#define ID_BOUTON_FCFS 2
#define ID_BOUTON_SJF 3
#define ID_CHECKBOX_PREEMPTIF 4


// Structure pour représenter un processus
struct Processus {
    int id;
    int temps_arrivee;
    int temps_execution;
    int temps_restant;
    HWND barre_progression;
    HWND label;
};

// Données globales
std::vector<Processus> processus;
int temps_actuel = 0;
size_t index_processus_actuel = 0;

// Variables globales pour la simulation
enum Algorithme { FCFS, SJF };
Algorithme algorithme_actuel = FCFS; // Algorithme par défaut
bool sjf_preemptif = false;


// Fonction pour convertir un entier en std::wstring
std::wstring intToWstring(int value) {
    std::wstringstream wss;
    wss << value;
    return wss.str();
}


// Fonction pour créer une barre de progression
HWND CreerBarreProgression(HWND hwndParent, int x, int y, int largeur, int hauteur, int id) {
    HWND hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
                                       WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                       x, y, largeur, hauteur,
                                       hwndParent, (HMENU)id, GetModuleHandle(NULL), NULL);
    SendMessage(hProgressBar, PBM_SETRANGE32, 0, 100); // Utilisation de PBM_SETRANGE32
    return hProgressBar;
}

// Fonction pour créer un label pour chaque processus
HWND CreerLabel(HWND hwndParent, const std::wstring& text, int x, int y, int largeur, int hauteur) {
    return CreateWindowEx(0, L"STATIC", text.c_str(),
                          WS_CHILD | WS_VISIBLE | SS_LEFT,
                          x, y, largeur, hauteur,
                          hwndParent, NULL, GetModuleHandle(NULL), NULL);
}

// Fonction pour mettre à jour une barre de progression
void MettreAJourBarreProgression(HWND hProgressBar, int pourcentage) {
    SendMessage(hProgressBar, PBM_SETPOS, pourcentage, 0);
}


// Fonction pour démarrer la simulation (générique)
void DemarrerSimulation(HWND hwnd) {
    KillTimer(hwnd, ID_TIMER); 
    temps_actuel = 0;
    index_processus_actuel = 0;

    // Tri des processus en fonction de l'algorithme
    if (algorithme_actuel == SJF) {
        std::sort(processus.begin(), processus.end(), [](const Processus& a, const Processus& b) {
            if (a.temps_arrivee != b.temps_arrivee) {
                return a.temps_arrivee < b.temps_arrivee;
            } else {
                return a.temps_execution < b.temps_execution; 
            }
        });
    } else { // FCFS
        std::sort(processus.begin(), processus.end(), [](const Processus& a, const Processus& b) {
            return a.temps_arrivee < b.temps_arrivee;
        });
    }

    for (auto& p : processus) {
        p.temps_restant = p.temps_execution;
        MettreAJourBarreProgression(p.barre_progression, 0);
        std::wstring label_text = L"Processus " + intToWstring(p.id) + L": 0/" + intToWstring(p.temps_execution);
        SetWindowText(p.label, label_text.c_str());
    }

    SetTimer(hwnd, ID_TIMER, 100, NULL);
}



// Gestionnaire de Timer
void GestionTimer(HWND hwnd) {
    if (index_processus_actuel >= processus.size()) {
        KillTimer(hwnd, ID_TIMER);
        return;
    }

    if (algorithme_actuel == SJF && sjf_preemptif) {
        // SJF Préemptif : Trouver le processus avec le plus petit temps restant parmi ceux qui sont arrivés
        size_t prochain_processus = index_processus_actuel;
        int temps_restant_min = processus[index_processus_actuel].temps_restant;

        for (size_t i = index_processus_actuel + 1; i < processus.size(); ++i) {
            if (processus[i].temps_arrivee <= temps_actuel && processus[i].temps_restant < temps_restant_min) {
                temps_restant_min = processus[i].temps_restant;
                prochain_processus = i;
            }
        }

        // Si un processus plus court est trouvé, passer à ce processus
        if (prochain_processus != index_processus_actuel) {
            index_processus_actuel = prochain_processus;
        }
    }

    Processus& p = processus[index_processus_actuel];

    if (temps_actuel >= p.temps_arrivee) {
        if (p.temps_restant > 0) {
            p.temps_restant--;
            temps_actuel++;
            int pourcentage = (p.temps_execution - p.temps_restant) * 100 / p.temps_execution;
            MettreAJourBarreProgression(p.barre_progression, pourcentage);
            std::wstring label_text = L"Processus " + intToWstring(p.id) + L": " + intToWstring(p.temps_execution - p.temps_restant) + L"/" + intToWstring(p.temps_execution);
            SetWindowText(p.label, label_text.c_str());
        } else {
            index_processus_actuel++;

            if (algorithme_actuel == SJF && sjf_preemptif) {
                // Retrier les processus restants par temps d'exécution restant
                std::sort(processus.begin() + index_processus_actuel, processus.end(), [](const Processus& a, const Processus& b) {
                   if(a.temps_arrivee <= temps_actuel && b.temps_arrivee <= temps_actuel)
                        return a.temps_restant < b.temps_restant;
                    if(a.temps_arrivee <= temps_actuel)
                        return true;

                    return false;

                });
            }
        }
    } else {
        temps_actuel++;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Initialisation des contrôles communs
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_WIN95_CLASSES;
            InitCommonControlsEx(&icex);

            HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);

            // Créer le bouton FCFS
            CreateWindowEx(0, L"BUTTON", L"Démarrer FCFS",
                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                           10, 10, 100, 30, hwnd, (HMENU)ID_BOUTON_FCFS, GetModuleHandle(NULL), NULL);

            // Créer le bouton SJF
            CreateWindowEx(0, L"BUTTON", L"Démarrer SJF",
                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                           120, 10, 100, 30, hwnd, (HMENU)ID_BOUTON_SJF, GetModuleHandle(NULL), NULL);

            // Créer la checkbox pour SJF préemptif (désactivée car non utilisée)
            CreateWindowEx(0, L"BUTTON", L"Préemptif", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,  // <-- BS_AUTOCHECKBOX ajouté
                   230, 10, 80, 30, hwnd, (HMENU)ID_CHECKBOX_PREEMPTIF, GetModuleHandle(NULL), NULL);

            // Créer des processus exemples avec des barres et des labels
            processus.push_back({ 1, 0, 5, 5, CreerBarreProgression(hwnd, 10, 70, 250, 20, 1), CreerLabel(hwnd, L"Processus 1: 0/5", 10, 50, 200, 20) });
            processus.push_back({ 2, 1, 3, 3, CreerBarreProgression(hwnd, 10, 120, 250, 20, 2), CreerLabel(hwnd, L"Processus 2: 0/3", 10, 100, 200, 20) });
            processus.push_back({ 3, 2, 7, 7, CreerBarreProgression(hwnd, 10, 170, 250, 20, 3), CreerLabel(hwnd, L"Processus 3: 0/7", 10, 150, 200, 20) });

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BOUTON_FCFS:
                    algorithme_actuel = FCFS;
                    DemarrerSimulation(hwnd);
                    break;
                case ID_BOUTON_SJF:
                    algorithme_actuel = SJF;
                    DemarrerSimulation(hwnd);
                    break;
                
                case ID_CHECKBOX_PREEMPTIF:
                    sjf_preemptif = (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    break;
            }
            break;

        case WM_TIMER:
            if (wParam == ID_TIMER) {
                GestionTimer(hwnd);
            }
            break;

        case WM_DESTROY: {
            HBRUSH hBrush = (HBRUSH)GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND);
            DeleteObject(hBrush); // Libérer le pinceau
            KillTimer(hwnd, ID_TIMER);
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Simulateur FCFS";

    WNDCLASSW wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Simulateur de planification FCFS",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}