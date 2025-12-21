/*******************************************************************************************
 CALCULADORA MOYA – GUI RAYLIB - EDICIÓN ESPACIAL ULTIMATE
 VERSIÓN COMPLETA CON TODAS LAS MEJORAS VISUALES Y FUNCIONALES
 SONIDOS ESPACIALES, ANIMACIONES, TEMAS, Y MUCHO MÁS. [BY -srmackenzie-]
********************************************************************************************/

#include <raylib.h>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <cstring>

using namespace std;

// ======================================================
// ESTRUCTURAS Y VARIABLES GLOBALES
// ======================================================

vector<string> historial;
string userName = "";
string historialFilePath = "historial.txt"; // default, may be overridden to APPDATA
string sessionHistorialPath = ""; // per-run session file in exe folder
int historialCount = 0; // number of lines written to historial file (kept on disk only)

// Estructura para temas
struct Tema {
    Color fondo;
    Color texto;
    Color acento;
    Color tarjeta;
    Color verde;
    Color naranja;
    Color purpura;
    Color rojo;
};

// Temas disponibles
Tema temaOscuro = {{18, 18, 18, 255}, {240, 240, 240, 255}, {100, 150, 255, 255}, 
                  {30, 30, 35, 255}, {100, 220, 100, 255}, {255, 160, 40, 255}, 
                  {180, 80, 220, 255}, {255, 80, 80, 255}};

Tema temaEspacial = {{10, 10, 30, 255}, {220, 240, 255, 255}, {80, 180, 255, 255},
                    {25, 25, 45, 255}, {100, 255, 200, 255}, {255, 200, 50, 255},
                    {200, 100, 255, 255}, {255, 100, 100, 255}};

Tema temaNeon = {{5, 5, 15, 255}, {255, 255, 255, 255}, {0, 255, 255, 255},
                {20, 20, 40, 255}, {0, 255, 150, 255}, {255, 100, 0, 255},
                {200, 0, 255, 255}, {255, 0, 100, 255}};

Tema* temaActual = &temaEspacial;

// Variables de animación
float animacionAlpha = 0.0f;
bool animacionEntrando = true;
double ultimoBorrado = 0;

// Sonidos
Sound clickSound;
Sound beepSound;
Sound successSound;
Sound errorSound;
Music backgroundMusic;
bool musicaReproduciendo = false;

// ======================================================
// DECLARACIÓN DE FUNCIONES
// ======================================================

void pantallaInicial();
void pantallaSumarRestar();
void pantallaMultiplicar();
void pantallaDividir();
void pantallaDistanciaPlanetas();
void pantallaFibonacci();
void pantallaParImpar();
void pantallaFactorial();
void pantallaPotencia();
void tecladoNumerico(string &buffer, int x, int y);
bool botonSimple(Rectangle r, const char *text);
void dibujarFondoAnimado();
void dibujarTarjetaConSombra(Rectangle rect, Color color, float redondeo = 0.1f);
void dibujarTextoConSombra(const char* texto, int x, int y, int fontSize, Color color);
void dibujarIndicadorEstado(int x, int y, const char* texto, Color color);
void mostrarResultadoBonito(int x, int y, const string& operacion, const string& resultado);
void mostrarTooltip(const char* texto, int x, int y);
void mostrarEfectoExito(int x, int y);
void mostrarEfectoError(int x, int y);
void cambiarTema();
void reproducirSonido(Sound sonido);
void cargarHistorial();
void appendHistorialLine(const string& line);
void appendSectionHeader(const string& usuario);
void setWorkingDirectoryToExePath(int argc, char **argv);
void initHistorialPath();
void inicializarRecursos();
void liberarRecursos();
bool esNumeroValido(const string& str);
void dibujarIconoMenu(int index, int x, int y, bool hover);
void dibujarTituloDecorado(const char* texto, int x, int y, int fontSize, Color color);

// ======================================================
// OPCIONES MENÚ MEJORADAS
// ======================================================

const char *menuItems[] = {
    "1) Sumar / Restar",
    "2) Multiplicar", 
    "3) Dividir",
    "4) Distancia Planetas",
    "5) Fibonacci",
    "6) Par/Impar",
    "7) Factorial",
    "8) Potencia",
    "9) Ver Historial",
    "T) Cambiar Tema",
    "0) Salir"
};
int menuCount = 11;

// ======================================================
// FUNCIÓN PRINCIPAL
// ======================================================

int main(int argc, char** argv) {
    // Asegurar que el directorio de trabajo sea la carpeta del ejecutable
    setWorkingDirectoryToExePath(argc, argv);
    InitWindow(900, 600, "Calculadora Moya SPACE EDITION");
    SetTargetFPS(60);
    
    // Inicializar audio y recursos
    InitAudioDevice();
    // Inicializar la ruta de historial (por usuario si es posible)
    initHistorialPath();
    inicializarRecursos();
    
    pantallaInicial();
    
    while (!WindowShouldClose()) {
        // Actualizar música de fondo
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        
        dibujarFondoAnimado();
        
        // Título principal con efectos mejorados
        string titulo = "BIENVENIDO " + userName;
        dibujarTituloDecorado(titulo.c_str(), (900 - MeasureText(titulo.c_str(), 36)) / 2, 40, 36, temaActual->acento);
        
        string version = "CALCULATOR SPACE EDITION V3.0";
        dibujarTextoConSombra(version.c_str(), (900 - MeasureText(version.c_str(), 20)) / 2, 90, 20, Fade(temaActual->acento, 0.8f));
        
        // Marco principal mejorado
        Rectangle marco = {150, 120, 600, 450};
        dibujarTarjetaConSombra(marco, temaActual->tarjeta, 0.15f);
        
        // Opciones del menú con efectos hover mejorados
        int y = 150;
        for(int i = 0; i < menuCount; i++) {
            Rectangle opcionRect = {175, (float)(y-5), 550, 40};
            bool overOpcion = CheckCollisionPointRec(GetMousePosition(), opcionRect);
            
            if(overOpcion) {
                DrawRectangleRounded(opcionRect, 0.3f, 8, Fade(temaActual->verde, 0.15f));
                DrawRectangleRoundedLinesEx(opcionRect, 0.3f, 8, 2, Fade(temaActual->verde, 0.6f));
                
                // Tooltip para opciones
                if(IsKeyDown(KEY_LEFT_SHIFT)) {
                    mostrarTooltip("Haz clic o presiona la tecla correspondiente", GetMouseX() + 10, GetMouseY() - 30);
                }
            }
            
            // Icono decorativo mejorado (sin emojis problemáticos)
            dibujarIconoMenu(i, 190, y, overOpcion);
            
            dibujarTextoConSombra(menuItems[i], 230, y, 24, overOpcion ? temaActual->verde : temaActual->texto);
            y += 40;
        }
        
        // Contador de historial (file-backed)
        string historialInfo = string("Operaciones en historial: ") + to_string(historialCount);
        dibujarTextoConSombra(historialInfo.c_str(), 300, 540, 18, Fade(temaActual->acento, 0.7f));
        
        // Navegación por teclado
        if (IsKeyPressed(KEY_ONE)) pantallaSumarRestar();
        if (IsKeyPressed(KEY_TWO)) pantallaMultiplicar();
        if (IsKeyPressed(KEY_THREE)) pantallaDividir();
        if (IsKeyPressed(KEY_FOUR)) pantallaDistanciaPlanetas();
        if (IsKeyPressed(KEY_FIVE)) pantallaFibonacci();
        if (IsKeyPressed(KEY_SIX)) pantallaParImpar();
        if (IsKeyPressed(KEY_SEVEN)) pantallaFactorial();
        if (IsKeyPressed(KEY_EIGHT)) pantallaPotencia();
        if (IsKeyPressed(KEY_NINE)) {
            while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
                // Leer el archivo de historial en el momento (no almacenar en memoria global)
                vector<string> tempLines;
                try {
                    ifstream fin(historialFilePath);
                    string line;
                    while(getline(fin, line)) {
                        tempLines.push_back(line);
                    }
                } catch(...) {
                    // Ignorar errores de lectura y mostrar vacío
                }

                BeginDrawing();
                ClearBackground(temaActual->fondo);
                dibujarFondoAnimado();

                dibujarTituloDecorado("HISTORIAL DE OPERACIONES", 250, 40, 34, temaActual->acento);

                Rectangle historialRect = {50, 100, 800, 450};
                dibujarTarjetaConSombra(historialRect, temaActual->tarjeta, 0.1f);

                int yy = 130;
                for(const string& h : tempLines) {
                    Color colorLinea = (yy % 50 == 0) ? temaActual->texto : Fade(temaActual->texto, 0.8f);
                    DrawText(h.c_str(), 70, yy, 20, colorLinea);
                    yy += 25;
                    if(yy > 520) break; // evitar overflow visual
                }

                if(tempLines.empty()) {
                    dibujarTextoConSombra("El historial esta vacio", 350, 300, 26, Fade(temaActual->texto, 0.6f));
                }

                dibujarTextoConSombra("BACKSPACE -> volver al menu", 320, 570, 20, Fade(temaActual->texto, 0.7f));
                EndDrawing();
            }
        }
        if (IsKeyPressed(KEY_T)) cambiarTema();
        if (IsKeyPressed(KEY_ZERO)) break;
        
        // Navegación por mouse
        for(int i = 0; i < menuCount; i++) {
            Rectangle opcionRect = {175, (float)(150 + i*40 - 5), 550, 40};
            if(CheckCollisionPointRec(GetMousePosition(), opcionRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                reproducirSonido(clickSound);
                switch(i) {
                    case 0: pantallaSumarRestar(); break;
                    case 1: pantallaMultiplicar(); break;
                    case 2: pantallaDividir(); break;
                    case 3: pantallaDistanciaPlanetas(); break;
                    case 4: pantallaFibonacci(); break;
                    case 5: pantallaParImpar(); break;
                    case 6: pantallaFactorial(); break;
                    case 7: pantallaPotencia(); break;
                    case 8: {
                        while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
                            BeginDrawing();
                            ClearBackground(temaActual->fondo);
                            dibujarFondoAnimado();
                            
                            dibujarTituloDecorado("HISTORIAL DE OPERACIONES", 250, 40, 34, temaActual->acento);
                            
                            Rectangle historialRect = {50, 100, 800, 450};
                            dibujarTarjetaConSombra(historialRect, temaActual->tarjeta, 0.1f);
                            
                            int yy = 130;
                            for(const string& h : historial) {
                                Color colorLinea = (yy % 50 == 0) ? temaActual->texto : Fade(temaActual->texto, 0.8f);
                                DrawText(h.c_str(), 70, yy, 20, colorLinea);
                                yy += 25;
                            }
                            
                            if(historial.empty()) {
                                dibujarTextoConSombra("El historial esta vacio", 350, 300, 26, Fade(temaActual->texto, 0.6f));
                            }
                            
                            dibujarTextoConSombra("BACKSPACE -> volver al menu", 320, 570, 20, Fade(temaActual->texto, 0.7f));
                            EndDrawing();
                        }
                    } break;
                    case 9: cambiarTema(); break;
                    case 10: break;
                }
            }
        }
        
        EndDrawing();
    }
    
    liberarRecursos();
    CloseWindow();
    return 0;
}

// ======================================================
// INICIALIZACIÓN DE RECURSOS
// ======================================================

void inicializarRecursos() {
    // Cargar sonidos .wav
    clickSound = LoadSound("assets/sounds/click_space.wav");
    beepSound = LoadSound("assets/sounds/beep_space.wav");
    errorSound = LoadSound("assets/sounds/error_space.wav");

    // Intentar cargar el sonido de éxito; si no existe, usar clickSound como fallback
    if (std::filesystem::exists("assets/sounds/success_space.wav")) {
        successSound = LoadSound("assets/sounds/success_space.wav");
    } else {
        // Si hay un archivo con nombre cercano (typo) intentamos usarlo como música corta
        if (std::filesystem::exists("assets/sounds/succes_space.mp3")) {
            // No es ideal cargar mp3 en Sound, así que dejamos successSound apuntando a clickSound
            successSound = clickSound;
        } else {
            successSound = clickSound;
        }
    }
    
    // Cargar música de fondo .mp3 (ruta relativa)
    backgroundMusic = LoadMusicStream("assets/music/space_ambient.mp3");
    
    // Configurar música
    SetMusicVolume(backgroundMusic, 0.3f); // Volumen al 30%
    if (IsAudioDeviceReady()) {
        PlayMusicStream(backgroundMusic);
        musicaReproduciendo = true;
    } else {
        musicaReproduciendo = false;
    }

    // Cargar historial desde archivo (si existe)
    cargarHistorial();
}

void liberarRecursos() {
    // Detener y liberar música
    if (musicaReproduciendo) {
        StopMusicStream(backgroundMusic);
    }
    UnloadMusicStream(backgroundMusic);
    
    // Liberar sonidos
    UnloadSound(clickSound);
    UnloadSound(beepSound);
    UnloadSound(successSound);
    UnloadSound(errorSound);
    
    CloseAudioDevice();
}

// ======================================================
// TÍTULOS DECORADOS (SIN EMOJIS)
// ======================================================

void dibujarTituloDecorado(const char* texto, int x, int y, int fontSize, Color color) {
    // Dibujar texto con sombra
    dibujarTextoConSombra(texto, x, y, fontSize, color);
    
    // Decoración con formas geométricas en lugar de emojis
    float tiempo = GetTime();
    float pulso = 0.7f + sin(tiempo * 3) * 0.3f;
    
    // Círculo izquierdo decorativo
    DrawCircle(x - 25, y + fontSize/2, 8, Fade(temaActual->verde, pulso));
    DrawCircleLines(x - 25, y + fontSize/2, 8, Fade(color, 0.8f));
    
    // Círculo derecho decorativo
    DrawCircle(x + MeasureText(texto, fontSize) + 25, y + fontSize/2, 8, Fade(temaActual->purpura, pulso));
    DrawCircleLines(x + MeasureText(texto, fontSize) + 25, y + fontSize/2, 8, Fade(color, 0.8f));
    
    // Líneas decorativas
    DrawLine(x - 40, y + fontSize + 5, x + MeasureText(texto, fontSize) + 40, y + fontSize + 5, Fade(color, 0.3f));
    DrawLine(x - 35, y + fontSize + 8, x + MeasureText(texto, fontSize) + 35, y + fontSize + 8, Fade(color, 0.2f));
}

// ======================================================
// ICONOS DEL MENÚ MEJORADOS (SIN EMOJIS)
// ======================================================

void dibujarIconoMenu(int index, int x, int y, bool hover) {
    Color colorIcono = hover ? temaActual->verde : temaActual->acento;
    float tiempo = GetTime();
    float pulso = 0.7f + sin(tiempo * 5) * 0.3f;
    
    switch(index) {
        case 0: // Sumar/Restar
            DrawCircle(x + 8, y + 12, 6, colorIcono);
            DrawRectangle(x + 4, y + 6, 8, 12, colorIcono);
            break;
        case 1: // Multiplicar
            DrawCircle(x + 8, y + 12, 6, colorIcono);
            DrawLine(x + 4, y + 6, x + 12, y + 18, colorIcono);
            DrawLine(x + 12, y + 6, x + 4, y + 18, colorIcono);
            break;
        case 2: // Dividir
            DrawCircle(x + 8, y + 12, 6, colorIcono);
            DrawRectangle(x + 4, y + 8, 8, 1, colorIcono);
            DrawCircle(x + 8, y + 6, 1, colorIcono);
            DrawCircle(x + 8, y + 18, 1, colorIcono);
            break;
        case 3: // Distancia Planetas
            DrawCircle(x + 8, y + 12, 8, Fade(temaActual->naranja, pulso));
            DrawCircleLines(x + 8, y + 12, 8, colorIcono);
            DrawCircle(x + 4, y + 8, 2, Fade(WHITE, 0.8f));
            break;
        case 4: // Fibonacci
            for(int i = 0; i < 3; i++) {
                DrawCircle(x + 4 + i*6, y + 12, 2 + i, Fade(colorIcono, 0.7f + i*0.1f));
            }
            break;
        case 5: // Par/Impar
            if((int)(tiempo * 2) % 2 == 0) {
                DrawCircle(x + 8, y + 12, 6, colorIcono); // Par - círculo
            } else {
                DrawRectangle(x + 4, y + 6, 8, 12, colorIcono); // Impar - cuadrado
            }
            break;
        case 6: // Factorial
            DrawText("!", x, y, 20, colorIcono);
            break;
        case 7: // Potencia
            DrawCircle(x + 8, y + 8, 4, colorIcono);
            DrawCircle(x + 12, y + 16, 3, Fade(colorIcono, 0.7f));
            break;
        case 8: // Historial
            DrawRectangle(x + 4, y + 6, 10, 2, colorIcono);
            DrawRectangle(x + 4, y + 10, 10, 2, colorIcono);
            DrawRectangle(x + 4, y + 14, 10, 2, colorIcono);
            break;
        case 9: // Cambiar Tema
            DrawCircle(x + 8, y + 12, 6, Fade(temaActual->purpura, pulso));
            DrawCircleLines(x + 8, y + 12, 6, colorIcono);
            break;
        case 10: // Salir
            DrawCircle(x + 8, y + 12, 6, Fade(temaActual->rojo, pulso));
            DrawLine(x + 5, y + 9, x + 11, y + 15, WHITE);
            DrawLine(x + 11, y + 9, x + 5, y + 15, WHITE);
            break;
    }
}

// ======================================================
// PANTALLA INICIAL MEJORADA (SIN EMOJIS)
// ======================================================

void pantallaInicial() {
    string nombre = "";
    bool activo = true;
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;
    
    while(activo && !WindowShouldClose()) {
        // Actualizar música de fondo
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        
        // Fondo animado mejorado
        dibujarFondoAnimado();
        
        // Aplicar animación de entrada
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.02f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        // Título con animación (sin emojis)
        dibujarTituloDecorado("CALCULADORA MOYA", 220, 80, 50, Fade(temaActual->acento, animacionAlpha));
        
        dibujarTextoConSombra("Por favor, ingresa tu nombre:", 260, 180, 28, Fade(temaActual->texto, animacionAlpha));
        
        // Campo de entrada mejorado
        Rectangle campo = {150, 230, 600, 70};
        dibujarTarjetaConSombra(campo, Fade(temaActual->tarjeta, animacionAlpha), 0.2f);
        
        // Efecto de focus
        if(!nombre.empty()) {
            DrawRectangleRoundedLinesEx(campo, 0.2f, 8, 3, Fade(temaActual->verde, animacionAlpha));
        }
        
        DrawText(nombre.c_str(), 170, 250, 32, Fade(temaActual->texto, animacionAlpha));
        
        // Placeholder visual
        if(nombre.empty()) {
            DrawText("Escribe tu nombre aqui...", 170, 250, 32, Fade(temaActual->texto, 0.4f * animacionAlpha));
        }
        
        // Botón continuar mejorado
        Rectangle btnContinuar = {350, 330, 200, 70};
        bool sobreBtn = CheckCollisionPointRec(GetMousePosition(), btnContinuar);

        // Manejo de entrada de texto (nombre)
        int key = 0;
        while ((key = GetCharPressed()) > 0) {
            if ((key >= 32) && (key <= 125) && nombre.size() < 32) {
                nombre.push_back((char)key);
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !nombre.empty()) {
            nombre.pop_back();
            reproducirSonido(clickSound);
        }

        // Botón continuar o ENTER
        if (botonSimple(btnContinuar, "CONTINUAR") || (IsKeyPressed(KEY_ENTER) && !nombre.empty())) {
            if(!nombre.empty()) {
                userName = nombre;
                appendSectionHeader(userName);
                activo = false;
                reproducirSonido(successSound);
            }
        }

        dibujarTextoConSombra("Presiona ENTER o haz clic en CONTINUAR", 260, 420, 20, Fade(temaActual->texto, 0.7f * animacionAlpha));

        EndDrawing();
    }
}

// ======================================================
// FONDO ANIMADO MEJORADO CON ESTÉTICA ESPACIAL
// ======================================================

void dibujarFondoAnimado() {
    float tiempo = GetTime();
    
    // Estrellas fijas de fondo
    for(int i = 0; i < 100; i++) {
        float x = (i * 37) % 900;
        float y = (i * 23) % 600;
        float brillo = 0.3f + sin(tiempo * 0.5 + i) * 0.2f;
        DrawPixel(x, y, Fade(temaActual->texto, brillo * 0.3f));
    }
    
    // Capa 1: Planetas y estrellas grandes
    for(int i = 0; i < 15; i++) {
        float x = 100 + (i * 60) + sin(tiempo * 0.3 + i * 0.7) * 50;
        float y = 80 + cos(tiempo * 0.4 + i * 0.5) * 40;
        float radio = 3 + sin(tiempo * 0.2 + i) * 1.5;
        
        Color particulaColor;
        switch(i % 5) {
            case 0: particulaColor = Fade(temaActual->acento, 0.15f); break;
            case 1: particulaColor = Fade(temaActual->verde, 0.12f); break;
            case 2: particulaColor = Fade(temaActual->purpura, 0.1f); break;
            case 3: particulaColor = Fade(temaActual->naranja, 0.08f); break;
            default: particulaColor = Fade(WHITE, 0.1f); break;
        }
        DrawCircle(x, y, radio, particulaColor);
        
        // Anillos para algunos
        if(i % 3 == 0) {
            DrawRing({x, y}, radio + 2, radio + 4, 0, 360, 3, Fade(particulaColor, 0.3f));
        }
    }
    
    // Capa 2: Nebulosas grandes
    for(int i = 0; i < 6; i++) {
        float x = 200 + (i * 150) + cos(tiempo * 0.2 + i) * 80;
        float y = 400 + sin(tiempo * 0.3 + i * 0.8) * 60;
        float radio = 25 + sin(tiempo * 0.1 + i) * 8;
        
        switch(i % 3) {
            case 0: DrawCircleGradient(x, y, radio, Fade(temaActual->acento, 0.04f), Fade(temaActual->purpura, 0.01f)); break;
            case 1: DrawCircleGradient(x, y, radio, Fade(temaActual->verde, 0.03f), Fade(temaActual->acento, 0.01f)); break;
            default: DrawCircleGradient(x, y, radio, Fade(temaActual->naranja, 0.02f), Fade(temaActual->rojo, 0.005f)); break;
        }
    }
    
    // Capa 3: Líneas de conexión (constelaciones)
    for(int i = 0; i < 10; i += 2) {
        float x1 = 50 + (i * 90) + sin(tiempo * 0.5 + i) * 30;
        float y1 = 100 + cos(tiempo * 0.4 + i) * 35;
        float x2 = 50 + ((i+1) * 90) + sin(tiempo * 0.5 + (i+1)) * 30;
        float y2 = 100 + cos(tiempo * 0.4 + (i+1)) * 35;
        DrawLineEx({x1, y1}, {x2, y2}, 1.0f, Fade(temaActual->acento, 0.08f));
    }

    // Capa 4: Formas matemáticas animadas mejoradas
    for(int i = 0; i < 10; i++) {
        float x = 150 + (i * 80) + sin(tiempo * 0.4 + i * 0.7) * 70;
        float y = 180 + cos(tiempo * 0.5 + i * 0.8) * 60;
        float width = 18 + sin(tiempo * 0.3 + i) * 8;
        float height = 24 + cos(tiempo * 0.35 + i) * 10;
        float rotation = tiempo * 0.6 + i * 36.0f;

        Rectangle rect = {x, y, width, height};
        Color rectColor;
        float alpha = 0.08f + sin(tiempo * 0.8 + i) * 0.03f;
        switch(i % 5) {
            case 0: rectColor = Fade(temaActual->verde, alpha); break;
            case 1: rectColor = Fade(temaActual->purpura, alpha); break;
            case 2: rectColor = Fade(temaActual->naranja, alpha); break;
            case 3: rectColor = Fade(temaActual->acento, alpha); break;
            default: rectColor = Fade(temaActual->rojo, alpha); break;
        }
        DrawRectanglePro(rect, {width/2, height/2}, rotation, rectColor);
    }

    // Triángulos escalando y rotando representando geometría
    for(int i = 0; i < 8; i++) {
        float centerX = 250 + (i * 100) + cos(tiempo * 0.25 + i) * 80;
        float centerY = 320 + sin(tiempo * 0.4 + i * 0.9) * 70;
        float size = 15 + sin(tiempo * 0.5 + i) * 8;
        float rotation = tiempo * 0.4 + i * 45.0f;

        // Calcular vértices del triángulo equilátero
        float angle1 = rotation * DEG2RAD;
        float angle2 = (rotation + 120) * DEG2RAD;
        float angle3 = (rotation + 240) * DEG2RAD;

        Vector2 v1 = {(float)(centerX + cos(angle1) * size), (float)(centerY + sin(angle1) * size)};
        Vector2 v2 = {(float)(centerX + cos(angle2) * size), (float)(centerY + sin(angle2) * size)};
        Vector2 v3 = {(float)(centerX + cos(angle3) * size), (float)(centerY + sin(angle3) * size)};

        Color triangleColor;
        float alpha = 0.08f + cos(tiempo * 0.7 + i) * 0.04f;
        switch(i % 4) {
            case 0: triangleColor = Fade(temaActual->verde, alpha); break;
            case 1: triangleColor = Fade(temaActual->purpura, alpha); break;
            case 2: triangleColor = Fade(temaActual->naranja, alpha); break;
            default: triangleColor = Fade(temaActual->acento, alpha); break;
        }
        DrawTriangle(v1, v2, v3, triangleColor);
    }

    DrawCircle(80, 80, 60, Fade(temaActual->acento, 0.05f));
    DrawCircle(820, 520, 80, Fade(temaActual->verde, 0.04f));
    DrawCircle(750, 120, 55, Fade(temaActual->purpura, 0.06f));
    
    // Cometas ocasionales
    if((int)(tiempo * 0.3) % 20 == 0) {
        float cometX = 900 - fmod(tiempo * 100, 1200);
        float cometY = 100 + sin(tiempo) * 50;
        DrawCircle(cometX, cometY, 4, YELLOW);
        DrawLine(cometX, cometY, cometX - 20, cometY - 5, Fade(YELLOW, 0.5f));
    }
}

// ======================================================
// BOTÓN SIMPLE MEJORADO
// ======================================================

bool botonSimple(Rectangle r, const char *text) {
    bool over = CheckCollisionPointRec(GetMousePosition(), r);
    Color colorBoton = over ? temaActual->verde : temaActual->acento;
    
    // Efecto de elevación
    float elevacion = over ? 2.0f : 0.0f;
    
    // Sombra
    Rectangle btnShadow = {r.x, r.y + 3, r.width, r.height};
    if(over) {
        DrawRectangleRounded(btnShadow, 0.3f, 12, Fade(BLACK, 0.2f));
    }
    
    // Botón principal
    DrawRectangleRounded({r.x, r.y - elevacion, r.width, r.height}, 0.3f, 12, colorBoton);
    
    // Borde con efecto de brillo
    DrawRectangleRoundedLinesEx({r.x, r.y - elevacion, r.width, r.height}, 0.3f, 12, 2, 
                               over ? WHITE : Fade(WHITE, 0.7f));
    
    // Texto con efecto de sombra
    int textWidth = MeasureText(text, 26);
    dibujarTextoConSombra(text, r.x + (r.width - textWidth)/2, r.y - elevacion + 17, 26, WHITE);
    
    return (over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

// ======================================================
// TARJETA CON SOMBRA
// ======================================================

void dibujarTarjetaConSombra(Rectangle rect, Color color, float redondeo) {
    // Sombra
    Rectangle shadowRect = {rect.x + 4, rect.y + 4, rect.width, rect.height};
    DrawRectangleRounded(shadowRect, redondeo, 12, Fade(BLACK, 0.15f));
    
    // Tarjeta principal
    DrawRectangleRounded(rect, redondeo, 12, color);
    DrawRectangleRoundedLinesEx(rect, redondeo, 12, 2, Fade(WHITE, 0.1f));
}

// ======================================================
// TEXTO CON SOMBRA
// ======================================================

void dibujarTextoConSombra(const char* texto, int x, int y, int fontSize, Color color) {
    DrawText(texto, x + 1, y + 1, fontSize, Fade(BLACK, 0.3f));
    DrawText(texto, x, y, fontSize, color);
}

// ======================================================
// NUEVAS FUNCIONES AUXILIARES
// ======================================================

void dibujarIndicadorEstado(int x, int y, const char* texto, Color color) {
    // Círculo indicador pulsante
    float pulso = 0.7f + sin(GetTime() * 5) * 0.3f;
    DrawCircle(x, y, 6, Fade(color, pulso));
    DrawText(TextFormat("%s", texto), x + 15, y - 8, 18, color);
}

void mostrarResultadoBonito(int x, int y, const string& operacion, const string& resultado) {
    Rectangle resultadoRect = {(float)x-20, (float)y-15, 450, 80};
    dibujarTarjetaConSombra(resultadoRect, Fade(temaActual->verde, 0.1f), 0.1f);
    
    // Operación
    dibujarTextoConSombra(operacion.c_str(), x, y, 22, temaActual->texto);
    
    // Signo igual
    int opWidth = MeasureText(operacion.c_str(), 22);
    dibujarTextoConSombra("=", x + opWidth + 10, y, 24, temaActual->acento);
    
    // Resultado
    dibujarTextoConSombra(resultado.c_str(), x + opWidth + 35, y, 26, temaActual->verde);
    
    // Efecto de éxito
    mostrarEfectoExito(x + opWidth + 35 + MeasureText(resultado.c_str(), 26)/2, y + 10);
}

void mostrarTooltip(const char* texto, int x, int y) {
    int padding = 10;
    int textWidth = MeasureText(texto, 18);
    Rectangle tooltipRect = {(float)x, (float)y, (float)(textWidth + padding * 2), 30};

    dibujarTarjetaConSombra(tooltipRect, Fade(BLACK, 0.85f), 0.1f);
    DrawText(texto, x + padding, y + 5, 18, WHITE);
}

void mostrarEfectoExito(int x, int y) {
    float tiempo = GetTime();
    for(int i = 0; i < 8; i++) {
        float angle = tiempo * 2 + i * (360/8) * DEG2RAD;
        float dist = 10 + sin(tiempo * 8 + i) * 5;
        float px = x + cos(angle) * dist;
        float py = y + sin(angle) * dist;
        float radio = 2 + sin(tiempo * 10 + i) * 1;
        DrawCircle(px, py, radio, Fade(temaActual->verde, 0.7f));
    }
}

void mostrarEfectoError(int x, int y) {
    float tiempo = GetTime();
    for(int i = 0; i < 6; i++) {
        float angle = i * (360/6) * DEG2RAD;
        float dist = 8 + sin(tiempo * 15) * 3;
        float px = x + cos(angle) * dist;
        float py = y + sin(angle) * dist;
        DrawCircle(px, py, 3, Fade(temaActual->rojo, 0.8f));
    }
}

void cambiarTema() {
    reproducirSonido(clickSound);
    if(temaActual == &temaEspacial) {
        temaActual = &temaOscuro;
    } else if(temaActual == &temaOscuro) {
        temaActual = &temaNeon;
    } else {
        temaActual = &temaEspacial;
    }
}

void reproducirSonido(Sound sonido) {
    if(IsAudioDeviceReady()) {
        PlaySound(sonido);
    }
}

bool esNumeroValido(const string& str) {
    if(str.empty()) return false;
    if(str == "-" || str == "." || str == "-.") return false;
    if(str.find("--") != string::npos) return false;
    if(count(str.begin(), str.end(), '.') > 1) return false;
    return true;
}

// ======================================================
// TECLADO NUMÉRICO SUPER MEJORADO (SIN EMOJIS)
// ======================================================

void tecladoNumerico(string &buffer, int x, int y) {
    // Responsive keypad layout: compute button sizes and gaps so it fits on screen
    const char *nums[9] = {"1","2","3","4","5","6","7","8","9"};
    int idx = 0;

    int scrW = GetScreenWidth();
    int scrH = GetScreenHeight();

    int cols = 3;
    int rows = 5; // 3x3 + row for 0/. /- + row for controls

    // preferred sizes (more compact and centered)
    int btnW = 52;
    int btnH = 44;
    int hGap = 6;
    int vGap = 6;

    // calculate total size
    int totalW = cols * btnW + (cols - 1) * hGap;
    int totalH = rows * btnH + (rows - 1) * vGap;

    // Center horizontally and clamp vertically so keypad doesn't overflow
    int centerX = (scrW - totalW) / 2;
    if (centerX < 20) centerX = 20;
    x = centerX; // center more consistently

    int maxY = scrH - totalH - 40;
    if (y > maxY) y = maxY;
    if (y < 140) y = 140; // don't go too high over header area

    for(int j=0;j<3;j++) {
        for(int i=0;i<3;i++) {
            Rectangle r = { (float)(x + i*(btnW + hGap)), (float)(y + j*(btnH + vGap)), (float)btnW, (float)btnH };
            bool over = CheckCollisionPointRec(GetMousePosition(), r);
            Color colorBoton = over ? Fade(temaActual->verde,0.9f) : Fade(temaActual->acento,0.7f);
            
            // Sombra del botón
            if(over) {
                DrawRectangleRounded({r.x, r.y+3, r.width, r.height}, 0.3f, 8, Fade(BLACK, 0.2f));
                mostrarTooltip("Numero", r.x, r.y - 25);
            }
            
            // Botón principal
            float elevacion = over ? 2.0f : 0.0f;
            DrawRectangleRounded({r.x, r.y - elevacion, r.width, r.height}, 0.3f, 8, colorBoton);
            DrawRectangleRoundedLinesEx({r.x, r.y - elevacion, r.width, r.height}, 0.3f, 8, 2, 
                                       over ? WHITE : Fade(WHITE, 0.6f));
            
            // Texto del número con decoración circular (compact)
            DrawCircle(r.x + btnW/2.0f, r.y - elevacion + 18, 10, Fade(WHITE, 0.9f));
            dibujarTextoConSombra(nums[idx], (int)(r.x + btnW/2.0f - MeasureText(nums[idx],16)/2), (int)(r.y - elevacion + 26), 16, colorBoton);
            
            if(over && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                buffer += nums[idx];
                reproducirSonido(beepSound);
            }
            idx++;
        }
    }
    
    // Fila 4: Cero, punto, negativo
    // Cero
    Rectangle r0 = { (float)(x + (btnW + hGap) * 1), (float)(y + 3*(btnH + vGap)), (float)btnW, (float)btnH };
    bool over0 = CheckCollisionPointRec(GetMousePosition(), r0);
    Color colorBoton0 = over0 ? Fade(temaActual->verde,0.9f) : Fade(temaActual->acento,0.7f);
    
    if(over0) {
        DrawRectangleRounded({r0.x, r0.y+3, r0.width, r0.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Cero", r0.x, r0.y - 25);
    }
    
    float elevacion0 = over0 ? 2.0f : 0.0f;
    DrawRectangleRounded({r0.x, r0.y - elevacion0, r0.width, r0.height}, 0.3f, 8, colorBoton0);
    DrawRectangleRoundedLinesEx({r0.x, r0.y - elevacion0, r0.width, r0.height}, 0.3f, 8, 2, 
                               over0 ? WHITE : Fade(WHITE, 0.6f));
    DrawCircle(r0.x + btnW/2.0f, r0.y - elevacion0 + 18, 10, Fade(WHITE, 0.9f));
    dibujarTextoConSombra("0", (int)(r0.x + btnW/2.0f - MeasureText("0",16)/2), (int)(r0.y - elevacion0 + 26), 16, colorBoton0);
    
    if(over0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buffer += "0";
        reproducirSonido(beepSound);
    }
    
    // Punto decimal
    Rectangle rDot = { (float)(x + (btnW + hGap) * 2), (float)(y + 3*(btnH + vGap)), (float)btnW, (float)btnH };
    bool overDot = CheckCollisionPointRec(GetMousePosition(), rDot);
    Color colorBotonDot = overDot ? Fade(temaActual->naranja,0.9f) : Fade(temaActual->naranja,0.7f);
    
    if(overDot) {
        DrawRectangleRounded({rDot.x, rDot.y+3, rDot.width, rDot.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Punto decimal", rDot.x, rDot.y - 25);
    }
    
    float elevacionDot = overDot ? 2.0f : 0.0f;
    DrawRectangleRounded({rDot.x, rDot.y - elevacionDot, rDot.width, rDot.height}, 0.3f, 8, colorBotonDot);
    DrawRectangleRoundedLinesEx({rDot.x, rDot.y - elevacionDot, rDot.width, rDot.height}, 0.3f, 8, 2, 
                               overDot ? WHITE : Fade(WHITE, 0.6f));
    DrawCircle(rDot.x + btnW/2.0f, rDot.y - elevacionDot + 18, 9, Fade(WHITE, 0.9f));
    dibujarTextoConSombra(".", (int)(rDot.x + btnW/2.0f - MeasureText(".",16)/2), (int)(rDot.y - elevacionDot + 26), 16, colorBotonDot);
    
    if(overDot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && buffer.find('.') == string::npos) {
        buffer += ".";
        reproducirSonido(beepSound);
    }
    
    // Signo negativo
    Rectangle rNeg = { (float)x, (float)(y + 3*(btnH + vGap)), (float)btnW, (float)btnH };
    bool overNeg = CheckCollisionPointRec(GetMousePosition(), rNeg);
    Color colorBotonNeg = overNeg ? Fade(temaActual->purpura,0.9f) : Fade(temaActual->purpura,0.7f);
    
    if(overNeg) {
        DrawRectangleRounded({rNeg.x, rNeg.y+3, rNeg.width, rNeg.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Signo negativo", rNeg.x, rNeg.y - 25);
    }
    
    float elevacionNeg = overNeg ? 2.0f : 0.0f;
    DrawRectangleRounded({rNeg.x, rNeg.y - elevacionNeg, rNeg.width, rNeg.height}, 0.3f, 8, colorBotonNeg);
    DrawRectangleRoundedLinesEx({rNeg.x, rNeg.y - elevacionNeg, rNeg.width, rNeg.height}, 0.3f, 8, 2, 
                               overNeg ? WHITE : Fade(WHITE, 0.6f));
    dibujarTextoConSombra("-", (int)(rNeg.x + btnW/2.0f - MeasureText("-",20)/2), (int)(rNeg.y - elevacionNeg + 28), 20, WHITE);
    
    if(overNeg && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if(buffer.empty() || (buffer.length() > 0 && buffer[0] != '-')) {
            buffer = "-" + buffer;
            reproducirSonido(beepSound);
        }
    }
    
    // Fila 5: Borrar, borrar todo, enter
    // Borrar (backspace)
    Rectangle rb = { (float)x, (float)(y + 4*(btnH + vGap)), (float)btnW, (float)btnH };
    bool overB = CheckCollisionPointRec(GetMousePosition(), rb);
    Color colorBotonB = overB ? Fade(temaActual->rojo,0.9f) : Fade(temaActual->rojo,0.7f);
    
    if(overB) {
        DrawRectangleRounded({rb.x, rb.y+3, rb.width, rb.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Borrar ultimo caracter", rb.x, rb.y - 25);
    }
    
    float elevacionB = overB ? 2.0f : 0.0f;
    DrawRectangleRounded({rb.x, rb.y - elevacionB, rb.width, rb.height}, 0.3f, 8, colorBotonB);
    DrawRectangleRoundedLinesEx({rb.x, rb.y - elevacionB, rb.width, rb.height}, 0.3f, 8, 2, 
                               overB ? WHITE : Fade(WHITE, 0.6f));
    dibujarTextoConSombra("<-", (int)(rb.x + btnW/2.0f - MeasureText("<-",16)/2), (int)(rb.y - elevacionB + 26), 16, WHITE);
    
    if(overB && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !buffer.empty()) {
        buffer.pop_back();
        reproducirSonido(clickSound);
    }
    
    // Borrar todo (clear)
    Rectangle rClear = { (float)(x + (btnW + hGap) * 1), (float)(y + 4*(btnH + vGap)), (float)btnW, (float)btnH };
    bool overClear = CheckCollisionPointRec(GetMousePosition(), rClear);
    Color colorBotonClear = overClear ? Fade(temaActual->rojo,0.9f) : Fade(temaActual->rojo,0.7f);
    
    if(overClear) {
        DrawRectangleRounded({rClear.x, rClear.y+3, rClear.width, rClear.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Borrar todo", rClear.x, rClear.y - 25);
    }
    
    float elevacionClear = overClear ? 2.0f : 0.0f;
    DrawRectangleRounded({rClear.x, rClear.y - elevacionClear, rClear.width, rClear.height}, 0.3f, 8, colorBotonClear);
    DrawRectangleRoundedLinesEx({rClear.x, rClear.y - elevacionClear, rClear.width, rClear.height}, 0.3f, 8, 2, 
                               overClear ? WHITE : Fade(WHITE, 0.6f));
    dibujarTextoConSombra("C", (int)(rClear.x + btnW/2.0f - MeasureText("C",16)/2), (int)(rClear.y - elevacionClear + 26), 16, WHITE);
    
    if(overClear && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        buffer.clear();
        reproducirSonido(clickSound);
    }
    
    // Enter
    Rectangle rEnter = { (float)(x + (btnW + hGap) * 2), (float)(y + 4*(btnH + vGap)), (float)btnW, (float)btnH };
    bool overEnter = CheckCollisionPointRec(GetMousePosition(), rEnter);
    Color colorBotonEnter = overEnter ? Fade(temaActual->verde,0.9f) : Fade(temaActual->verde,0.7f);
    
    if(overEnter) {
        DrawRectangleRounded({rEnter.x, rEnter.y+3, rEnter.width, rEnter.height}, 0.3f, 8, Fade(BLACK, 0.2f));
        mostrarTooltip("Confirmar entrada", rEnter.x, rEnter.y - 25);
    }
    
    float elevacionEnter = overEnter ? 2.0f : 0.0f;
    DrawRectangleRounded({rEnter.x, rEnter.y - elevacionEnter, rEnter.width, rEnter.height}, 0.3f, 8, colorBotonEnter);
    DrawRectangleRoundedLinesEx({rEnter.x, rEnter.y - elevacionEnter, rEnter.width, rEnter.height}, 0.3f, 8, 2, 
                               overEnter ? WHITE : Fade(WHITE, 0.6f));
    dibujarTextoConSombra("OK", (int)(rEnter.x + btnW/2.0f - MeasureText("OK",16)/2), (int)(rEnter.y - elevacionEnter + 26), 16, WHITE);
    
    if(overEnter && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // El enter se maneja en las funciones de cada operación
        reproducirSonido(successSound);
    }

    // --- Keyboard hardware support: detect numeric keys and controls ---
    // Digits top row
    if (IsKeyPressed(KEY_ZERO)) { buffer += "0"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_ONE)) { buffer += "1"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_TWO)) { buffer += "2"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_THREE)) { buffer += "3"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_FOUR)) { buffer += "4"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_FIVE)) { buffer += "5"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_SIX)) { buffer += "6"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_SEVEN)) { buffer += "7"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_EIGHT)) { buffer += "8"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_NINE)) { buffer += "9"; reproducirSonido(beepSound); }
    // Numpad
    if (IsKeyPressed(KEY_KP_0)) { buffer += "0"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_1)) { buffer += "1"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_2)) { buffer += "2"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_3)) { buffer += "3"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_4)) { buffer += "4"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_5)) { buffer += "5"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_6)) { buffer += "6"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_7)) { buffer += "7"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_8)) { buffer += "8"; reproducirSonido(beepSound); }
    if (IsKeyPressed(KEY_KP_9)) { buffer += "9"; reproducirSonido(beepSound); }
    // Decimal / period
    if (IsKeyPressed(KEY_PERIOD) || IsKeyPressed(KEY_KP_DECIMAL)) {
        if (buffer.find('.') == string::npos) { buffer += "."; reproducirSonido(beepSound); }
    }
    // Minus sign (toggle)
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
        if(buffer.empty() || (buffer.length() > 0 && buffer[0] != '-')) {
            buffer = string("-") + buffer;
            reproducirSonido(beepSound);
        }
    }
    // Backspace
    if (IsKeyPressed(KEY_BACKSPACE) && !buffer.empty()) { buffer.pop_back(); reproducirSonido(clickSound); }
    // Enter handled by each screen (IsKeyPressed(KEY_ENTER))
}

// ======================================================
// FUNCIONES DE OPERACIONES COMPLETAS (ACTUALIZADAS)
// ======================================================

void pantallaSumarRestar() {
    string op = "";
    string num1 = "", num2 = "";
    bool choose = true, input1=false, input2=false, done=false;
    string resultado = "";
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;
    
    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        // Aplicar animación
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("SUMAR / RESTAR", 280, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(choose) {
            dibujarTextoConSombra("Elige operacion:", 350, 150, 24, Fade(temaActual->texto, animacionAlpha));
            
            Rectangle addR = {300,220,140,70};
            Rectangle resR = {460,220,140,70};
            
            if (botonSimple(addR, "SUMA (+)")) {
                op = "+";
                choose=false;
                input1=true;
                reproducirSonido(clickSound);
            }
            if (botonSimple(resR, "RESTA (-)")) {
                op = "-";
                choose=false;
                input1=true;
                reproducirSonido(clickSound);
            }
        } else if(input1) {
            // Adjusted layout: subtitle lower to avoid overlap with indicators
            // Indicator (left) and subtitle separated vertically for clarity
            dibujarIndicadorEstado(180, 150, "Ingresando primer numero", temaActual->verde);
            dibujarTextoConSombra("Primer numero:", 350, 190, 22, Fade(temaActual->texto, animacionAlpha));

            // Mostrar operación parcial (moved below subtitle)
            if(!num1.empty()) {
                string operacionParcial = num1 + " " + op + " ?";
                dibujarTextoConSombra(operacionParcial.c_str(), 350, 220, 18, Fade(temaActual->texto, 0.6f));
            }

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num1.c_str(), 220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num1, 300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num1)) {
                input1=false;
                input2=true;
                reproducirSonido(successSound);
            }
        } else if(input2) {
            dibujarIndicadorEstado(180, 150, "Ingresando segundo numero", temaActual->naranja);
            dibujarTextoConSombra("Segundo numero:", 350, 190, 22, Fade(temaActual->texto, animacionAlpha));

            // Mostrar operación parcial
            string operacionParcial = num1 + " " + op + " ?";
            dibujarTextoConSombra(operacionParcial.c_str(), 350, 220, 18, Fade(temaActual->texto, 0.6f));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num2.c_str(), 220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num2, 300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num2)) {
                float a = stof(num1);
                float b = stof(num2);
                float r = (op == "+") ? a+b : a-b;
                char buf[64];
                sprintf(buf,"%.4f",r);
                resultado = buf;
                string h = num1 + " " + op + " " + num2 + " = " + buf;
                historial.push_back(h);
                appendHistorialLine(h);
                input2 = false;
                done = true;
                reproducirSonido(successSound);
            }
        } else if(done) {
            string operacionCompleta = num1 + " " + op + " " + num2;
            mostrarResultadoBonito(250, 220, operacionCompleta, resultado);
            
            dibujarTextoConSombra("ENTER -> volver", 350, 350, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20, 20, 18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaMultiplicar() {
    string num1="", num2="";
    bool input1=true, input2=false, done=false;
    string resultado = "";
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;
    
    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("MULTIPLICAR", 320, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input1) {
            dibujarIndicadorEstado(180, 150, "Ingresando primer numero", temaActual->verde);
            dibujarTextoConSombra("Primer numero:", 350, 190, 22, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num1.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num1,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num1)) {
                input1=false;
                input2=true;
                reproducirSonido(successSound);
            }
        } else if(input2) {
            dibujarIndicadorEstado(180, 150, "Ingresando segundo numero", temaActual->naranja);
            dibujarTextoConSombra("Segundo numero:", 350, 190, 22, Fade(temaActual->texto, animacionAlpha));

            // Mostrar operación parcial
            string operacionParcial = num1 + " x ?";
            dibujarTextoConSombra(operacionParcial.c_str(), 350, 220, 18, Fade(temaActual->texto, 0.6f));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num2.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num2,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num2)) {
                float a = stof(num1);
                float b = stof(num2);
                float r = a*b;
                char buf[64];
                sprintf(buf,"%.4f", r);
                resultado = buf;
                string h = num1 + " x " + num2 + " = " + buf;
                historial.push_back(h);
                appendHistorialLine(h);
                input2 = false;
                done = true;
                reproducirSonido(successSound);
            }
        } else if(done) {
            string operacionCompleta = num1 + " x " + num2;
            mostrarResultadoBonito(250, 220, operacionCompleta, resultado);
            
            dibujarTextoConSombra("ENTER -> volver", 350, 350, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaDividir() {
    string num1="", num2="";
    bool input1=true, input2=false, done=false;
    string resultado = "";
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;
    
    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("DIVIDIR", 370, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input1) {
            // Align with other screens: increase spacing and center keypad
            dibujarIndicadorEstado(180, 150, "Ingresando dividendo", temaActual->verde);
            dibujarTextoConSombra("Dividendo:", 350, 190, 24, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num1.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num1,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num1)) {
                input1=false;
                input2=true;
                reproducirSonido(successSound);
            }
        } else if(input2) {
            dibujarIndicadorEstado(180, 150, "Ingresando divisor", temaActual->naranja);
            dibujarTextoConSombra("Divisor:", 350, 190, 24, Fade(temaActual->texto, animacionAlpha));

            string operacionParcial = num1 + " / ?";
            dibujarTextoConSombra(operacionParcial.c_str(), 350, 220, 18, Fade(temaActual->texto, 0.6f));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num2.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num2,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num2)) {
                float a=stof(num1), b=stof(num2);
                if(b==0) {
                    resultado = "ERROR: Division por cero";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                } else {
                    float r=a/b;
                    char buf[64];
                    sprintf(buf,"%.4f",r);
                    resultado = buf;
                    string h = num1 + " / " + num2 + " = " + buf;
                    historial.push_back(h);
                    appendHistorialLine(h);
                    reproducirSonido(successSound);
                }
                input2 = false;
                done = true;
            }
        } else if(done) {
            if(resultado.find("ERROR") != string::npos) {
                dibujarTextoConSombra("Resultado:", 300, 220, 26, Fade(temaActual->rojo, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 300, 250, 24, Fade(temaActual->rojo, animacionAlpha));
            } else {
                string operacionCompleta = num1 + " / " + num2;
                mostrarResultadoBonito(250, 220, operacionCompleta, resultado);
            }
            
            dibujarTextoConSombra("ENTER -> volver", 350, 350, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaDistanciaPlanetas() {
    struct Planet {
        string name;
        float dist;
        Color color;
    };
    
    vector<Planet> P = {
        {"Sol", 149.6f, YELLOW},
        {"Mercurio", 91.7f, GRAY},
        {"Venus", 41.4f, ORANGE},
        {"Marte", 78.3f, RED},
        {"Jupiter", 628.7f, {255, 200, 100, 255}},
        {"Saturno", 1275.0f, {210, 180, 140, 255}},
        {"Urano", 2723.0f, {100, 200, 255, 255}},
        {"Neptuno", 4351.0f, {50, 100, 255, 255}}
    };
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;

    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("DISTANCIA DESDE LA TIERRA", 180, 40, 34, Fade(temaActual->acento, animacionAlpha));
        dibujarTextoConSombra("(millones de kilometros)", 320, 80, 20, Fade(temaActual->texto, 0.8f * animacionAlpha));
        
        Rectangle contenido = {80, 120, 740, 430};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);

        float y = 150.0f;
        for(auto &p : P) {
            char buf[64];
            sprintf(buf,"%.1f", p.dist);
            
            // Icono del planeta
            DrawCircle(120, y + 15, 8, Fade(p.color, animacionAlpha));
            if(p.name == "Saturno") {
                DrawRing({120, y + 15}, 10, 13, 0, 360, 2, Fade(WHITE, 0.6f * animacionAlpha));
            }
            
            // Nombre del planeta
            DrawText(p.name.c_str(), 140, (int)y, 24, Fade(temaActual->texto, animacionAlpha));

            // Distancia
            DrawText(buf, 500, (int)y, 24, Fade(temaActual->verde, animacionAlpha));
            DrawText("millones km", 580, (int)y, 20, Fade(temaActual->texto, 0.7f * animacionAlpha));

            // Barra visual de distancia
            float barWidth = (p.dist / 4351.0f) * 150.0f;
            DrawRectangle(650, (int)(y + 10), (int)barWidth, 8, Fade(p.color, 0.7f * animacionAlpha));
            DrawRectangleLines(650, (int)(y + 10), 150, 8, Fade(temaActual->texto, 0.3f * animacionAlpha));
            
            y += 45;
        }
        
        // Información adicional
        dibujarTextoConSombra("La Tierra esta a 149.6 millones de km del Sol", 200, 500, 18, 
                             Fade(temaActual->texto, 0.7f * animacionAlpha));
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20, 20, 18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaFibonacci() {
    string num = "";
    bool input = true, done = false;
    string resultado = "";
    vector<long long> secuencia;
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;

    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("SECUENCIA FIBONACCI", 280, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input) {
            dibujarIndicadorEstado(180, 150, "Ingresando numero de terminos", temaActual->verde);
            dibujarTextoConSombra("Numero de terminos:", 330, 190, 24, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num)) {
                int n = stoi(num);
                if(n <= 0) {
                    resultado = "Error: debe ser > 0";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                } else if(n > 50) {
                    resultado = "Error: maximo 50 terminos";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                } else {
                    // Generar secuencia Fibonacci
                    secuencia.clear();
                    long long a = 0, b = 1;
                    for(int i = 0; i < n; i++) {
                        secuencia.push_back(a);
                        long long temp = a + b;
                        a = b;
                        b = temp;
                    }
                    
                    // Formatear resultado
                    resultado = "";
                    for(int i = 0; i < secuencia.size(); i++) {
                        resultado += to_string(secuencia[i]);
                        if(i < secuencia.size() - 1) resultado += ", ";
                    }
                    
                    string h = "Fibonacci(" + num + ") = " + 
                              (secuencia.size() > 5 ? 
                               to_string(secuencia[0]) + ",...," + to_string(secuencia.back()) :
                               resultado.substr(0, 40));
                    historial.push_back(h);
                    appendHistorialLine(h);
                    reproducirSonido(successSound);
                }
                input = false;
                done = true;
            }
        } else if(done) {
            if(resultado.find("Error") != string::npos) {
                dibujarTextoConSombra("Error:", 300, 150, 26, Fade(temaActual->rojo, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 300, 180, 24, Fade(temaActual->rojo, animacionAlpha));
            } else {
                dibujarTextoConSombra(("Secuencia Fibonacci (" + num + " terminos):").c_str(), 
                                    200, 130, 24, Fade(temaActual->acento, animacionAlpha));
                
                // Mostrar secuencia con scroll si es muy larga
                int y_pos = 170;
                string temp = resultado;
                int maxWidth = 650;
                int charCount = 0;
                string currentLine = "";
                
                for(char c : resultado) {
                    currentLine += c;
                    charCount++;
                    
                    if(charCount > 60 && c == ',') {
                        DrawText(currentLine.c_str(), 120, y_pos, 20, Fade(temaActual->texto, animacionAlpha));
                        y_pos += 25;
                        currentLine = "";
                        charCount = 0;
                    }
                }
                
                if(!currentLine.empty()) {
                    DrawText(currentLine.c_str(), 120, y_pos, 20, Fade(temaActual->texto, animacionAlpha));
                }
                
                // Patrón visual de Fibonacci
                int startX = 120;
                int startY = 350;
                int size = 8;
                for(int i = 0; i < min(10, (int)secuencia.size()); i++) {
                    int circles = static_cast<int>(min(secuencia[i], 5LL)); // Limitar para visualización
                    for(int j = 0; j < circles; j++) {
                        DrawCircle(startX + i * 50 + j * 10, startY, size,
                                  Fade(temaActual->verde, 0.3f + j * 0.1f));
                    }
                }
            }
            
            dibujarTextoConSombra("ENTER -> volver", 350, 450, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaParImpar() {
    string num = "";
    bool input = true, done = false;
    string resultado = "";
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;

    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("PAR O IMPAR", 340, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input) {
            // Increase vertical spacing to avoid overlap with subtitle and center keypad
            dibujarIndicadorEstado(180, 150, "Ingresando numero", temaActual->verde);
            dibujarTextoConSombra("Ingresa un numero:", 340, 190, 24, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            // Let tecladoNumerico center itself horizontally; pass a standard vertical offset
            tecladoNumerico(num, 300, 320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num)) {
                try {
                    int n = stoi(num);
                    if(n % 2 == 0) {
                        resultado = "PAR";
                        reproducirSonido(successSound);
                    } else {
                        resultado = "IMPAR";
                        reproducirSonido(successSound);
                    }
                    string h = num + " es " + resultado;
                    historial.push_back(h);
                    appendHistorialLine(h);
                } catch(...) {
                    resultado = "Error: numero invalido";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                }
                input = false;
                done = true;
            }
        } else if(done) {
            if(resultado.find("Error") != string::npos) {
                dibujarTextoConSombra("Error:", 300, 180, 26, Fade(temaActual->rojo, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 300, 210, 24, Fade(temaActual->rojo, animacionAlpha));
            } else {
                dibujarTextoConSombra("El numero", 300, 180, 26, Fade(temaActual->texto, animacionAlpha));
                dibujarTextoConSombra(num.c_str(), 420, 180, 26, Fade(temaActual->acento, animacionAlpha));
                dibujarTextoConSombra("es:", 480, 180, 26, Fade(temaActual->texto, animacionAlpha));
                
                Color colorResultado = (resultado == "PAR") ? temaActual->verde : temaActual->purpura;
                dibujarTextoConSombra(resultado.c_str(), 520, 180, 32, Fade(colorResultado, animacionAlpha));
                
                // Icono decorativo animado
                float tiempo = GetTime();
                float centerX = 450.0f;
                float centerY = 280.0f;
                
                if(resultado == "PAR") {
                    // Círculo para PAR
                    float pulso = 0.8f + sin(tiempo * 5) * 0.2f;
                    DrawCircle(centerX, centerY, 50, Fade(temaActual->verde, 0.2f * pulso * animacionAlpha));
                    DrawCircleLines(centerX, centerY, 50, Fade(temaActual->verde, animacionAlpha));
                    
                    // Efectos de partículas concéntricas
                    for(int i = 0; i < 8; i++) {
                        float angle = tiempo * 2 + i * (360/8) * DEG2RAD;
                        float dist = 30 + sin(tiempo * 3 + i) * 10;
                        float px = centerX + cos(angle) * dist;
                        float py = centerY + sin(angle) * dist;
                        DrawCircle(px, py, 3, Fade(temaActual->verde, 0.7f * animacionAlpha));
                    }
                } else {
                    // Cuadrado para IMPAR
                    float pulso = 0.8f + sin(tiempo * 7) * 0.2f;
                    DrawRectangle(centerX - 40, centerY - 40, 80, 80, 
                                 Fade(temaActual->purpura, 0.2f * pulso * animacionAlpha));
                    DrawRectangleLines(centerX - 40, centerY - 40, 80, 80, 
                                      Fade(temaActual->purpura, animacionAlpha));
                    
                    // Efectos de partículas en esquinas
                    Vector2 esquinas[] = {
                        {centerX - 35, centerY - 35},
                        {centerX + 35, centerY - 35},
                        {centerX + 35, centerY + 35},
                        {centerX - 35, centerY + 35}
                    };
                    
                    for(int i = 0; i < 4; i++) {
                        float offset = sin(tiempo * 4 + i) * 5;
                        DrawCircle(esquinas[i].x + offset, esquinas[i].y + offset, 4, 
                                  Fade(temaActual->purpura, 0.8f * animacionAlpha));
                    }
                }
            }
            
            dibujarTextoConSombra("ENTER -> volver", 350, 400, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaFactorial() {
    string num = "";
    bool input = true, done = false;
    string resultado = "";
    long long factResult = 0;
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;

    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("FACTORIAL", 360, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input) {
            dibujarIndicadorEstado(180, 150, "Ingresando numero", temaActual->verde);
            dibujarTextoConSombra("Ingresa un numero:", 340, 190, 24, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(num.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(num,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(num)) {
                try {
                    int n = stoi(num);
                    if(n < 0) {
                        resultado = "Error: no existe factorial de negativo";
                        reproducirSonido(errorSound);
                        mostrarEfectoError(450, 250);
                    } else if(n > 20) {
                        resultado = "Error: numero muy grande (max 20)";
                        reproducirSonido(errorSound);
                        mostrarEfectoError(450, 250);
                    } else {
                        factResult = 1;
                        for(int i = 1; i <= n; i++) {
                            factResult *= i;
                        }
                        resultado = to_string(factResult);
                        string h = num + "! = " + resultado;
                        historial.push_back(h);
                        appendHistorialLine(h);
                        reproducirSonido(successSound);
                    }
                } catch(...) {
                    resultado = "Error: numero invalido";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                }
                input = false;
                done = true;
            }
        } else if(done) {
            if(resultado.find("Error") != string::npos) {
                dibujarTextoConSombra("Error:", 300, 180, 26, Fade(temaActual->rojo, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 300, 210, 24, Fade(temaActual->rojo, animacionAlpha));
            } else {
                // Mostrar cálculo paso a paso
                dibujarTextoConSombra("Factorial de", 300, 150, 26, Fade(temaActual->texto, animacionAlpha));
                dibujarTextoConSombra(num.c_str(), 430, 150, 26, Fade(temaActual->acento, animacionAlpha));
                
                // Mostrar la operación completa
                int n = stoi(num);
                string operacion = " = ";
                for(int i = 1; i <= n; i++) {
                    operacion += to_string(i);
                    if(i < n) operacion += " x ";
                }
                
                DrawText(operacion.c_str(), 300, 190, 22, Fade(temaActual->texto, 0.8f * animacionAlpha));
                
                // Resultado
                dibujarTextoConSombra("= ", 300, 230, 28, Fade(temaActual->texto, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 350, 230, 28, Fade(temaActual->verde, animacionAlpha));
                
                // Visualización de puntos progresivos
                int startX = 300;
                int startY = 300;
                int pointSize = 6;
                int pointsPerRow = 10;
                
                for(int i = 1; i <= min(n, 30); i++) {
                    int row = (i - 1) / pointsPerRow;
                    int col = (i - 1) % pointsPerRow;
                    int x = startX + col * 25;
                    int y = startY + row * 25;
                    
                    Color pointColor;
                    float hue = (float)i / max(n, 1) * 360.0f;
                    pointColor = ColorFromHSV(hue, 0.8f, 0.9f);
                    
                    DrawCircle(x, y, pointSize, Fade(pointColor, animacionAlpha));
                    DrawText(TextFormat("%d", i), x - 4, y - 5, 10, Fade(BLACK, animacionAlpha));
                }
                
                if(n > 30) {
                    DrawText("...", startX + pointsPerRow * 25, startY + 2 * 25, 20, 
                            Fade(temaActual->texto, 0.7f * animacionAlpha));
                }
            }
            
            dibujarTextoConSombra("ENTER -> volver", 350, 450, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

void pantallaPotencia() {
    string base = "", exponente = "";
    bool input1 = true, input2 = false, done = false;
    string resultado = "";
    double baseVal = 0, exponenteVal = 0;
    
    animacionAlpha = 0.0f;
    animacionEntrando = true;

    while(!IsKeyPressed(KEY_BACKSPACE) && !WindowShouldClose()) {
        // Actualizar música
        if (musicaReproduciendo) {
            UpdateMusicStream(backgroundMusic);
        }
        
        BeginDrawing();
        ClearBackground(temaActual->fondo);
        dibujarFondoAnimado();
        
        if(animacionEntrando && animacionAlpha < 1.0f) {
            animacionAlpha += 0.05f;
            if(animacionAlpha >= 1.0f) animacionEntrando = false;
        }
        
        dibujarTituloDecorado("POTENCIA", 360, 40, 34, Fade(temaActual->acento, animacionAlpha));
        
        Rectangle contenido = {100, 100, 700, 450};
        dibujarTarjetaConSombra(contenido, Fade(temaActual->tarjeta, animacionAlpha), 0.1f);
        
        if(input1) {
            dibujarIndicadorEstado(180, 150, "Ingresando base", temaActual->verde);
            dibujarTextoConSombra("Base:", 380, 190, 24, Fade(temaActual->texto, animacionAlpha));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(base.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(base,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(base)) {
                input1 = false;
                input2 = true;
                reproducirSonido(successSound);
            }
        } else if(input2) {
            dibujarIndicadorEstado(180, 150, "Ingresando exponente", temaActual->naranja);
            dibujarTextoConSombra("Exponente:", 350, 190, 24, Fade(temaActual->texto, animacionAlpha));

            // Mostrar operación parcial
            string operacionParcial = base + " ^ ?";
            dibujarTextoConSombra(operacionParcial.c_str(), 350, 220, 18, Fade(temaActual->texto, 0.6f));

            Rectangle campo = {200,240,500,64};
            dibujarTarjetaConSombra(campo, Fade(temaActual->acento,0.1f), 0.1f);
            DrawText(exponente.c_str(),220,260,30,Fade(temaActual->texto, animacionAlpha));

            tecladoNumerico(exponente,300,320);
            
            if(IsKeyPressed(KEY_ENTER) && esNumeroValido(exponente)) {
                try {
                    baseVal = stod(base);
                    int exp = stoi(exponente);
                    
                    if(exp < 0) {
                        resultado = "Error: exponente debe ser >= 0";
                        reproducirSonido(errorSound);
                        mostrarEfectoError(450, 250);
                    } else if(exp > 1000) {
                        resultado = "Error: exponente muy grande";
                        reproducirSonido(errorSound);
                        mostrarEfectoError(450, 250);
                    } else {
                        double potencia = pow(baseVal, exp);
                        char buf[64];
                        
                        if(potencia > 1e15) {
                            sprintf(buf, "%.4e", potencia);
                        } else {
                            sprintf(buf, "%.4f", potencia);
                        }
                        
                        resultado = buf;
                        string h = base + "^" + exponente + " = " + buf;
                        historial.push_back(h);
                        appendHistorialLine(h);
                        reproducirSonido(successSound);
                    }
                } catch(...) {
                    resultado = "Error: numeros invalidos";
                    reproducirSonido(errorSound);
                    mostrarEfectoError(450, 250);
                }
                input2 = false;
                done = true;
            }
        } else if(done) {
            if(resultado.find("Error") != string::npos) {
                dibujarTextoConSombra("Error:", 300, 180, 26, Fade(temaActual->rojo, animacionAlpha));
                dibujarTextoConSombra(resultado.c_str(), 300, 210, 24, Fade(temaActual->rojo, animacionAlpha));
            } else {
                string operacionCompleta = base + " ^ " + exponente;
                mostrarResultadoBonito(250, 220, operacionCompleta, resultado);
                
                // Visualización gráfica de la potencia
                int exp = stoi(exponente);
                if(exp <= 10) {
                    dibujarTextoConSombra("Descomposicion:", 300, 250, 22, 
                                        Fade(temaActual->texto, 0.8f * animacionAlpha));
                    
                    string descomposicion = base;
                    for(int i = 1; i < exp; i++) {
                        descomposicion += " x " + base;
                    }
                    
                    // Mostrar descomposición en líneas si es muy larga
                    int y_pos = 280;
                    string temp = descomposicion;
                    while(temp.length() > 0) {
                        string line = temp.substr(0, 50);
                        DrawText(line.c_str(), 200, y_pos, 18, 
                                Fade(temaActual->texto, 0.7f * animacionAlpha));
                        y_pos += 22;
                        if(temp.length() > 50) temp = temp.substr(50);
                        else break;
                    }
                }
                
                // Efecto visual de crecimiento exponencial
                if(exp > 0) {
                    int startX = 300;
                    int startY = 350;
                    int maxHeight = 60;
                    
                    for(int i = 0; i <= min(exp, 8); i++) {
                        double valor = pow(baseVal, i);
                        int height = (int)((valor / pow(baseVal, min(exp, 8))) * maxHeight);
                        int width = 30;
                        int x = startX + i * 40;
                        
                        Color barColor = ColorFromHSV((float)i / max(exp, 1) * 120.0f, 0.8f, 0.9f);
                        DrawRectangle(x, startY - height, width, height, 
                                     Fade(barColor, 0.6f * animacionAlpha));
                        DrawRectangleLines(x, startY - height, width, height, 
                                          Fade(barColor, animacionAlpha));
                        
                        if(i == exp) {
                            DrawRectangleLines(x - 2, startY - height - 2, width + 4, height + 4, 
                                              Fade(temaActual->verde, animacionAlpha));
                        }
                    }
                }
            }
            
            dibujarTextoConSombra("ENTER -> volver", 350, 450, 26, Fade(temaActual->acento, animacionAlpha));
            if(IsKeyPressed(KEY_ENTER)) {
                reproducirSonido(clickSound);
                return;
            }
        }
        
        dibujarTextoConSombra("BACKSPACE -> volver", 20,20,18, Fade(temaActual->texto, 0.7f));
        EndDrawing();
    }
}

// ======================================================
// MANEJO DE HISTORIAL EN ARCHIVO
// ======================================================

void appendHistorialLine(const string& line) {
    // Abrir en modo append para añadir la linea al final (usar ruta determinada)
    try {
        // Append to master per-user file (historialFilePath)
        std::ofstream ofs(historialFilePath, std::ios::app);
        if(ofs) {
            ofs << line << '\n';
            // Increment master file-backed count (only count once per master write)
            historialCount++;
        }
    } catch(...) {}
    try {
        // Also append to session file if set
        if(!sessionHistorialPath.empty()) {
            std::ofstream ofs2(sessionHistorialPath, std::ios::app);
            if(ofs2) ofs2 << line << '\n';
        }
    } catch(...) {}
}

// Añade una cabecera de sección con timestamp y usuario para diferenciar ejecuciones
void appendSectionHeader(const string& usuario) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[128];
    if(t) {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    } else {
        strcpy(buf, "unknown-time");
    }
    // Create a per-session filename first so session file will include the header
    char fn[128];
    if(t) {
        char stamp[64];
        strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", t);
        // sanitize username for filename (replace spaces and unsafe chars)
        string uname = usuario;
        for(auto &c : uname) if(c == ' ' || c=='\\' || c=='/' || c==':') c = '_';
        snprintf(fn, sizeof(fn), "historial_session_%s_%s.txt", stamp, uname.c_str());
    } else {
        snprintf(fn, sizeof(fn), "historial_session_unknown_%ld.txt", now);
    }
    sessionHistorialPath = string(fn);

    string header = string("=== Seccion: ") + buf + " | Usuario: " + usuario + " ===";
    // Do NOT store header in global memory; write directly to files
    appendHistorialLine(header);
}


void cargarHistorial() {
    // Count lines in the historial file and set historialCount.
    try {
        std::ifstream ifs(historialFilePath);
        if(!ifs) {
            historialCount = 0;
            return;
        }
        string linea;
        int count = 0;
        while(std::getline(ifs, linea)) {
            if(!linea.empty()) count++;
        }
        historialCount = count;
    } catch(...) {
        historialCount = 0;
    }
}

// Establece el directorio de trabajo a la carpeta donde se encuentra el ejecutable
void setWorkingDirectoryToExePath(int argc, char **argv) {
    try {
        if (argc > 0 && argv && argv[0]) {
            std::filesystem::path p(argv[0]);
            if (p.has_parent_path()) {
                std::filesystem::current_path(p.parent_path());
            }
        }
    } catch(...) {
        // ignore errors, no-op si filesystem no funciona
    }
}

// Inicializa la ruta de historial: si APPDATA está disponible, usar %APPDATA%\CalculadoraMoya\historial.txt
void initHistorialPath() {
    // Force local-only behavior: always use `historial.txt` next to the executable.
    // `setWorkingDirectoryToExePath` is called early in `main`, so current_path() is exe folder.
    try {
        std::filesystem::path p = std::filesystem::current_path();
        p /= "historial.txt";
        historialFilePath = p.string();
    } catch(...) {
        // If filesystem errors, fall back to simple filename (will be relative to cwd)
        historialFilePath = string("historial.txt");
    }
}

