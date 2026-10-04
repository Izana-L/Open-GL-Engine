// Texture.hpp
#pragma once

namespace open_gl_engine
{
    /// @brief Envoltura RAII para una textura 2D de OpenGL.
    /// Permite cargar, enlazar y liberar texturas. No copiable, solo movible.
    /// Almacena el ID de OpenGL, dimensiones, número de canales y un flag de transparencia.
    class Texture
    {
    public:
        unsigned int id;                     ///< Identificador de la textura en OpenGL.
        int width;                           ///< Ancho en píxeles.
        int height;                          ///< Alto en píxeles.
        int channels;                        ///< Número de canales (1, 3, 4...).
        bool has_alpha_less_than_one;        ///< Indica si la textura contiene canal alfa con valores < 1 (para ordenación de translúcidos).

        /// @brief Constructor por defecto. Inicializa una textura vacía.
        Texture() : id(0), width(0), height(0), channels(0), has_alpha_less_than_one(false)
        {
        }

        /// @brief Constructor parametrizado. Asigna un ID de OpenGL ya existente.
        /// @param _id ID de OpenGL de la textura.
        /// @param w Ancho en píxeles.
        /// @param h Alto en píxeles.
        /// @param c Número de canales.
        Texture(unsigned int _id, int w, int h, int c)
            : id(_id), width(w), height(h), channels(c), has_alpha_less_than_one(false)
        {
        }

        // No copiable
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Movible
        /// @brief Constructor de movimiento. Transfiere la propiedad del recurso.
        Texture(Texture&& other) noexcept;
        /// @brief Operador de asignación por movimiento.
        Texture& operator=(Texture&& other) noexcept;

        /// @brief Destructor. Libera la textura de OpenGL.
        ~Texture();

        /// @brief Vincula la textura a una unidad de textura.
        /// @param unit Unidad de textura (0 por defecto).
        void bind(unsigned int unit = 0) const;

        /// @brief Desvincula cualquier textura de la unidad de textura indicada.
        /// @param unit Unidad de textura a desvincular (0 por defecto).
        static void unbind(unsigned int unit = 0);

    private:
        /// @brief Libera el recurso OpenGL si está cargado.
        void release();
    };
}