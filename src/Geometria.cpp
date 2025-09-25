#include "Geometria.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

namespace Geometria {

double dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 cross(const Vector3& a, const Vector3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z, 
        a.x * b.y - a.y * b.x
    };
}

double norm(const Vector3& v) {
    return std::sqrt(dot(v, v));
}

Vector3 normalize(const Vector3& v) {
    double n = norm(v);
    if (n == 0.0) return {0, 0, 0};
    return v / n;  // ¡Ahora podemos usar el operador /!
}

double gradosARadianes(double grados) {
    return grados * M_PI / 180.0;
}

void proyectar(const Vector3& pos, const Cilindro& cil, 
               double& x, double& y, double& r, double& theta, bool& inside) {
    Vector3 a = normalize(cil.eje);
    
    // Vector auxiliar para construir base ortonormal
    Vector3 w = (std::fabs(a.x) < 0.9) ? Vector3{1, 0, 0} : Vector3{0, 1, 0};
    Vector3 u = normalize(cross(a, w));
    Vector3 v = cross(a, u);
    
    Vector3 rel = pos - cil.centro; 
    
    x = dot(rel, u);
    y = dot(rel, v);
    r = std::sqrt(x * x + y * y);
    theta = std::atan2(y, x);
    
    double z_a = dot(rel, a);
    inside = (r <= cil.radio && z_a >= 0.0 && z_a <= cil.altura);
}

bool estaEnCilindro(const Vector3& p, const Cilindro& c, float& radioDist) {
    Vector3 w = p - c.centro;  // ¡Operador -!
    double v2 = dot(c.eje, c.eje);
    
    if (v2 == 0.0) return false;
    
    double t = dot(w, c.eje) / v2;
    
    if (t < 0 || t > c.altura) {
        return false;
    }
    
    Vector3 proy = c.centro + t * c.eje;  // ¡Operadores + y *!
    Vector3 diff = p - proy;  // ¡Operador -!
    double diff_norm = norm(diff);
    radioDist = static_cast<float>(diff_norm);
    
    return diff_norm <= c.radio;
}

bool cargarDatosCilindro(const std::string& filename, double anguloBuscar,
                        double& radio, double& altura, Vector3& centro) {
    std::ifstream archivo(filename);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        return false;
    }
    
    std::string linea;
    bool encontradoCentro = false;
    bool encontradoRadio = false;
    bool encontradoAltura = false;
    
    while (std::getline(archivo, linea)) {
        std::istringstream iss(linea);
        std::string palabra;
        
        if (!(iss >> palabra)) continue;
        
        if (palabra == "radio") {
            iss >> radio;
            encontradoRadio = true;
        } else if (palabra == "altura") {
            iss >> altura;
            encontradoAltura = true;
        } else if (palabra == "angulo") {
            double anguloArchivo;
            std::string centroTag;
            double cx, cy, cz;
            iss >> anguloArchivo >> centroTag >> cx >> cy >> cz;
            
            if (std::abs(anguloArchivo - anguloBuscar) < 1e-4) {
                centro = {cx, cy, cz};
                encontradoCentro = true;
            }
        }
    }
    
    archivo.close();
    return encontradoCentro && encontradoRadio && encontradoAltura;
}

} // namespace Geometria
