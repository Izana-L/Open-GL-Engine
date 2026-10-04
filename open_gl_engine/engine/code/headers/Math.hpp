#pragma once
#include <math.h>
#include <glm.hpp>

/// @brief Funciones y constantes matemáticas de utilidad general para el motor.
namespace open_gl_engine
{
    using namespace glm;

    /// @brief Vector normal apuntando hacia abajo (0, -1, 0).
    vec3 normal_bottom = vec3(0.f, -1.f, 0.f);
    /// @brief Vector normal apuntando hacia arriba (0, 1, 0).
    vec3 normal_top = vec3(0.f, 1.f, 0.f);
    /// @brief Valor de 2 * PI.
    float two_pi = 6.28318530717958f;
    /// @brief Valor de PI.
    float pi = 3.14159265358979f;

    /// @brief Calcula la normal de un triángulo dados tres puntos.
    /// @param point1 Primer vértice.
    /// @param point2 Segundo vértice.
    /// @param point3 Tercer vértice.
    /// @return Vector normal unitario (orientado según la mano derecha, negado para apuntar hacia fuera en la convención habitual).
    vec3 calculate_normal_three_point(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
    {
        // Producto vectorial de dos aristas y normalización, con signo invertido
        return -normalize(glm::cross(point2 - point1, point3 - point1));
    }

    /// @brief Calcula un punto en una circunferencia horizontal (Y=0) de radio 1.
    /// @param n Índice del punto (determina el ángulo como n * angle_rotate).
    /// @param angle_rotate Ángulo de rotación entre puntos consecutivos (radianes).
    /// @return Coordenada 3D en la circunferencia (X, 0, Z).
    vec3 calculate_circunference_point(const unsigned& n, const float& angle_rotate)
    {
        return { cos(n * angle_rotate), 0, sin(n * angle_rotate) };
    }

    /// @brief Calcula un punto en una circunferencia horizontal con una altura dada.
    /// @param n Índice del punto.
    /// @param angle_rotate Ángulo entre puntos (radianes).
    /// @param high Altura en Y del punto.
    /// @return Coordenada 3D (X, high, Z).
    vec3 calculate_circunference_point_with_high(const unsigned& n, const float& angle_rotate, const unsigned& high)
    {
        return { cos(n * angle_rotate), high, sin(n * angle_rotate) };
    }

    /// @name Vectores de orientación extraídos de una matriz de transformación.
    /// @{

    /// @brief Vector local Right (eje X) normalizado.
    vec3 vector_right(const mat4& transform_matrix) { return normalize(vec3(transform_matrix[0])); }
    /// @brief Vector local Up (eje Y) normalizado.
    vec3 vector_up(const mat4& transform_matrix) { return normalize(vec3(transform_matrix[1])); }
    /// @brief Vector local Forward (eje Z) normalizado.
    vec3 vector_front(const mat4& transform_matrix) { return normalize(vec3(transform_matrix[2])); }
    /// @brief Vector local Left (opuesto a Right).
    vec3 vector_left(const mat4& transform_matrix) { return -vector_right(transform_matrix); }
    /// @brief Vector local Down (opuesto a Up).
    vec3 vector_down(const mat4& transform_matrix) { return -vector_up(transform_matrix); }
    /// @brief Vector local Back (opuesto a Forward).
    vec3 vector_back(const mat4& transform_matrix) { return -vector_front(transform_matrix); }
    // Vectores:

    template< unsigned DIMENSION, typename TYPE >
    using Vector = glm::vec< DIMENSION, TYPE >;

    using Vector2 = Vector< 2, float >;
    using Vector3 = Vector< 3, float >;
    using Vector4 = Vector< 4, float >;

    // Matrices:

    template< unsigned COLUMNS, unsigned ROWS, typename TYPE >
    using Matrix = glm::mat< static_cast<glm::length_t>(COLUMNS), static_cast<glm::length_t>(ROWS), TYPE >;

    using Matrix2 = Matrix< 2, 2, float >;
    using Matrix3 = Matrix< 2, 2, float >;
    using Matrix4 = Matrix< 2, 2, float >;

    // Cuaternión:

    template< typename VALUE_TYPE = float >
    using Quaternion = glm::qua< VALUE_TYPE >;
    /// @}
}