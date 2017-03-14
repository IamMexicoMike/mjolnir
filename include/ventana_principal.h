#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#define WIN32_LEAN_AND_MEAN //se supone que hace muchos headers derivados no sean incluidos, y por ende acelerar compilación

#include <windows.h>
#include <windowsx.h>

#define ID_T30 1001
#define ID_T5000 1002
#define ID_PB1 5001
#define ID_RB1 5101
#define ID_RB2 5102
#define ID_CB1 5201
#define IDC_MAIN_EDIT 6000

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool registrarClase(WNDCLASSEX& wc, HINSTANCE& hInstance);
bool crearVentana(HWND& hwnd, HINSTANCE& hInstance);
void inicializar_diagrama(HWND& hwnd);
void configuramos_parametros_diagrama(HWND& hwnd);
BOOL CALLBACK DialogoTextoProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void crear_dialogo_nombre();

#endif // VENTANA_PRINCIPAL_H
