#include "AnalizadorPresion.h"
#include "Geometria.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

AnalizadorPresion::AnalizadorPresion() { nombre = "Analizador de Presión"; }

void AnalizadorPresion::procesarArchivo(const std::string &filepath) {
  std::ifstream infile(filepath);
  if (!infile.is_open()) {
    std::cerr << "No se pudo abrir: " << filepath << '\n';
    return;
  }

  datosPresion.clear();

  std::string line;
  size_t num_atoms = 0;

  while (std::getline(infile, line)) {
    if (line.rfind("ITEM: NUMBER OF ATOMS", 0) == 0) {
      std::getline(infile, line);
      num_atoms = std::stoul(line);
    } else if (line.rfind("ITEM: ATOMS", 0) == 0) {
      size_t index = 0;
      while (index < num_atoms && std::getline(infile, line)) {
        // Crear el stringstream aquí
        std::istringstream ss(line);
        int id, tipo;
        double x, y, z;
        double vx, vy, vz, fx, fy, fz;
        double r, m;
        double sxx, syy, szz, sxy, sxz, syz;

        if (ss >> id >> tipo >> x >> y >> z >> vx >> vy >> vz >> fx >> fy >>
            fz >> r >> m >> sxx >> syy >> szz >> sxy >> sxz >> syz) {

          Vector3 posicion = {x, y, z};
          procesarLineaDatos(line, posicion);
        }
        ++index;
      }
      break;
    }
  }

  infile.close();
  guardarResultados(filepath);
}

void AnalizadorPresion::procesarLineaDatos(const std::string &linea,
                                           const Vector3 &pos) {
  // Recrear el stringstream desde la línea
  std::istringstream ss(linea);
  int id, tipo;
  double x, y, z;
  double vx, vy, vz, fx, fy, fz;
  double r, m;
  double sxx, syy, szz, sxy, sxz, syz;

  if (!(ss >> id >> tipo >> x >> y >> z >> vx >> vy >> vz >> fx >> fy >> fz >>
        r >> m >> sxx >> syy >> szz >> sxy >> sxz >> syz)) {
    return;
  }

  float radioDist;
  bool inside = Geometria::estaEnCilindro(pos, cilindro, radioDist);

  if (inside) {
    double presion_fix =
        -(sxx - m * vx * vx + syy - m * vy * vy + szz - m * vz * vz) / 3.0;
    double volume = 4.0 / 3.0 * M_PI * r * r * r;

    DatosPresion dato;
    dato.radio = radioDist;
    dato.presion = presion_fix / volume;
    {
      std::lock_guard<std::mutex> lock(datosMutex);
      datosPresion.push_back(dato);
    }
  }
}

void AnalizadorPresion::guardarResultados(const std::string &filepath) {
  std::string outname =
      "presion_" + fs::path(filepath).filename().string() + ".dat";
  std::ofstream out(outname);

  for (const auto &dato : datosPresion) {
    out << dato.radio << " " << dato.presion << "\n";
  }

  out.close();
  std::cout << "✓ Procesado: " << filepath << " → " << outname << " ("
            << datosPresion.size() << " partículas)\n";
}
