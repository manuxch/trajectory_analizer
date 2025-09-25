#pragma once
#include <string>
#include <vector>
#include "Geometria.h"

class AnalizadorBase {
public:
    virtual ~AnalizadorBase() = default;
    
    void configurar(const Cilindro& cilindro, const std::string& directorio);
    virtual void procesarArchivo(const std::string& filepath) = 0;
    void ejecutarAnalisis();
    
    std::string getNombre() const { return nombre; }

protected:
    std::string nombre;
    Cilindro cilindro;
    std::string directorioTrabajo;
    
    // Cambiar a const std::string& para evitar copias innecesarias
    virtual void procesarLineaDatos(const std::string& linea, const Vector3& pos) = 0;
};
