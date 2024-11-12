#define UNICODE
#define _WIN32_IE 0x0500
#include <windows.h>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <string>
#include <sstream>

// Fonction personnalisée pour convertir un entier en std::wstring
std::wstring intToWstring(int value) {
    std::wstringstream wss;
    wss << value;
    return wss.str();
}

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

// Fonction pour démarrer la simulation avec un timer
void DemarrerSimulationFCFS(HWND hwnd) {
    std::sort(processus.begin(), processus.end(), [](const Processus& a, const Processus& b) {
        return a.temps_arrivee < b.temps_arrivee;
    });
    index_processus_actuel = 0;
    temps_actuel = 0;
    SetTimer(hwnd, 1, 100, NULL); // Déclenchement toutes les 100 ms
}

// Gestionnaire de Timer pour la simulation FCFS
void GestionTimer(HWND hwnd) {
    if (index_processus_actuel >= processus.size()) {
        KillTimer(hwnd, 1); // Arrête le timer une fois tous les processus terminés
        return;
    }

    Processus& p = processus[index_processus_actuel];

    if (temps_actuel >= p.temps_arrivee) {
        if (p.temps_restant > 0) {
            p.temps_restant--;
            temps_actuel++;
            int pourcentage = (p.temps_execution - p.temps_restant) * 100 / p.temps_execution;
            MettreAJourBarreProgression(p.barre_progression, pourcentage);

            // Mettre à jour le texte de statut du processus
            std::wstring label_text = L"Processus " + intToWstring(p.id) + L": " + intToWstring(p.temps_execution - p.temps_restant) + L"/" + intToWstring(p.temps_execution);
            SetWindowText(p.label, label_text.c_str());
        } else {
            index_processus_actuel++; // Passer au processus suivant
        }
    } else {
        temps_actuel++; // Avancer le temps si aucun processus n'est prêt
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(RGB(240, 240, 240)));

        // Créer des processus exemples avec des barres et des labels
        processus.push_back({ 1, 0, 5, 5, CreerBarreProgression(hwnd, 10, 40, 250, 20, 1), CreerLabel(hwnd, L"Processus 1: 0/5", 10, 20, 200, 20) });
        processus.push_back({ 2, 1, 3, 3, CreerBarreProgression(hwnd, 10, 90, 250, 20, 2), CreerLabel(hwnd, L"Processus 2: 0/3", 10, 70, 200, 20) });
        processus.push_back({ 3, 2, 7, 7, CreerBarreProgression(hwnd, 10, 140, 250, 20, 3), CreerLabel(hwnd, L"Processus 3: 0/7", 10, 120, 200, 20) });

        DemarrerSimulationFCFS(hwnd);
        break;
    }
    case WM_TIMER: {
        GestionTimer(hwnd);
        break;
    }
    case WM_DESTROY:
        KillTimer(hwnd, 1); // Arrête le timer si la fenêtre est fermée
        PostQuitMessage(0);
        return 0;

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
