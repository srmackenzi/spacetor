#include <iostream>
using namespace std;

void calculadora() {
    cout << "\n--- CALCULADORA ---\n";
    int a, b;
    cout << "Ingresa el primer numero: ";
    cin >> a;
    cout << "Ingresa el segundo numero: ";
    cin >> b;

    cout << "Suma: " << a + b << endl;
    cout << "Resta: " << a - b << endl;
    cout << "Multiplicacion: " << a * b << endl;
    if (b != 0) {
        cout << "Division: " << (float)a / b << endl;
    } else {
        cout << "Division: No se puede dividir entre 0\n";
    }
}
