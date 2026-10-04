#pragma once

namespace open_gl_engine
{
    /// @brief Envoltura RAII para un cubemap de OpenGL.
    /// Gestiona la creación, enlace y liberación de un objeto de textura GL_TEXTURE_CUBE_MAP.
    /// No copiable, pero sí movible.
    class Cubemap
    {
    public:
        unsigned int id = 0;     ///< Identificador OpenGL del cubemap.
        bool loaded = false;     ///< Indica si el recurso ha sido cargado y está disponible.

        /// @brief Constructor por defecto. Crea un cubemap vacío.
        Cubemap();

        /// @brief Constructor explícito a partir de un ID de OpenGL ya generado.
        /// @param _id Identificador OpenGL del cubemap.
        explicit Cubemap(unsigned int _id);

        // No copiable
        Cubemap(const Cubemap&) = delete;
        Cubemap& operator=(const Cubemap&) = delete;

        // Movible
        /// @brief Constructor de movimiento. Transfiere la propiedad del recurso.
        Cubemap(Cubemap&& other) noexcept;

        /// @brief Operador de asignación por movimiento.
        Cubemap& operator=(Cubemap&& other) noexcept;

        /// @brief Destructor. Libera el recurso OpenGL si está cargado.
        ~Cubemap();

        /// @brief Vincula el cubemap a una unidad de textura.
        /// @param unit Unidad de textura a la que enlazar (por defecto 0).
        void bind(int unit = 0) const;

        /// @brief Desvincula cualquier cubemap de la unidad de textura indicada.
        /// @param unit Unidad de textura a desvincular (por defecto 0).
        static void unbind(int unit = 0);

    private:
        /// @brief Libera el recurso OpenGL si está cargado.
        void release();
    };
}