#!/bin/bash
# build.sh - Script para compilar el Analizador de Partículas
# Uso: ./build.sh [opciones]

set -e # Salir en error

# Configuración por defecto
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN_BUILD=false
SHOW_HELP=false
VERBOSE=false
JOBS=$(nproc)

# Detectar si soporta colores
if [ -t 1 ]; then
  HAS_COLOR=true
else
  HAS_COLOR=false
fi

# Función para mostrar ayuda
show_help() {
  cat <<'EOF'
Analizador de Partículas - Script de Compilación

Uso:
    ./build.sh [opciones]

Opciones:
    -d, --debug          Compilar en modo Debug (por defecto: Release)
    -c, --clean          Limpiar directorio de build antes de compilar
    -j, --jobs N         Usar N jobs paralelos (por defecto: todos los cores)
    -v, --verbose        Mostrar output detallado
    -h, --help           Mostrar este mensaje de ayuda

Ejemplos:
    ./build.sh                    # Compilación normal Release
    ./build.sh --debug            # Compilación Debug
    ./build.sh --clean --debug    # Limpiar y compilar en Debug
    ./build.sh --jobs 4           # Usar 4 cores para compilar
    ./build.sh --verbose          # Output detallado

Descripción:
    Este script automatiza la compilación del Analizador de Partículas,
    un programa para análisis de distribuciones de densidad y presión
    en simulaciones de partículas.

    El proyecto utiliza CMake y C++20, y está diseñado para ser extensible
    mediante el patrón de diseño de analizadores base.

Tipos de análisis soportados:
    - densidad: Histogramas 2D de distribución de partículas
    - presion:   Cálculo de presión por partícula

Uso del programa después de compilar:
    ./build/analizador_particulas densidad 45.0 datos_cilindros.txt
    ./build/analizador_particulas presion 30.0 datos_cilindros.txt ./directorio_datos

Repositorio: https://github.com/tuusuario/analizador-particulas
EOF
}

# Funciones de logging mejoradas (compatibles)
log_info() {
  if [ "$HAS_COLOR" = true ]; then
    printf "\033[34m[INFO]\033[0m %s\n" "$1"
  else
    printf "[INFO] %s\n" "$1"
  fi
}

log_success() {
  if [ "$HAS_COLOR" = true ]; then
    printf "\033[32m[SUCCESS]\033[0m %s\n" "$1"
  else
    printf "[SUCCESS] %s\n" "$1"
  fi
}

log_warning() {
  if [ "$HAS_COLOR" = true ]; then
    printf "\033[33m[WARNING]\033[0m %s\n" "$1"
  else
    printf "[WARNING] %s\n" "$1"
  fi
}

log_error() {
  if [ "$HAS_COLOR" = true ]; then
    printf "\033[31m[ERROR]\033[0m %s\n" "$1"
  else
    printf "[ERROR] %s\n" "$1"
  fi
}

# Parsear argumentos
while [[ $# -gt 0 ]]; do
  case $1 in
  -d | --debug)
    BUILD_TYPE="Debug"
    shift
    ;;
  -c | --clean)
    CLEAN_BUILD=true
    shift
    ;;
  -j | --jobs)
    if [[ $2 =~ ^[0-9]+$ ]]; then
      JOBS="$2"
      shift 2
    else
      log_error "Número de jobs inválido: $2"
      exit 1
    fi
    ;;
  -v | --verbose)
    VERBOSE=true
    shift
    ;;
  -h | --help)
    SHOW_HELP=true
    shift
    ;;
  *)
    log_error "Argumento desconocido: $1"
    echo "Usa './build.sh --help' para ver las opciones disponibles."
    exit 1
    ;;
  esac
done

# Mostrar ayuda si se solicitó
if [ "$SHOW_HELP" = true ]; then
  show_help
  exit 0
fi

echo ""

log_info "Iniciando compilación..."
log_info "Modo: $BUILD_TYPE"
log_info "Jobs: $JOBS"
log_info "Directorio de build: $BUILD_DIR"

# Limpiar build si se solicitó
if [ "$CLEAN_BUILD" = true ]; then
  log_info "Limpiando directorio de build..."
  rm -rf "$BUILD_DIR"
fi

# Crear directorio de build si no existe
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar CMake
log_info "Configurando CMake..."
if [ "$VERBOSE" = true ]; then
  cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
else
  cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" .. >/dev/null 2>&1
fi

# Compilar
log_info "Compilando proyecto..."
if [ "$VERBOSE" = true ]; then
  cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS"
else
  if cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS" >/dev/null 2>&1; then
    log_success "Compilación completada exitosamente!"
  else
    log_error "Error en la compilación. Ejecuta con --verbose para más detalles."
    exit 1
  fi
fi

# Verificar que el ejecutable se creó
if [ -f "trajectory_analizer" ]; then
  log_success "Ejecutable generado correctamente"
  echo ""
  log_info "Ejecutable: $BUILD_DIR/analizador_particulas"
  log_info "Tamaño: $(du -h analizador_particulas | cut -f1)"

  # Mostrar información del ejecutable
  echo ""
  log_info "Ejemplos de uso:"
  echo "  ./trajectory_analizer densidad 45.0 datos_cilindros.txt"
  echo "  ./trajectory_analizer presion 30.0 datos_cilindros.txt ./mi_directorio"
  echo ""
else
  log_error "El ejecutable no se generó correctamente"
  exit 1
fi

# Mensaje final
log_success "¡El proyecto está listo para usar!"
