#pragma once
#include "AnalizadorBase.h"
#include <vector>

struct DatosPresion {
    float radio;
    double presion;
};

class AnalizadorPresion : public AnalizadorBase {
public:
    AnalizadorPresion();
    void procesarArchivo(const std::string& filepath) override;

private:
    std::vector<DatosPresion> datosPresion;
    
    void procesarLineaDatos(const std::string& linea, const Vector3& pos) override;
    void guardarResultados(const std::string& filepath);
};
