#pragma once
#include <cmath>  
#include <string>

struct Vector3 {
    double x, y, z;
    
    // Operadores como métodos de la clase
    Vector3 operator-() const { return {-x, -y, -z}; } // Operador unario -
    Vector3 operator+(const Vector3& other) const { 
        return {x + other.x, y + other.y, z + other.z}; 
    }
    Vector3 operator-(const Vector3& other) const { 
        return {x - other.x, y - other.y, z - other.z}; 
    }
    Vector3 operator*(double scalar) const { 
        return {x * scalar, y * scalar, z * scalar}; 
    }
    Vector3 operator/(double scalar) const { 
        return {x / scalar, y / scalar, z / scalar}; 
    }
    
    // Operadores de asignación
    Vector3& operator+=(const Vector3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    Vector3& operator-=(const Vector3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    Vector3& operator*=(double scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    Vector3& operator/=(double scalar) {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }
    
    // Comparación
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }
};

struct Cilindro {
    Vector3 centro;
    Vector3 eje;
    double radio;
    double altura;
};

namespace Geometria {
    // Operador de multiplicación por escalar (para permitir double * Vector3)
    inline Vector3 operator*(double scalar, const Vector3& vec) {
        return vec * scalar;
    }
    
    // Producto punto
    double dot(const Vector3& a, const Vector3& b);
    
    // Producto cruz
    Vector3 cross(const Vector3& a, const Vector3& b);
    
    // Norma y normalización
    double norm(const Vector3& v);
    Vector3 normalize(const Vector3& v);
    
    // Conversión de grados a radianes
    double gradosARadianes(double grados);
    
    // Proyección en cilindro
    void proyectar(const Vector3& pos, const Cilindro& cil, 
                   double& x, double& y, double& r, double& theta, bool& inside);
    
    // Verificación de pertenencia al cilindro
    bool estaEnCilindro(const Vector3& p, const Cilindro& c, float& radio);
    
    // Carga de datos del cilindro
    bool cargarDatosCilindro(const std::string& filename, double anguloBuscar,
                            double& radio, double& altura, Vector3& centro);
}
