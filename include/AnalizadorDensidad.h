#pragma once
#include "AnalizadorBase.h"
#include <mutex>
#include <vector>

class AnalizadorDensidad : public AnalizadorBase {
public:
  AnalizadorDensidad();
  void procesarArchivo(const std::string &filepath) override;

private:
  static const int NR = 50;
  static const int NTH = 72;

  std::vector<std::vector<int>> hist2D;
  std::vector<int> histR;
  std::vector<int> histTheta;
  std::mutex datosMutex;

  // Eliminar el parámetro std::istringstream&
  void procesarLineaDatos(const std::string &linea,
                          const Vector3 &pos) override;
  void guardarResultados(const std::string &filepath);
};
