#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>    
#include <gtc/quaternion.hpp>
#include <Component.hpp>

namespace open_gl_engine
{
    /// @brief Componente de transformación con soporte para jerarquías.
    /// Almacena posición, rotación (cuaternión) y escala locales, y calcula matrices
    /// locales y mundiales con cacheo sucio. Permite enlazar padres e hijos.
    struct Transform_Component : Component
    {
        glm::vec3 position;     ///< Posición local respecto al padre (o al mundo si no tiene padre).
        glm::quat rotation;     ///< Rotación local expresada como cuaternión.
        glm::vec3 scale;        ///< Escala local (1,1,1 por defecto).

        Transform_Component* parent = nullptr;            ///< Padre en la jerarquía (nullptr si es raíz).
        std::vector<Transform_Component*> children;       ///< Hijos directos.

        mutable glm::mat4 cachedLocalMatrix = glm::mat4(1.0f); ///< Matriz local cacheada.
        mutable glm::mat4 cachedWorldMatrix = glm::mat4(1.0f); ///< Matriz mundial cacheada.
        mutable bool      localDirty = true;                    ///< Indica si la matriz local debe recalcularse.
        mutable bool      worldDirty = true;                    ///< Indica si la matriz mundial debe recalcularse.

        // ---------------------------------------------------------------------
        // Constructores
        // ---------------------------------------------------------------------

        /// @brief Constructor por defecto. Inicializa posición, rotación y escala básicas.
        Transform_Component(Id& my_entity_id)
        {
            entity_id = my_entity_id;
            parent = nullptr;
            inicializate();
        }

        /// @brief Constructor que solo asigna posición.
        Transform_Component(Id& my_entity_id, const glm::vec3& pos)
            : Transform_Component(my_entity_id)
        {
            position = pos;
        }

        /// @brief Constructor que asigna padre y enlaza la jerarquía.
        Transform_Component(Id& my_entity_id, Transform_Component* my_parent)
            : parent(my_parent)
        {
            entity_id = my_entity_id;
            set_parent(my_parent);  // establece el padre y actualiza la lista de hijos
            inicializate();
        }

        /// @brief Constructor con posición, ángulos de Euler y escala.
        Transform_Component(Id& my_entity_id, const glm::vec3& pos, const glm::vec3& eulerDeg, const glm::vec3& scl)
            : Transform_Component(my_entity_id)
        {
            position = pos;
            rotation = glm::quat(glm::radians(eulerDeg)); // conversión de grados a cuaternión
            scale = scl;
            markDirty();
        }

        /// @brief Constructor con posición, cuaternión y escala.
        Transform_Component(Id& my_entity_id, const glm::vec3& pos, const glm::quat& quat, const glm::vec3& scl)
            : Transform_Component(my_entity_id)
        {
            position = pos;
            rotation = quat;
            scale = scl;
            markDirty();
        }

        // ---------------------------------------------------------------------
        // Copia y movimiento
        // ---------------------------------------------------------------------

        /// @brief Constructor de copia. Copia los valores y la caché si no estaba sucia.
        /// No copia padre ni hijos (copia independiente).
        Transform_Component(const Transform_Component& other)
            : position(other.position), rotation(other.rotation), scale(other.scale), parent(nullptr)
        {
            entity_id = other.entity_id;
            if (!other.localDirty)
            {
                cachedLocalMatrix = other.cachedLocalMatrix;
                cachedWorldMatrix = other.cachedWorldMatrix;
                localDirty = false;
                worldDirty = false;
            }
            else
            {
                markDirty();
            }
        }

        /// @brief Constructor de movimiento. Transfiere todos los datos y reajusta los punteros de los hijos.
        Transform_Component(Transform_Component&& other) noexcept
            : position(std::move(other.position)), rotation(std::move(other.rotation)),
            scale(std::move(other.scale)), parent(other.parent),
            children(std::move(other.children)),
            cachedLocalMatrix(std::move(other.cachedLocalMatrix)),
            cachedWorldMatrix(std::move(other.cachedWorldMatrix)),
            localDirty(other.localDirty), worldDirty(other.worldDirty)
        {
            entity_id = std::move(other.entity_id);

            // Actualizar el puntero al padre en cada hijo
            for (auto* child : children)
                child->parent = this;

            other.parent = nullptr;
            other.children.clear();
            other.markDirty();
        }

        /// @brief Operador de asignación por copia (idioma copy-and-swap).
        Transform_Component& operator=(const Transform_Component& other)
        {
            if (this != &other)
            {
                Transform_Component temp(other);
                swap(*this, temp);
            }
            return *this;
        }

        /// @brief Operador de asignación por movimiento.
        Transform_Component& operator=(Transform_Component&& other) noexcept
        {
            if (this != &other)
            {
                detach_parent();                        // nos desvinculamos de la jerarquía actual
                position = std::move(other.position);
                rotation = std::move(other.rotation);
                scale = std::move(other.scale);
                entity_id = std::move(other.entity_id);

                parent = other.parent;
                children = std::move(other.children);
                for (auto* child : children)
                    child->parent = this;

                cachedLocalMatrix = std::move(other.cachedLocalMatrix);
                cachedWorldMatrix = std::move(other.cachedWorldMatrix);
                localDirty = other.localDirty;
                worldDirty = other.worldDirty;

                other.parent = nullptr;
                other.children.clear();
                other.markDirty();
            }
            return *this;
        }

        /// @brief Intercambia dos Transform_Component de forma segura, incluyendo punteros a hijos.
        friend void swap(Transform_Component& a, Transform_Component& b) noexcept
        {
            using std::swap;
            swap(a.position, b.position);
            swap(a.rotation, b.rotation);
            swap(a.scale, b.scale);
            swap(a.entity_id, b.entity_id);
            swap(a.parent, b.parent);
            swap(a.children, b.children);
            swap(a.cachedLocalMatrix, b.cachedLocalMatrix);
            swap(a.cachedWorldMatrix, b.cachedWorldMatrix);
            swap(a.localDirty, b.localDirty);
            swap(a.worldDirty, b.worldDirty);

            // Reajustar los punteros de los hijos después del intercambio
            for (auto* child : a.children) child->parent = &a;
            for (auto* child : b.children) child->parent = &b;
        }

        /// @brief Destructor. Se desvincula del padre y deja a los hijos sin padre.
        ~Transform_Component()
        {
            if (parent)
            {
                auto& siblings = parent->children;
                auto it = std::find(siblings.begin(), siblings.end(), this);
                if (it != siblings.end())
                    siblings.erase(it);
                parent = nullptr;
            }
            for (auto* child : children)
                child->parent = nullptr;
        }

        // ---------------------------------------------------------------------
        // Setters individuales
        // ---------------------------------------------------------------------

        void set_position(const glm::vec3& _position) { position = _position; markDirty(); }
        void set_position_x(const float& x) { position.x = x; markDirty(); }
        void set_position_y(const float& y) { position.y = y; markDirty(); }
        void set_position_z(const float& z) { position.z = z; markDirty(); }

        /// @brief Asigna la rotación a partir de ángulos de Euler en grados.
        void set_rotation(const glm::vec3& eulerDegrees)
        {
            rotation = glm::quat(glm::radians(eulerDegrees));
            markDirty();
        }

        void set_rotation_quat(const glm::quat& q) { rotation = q; markDirty(); }
        void set_scale(const glm::vec3& _scale) { scale = _scale; markDirty(); }
        void set_scale_x(float x) { scale.x = x; markDirty(); }
        void set_scale_y(float y) { scale.y = y; markDirty(); }
        void set_scale_z(float z) { scale.z = z; markDirty(); }

        // ---------------------------------------------------------------------
        // Getters
        // ---------------------------------------------------------------------

        const glm::vec3& get_position() const { return position; }
        const glm::vec3& get_scale()    const { return scale; }

        /// @brief Devuelve la rotación en ángulos de Euler (radianes).
        glm::vec3 get_rotation() const { return glm::eulerAngles(rotation); }

        /// @brief Devuelve la rotación en grados.
        glm::vec3 get_rotation_degrees() const { return glm::degrees(get_rotation()); }

        const glm::quat& get_rotation_quat() const { return rotation; }

        // ---------------------------------------------------------------------
        // Vectores de orientación
        // ---------------------------------------------------------------------

        /// @brief Vector hacia adelante (forward) local (-Z).
        glm::vec3 getForward() const { return rotation * glm::vec3(0.0f, 0.0f, -1.0f); }

        /// @brief Vector hacia la derecha (right) local (+X).
        glm::vec3 getRight() const { return rotation * glm::vec3(1.0f, 0.0f, 0.0f); }

        /// @brief Vector hacia arriba (up) local (+Y).
        glm::vec3 getUp() const { return rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

        // ---------------------------------------------------------------------
        // Posiciones
        // ---------------------------------------------------------------------

        glm::vec3 get_local_position() const { return position; }

        /// @brief Posición en coordenadas mundiales (extraída de la cuarta columna de la matriz mundial).
        glm::vec3 get_world_position() const { return glm::vec3(get_transform_matrix()[3]); }

        // ---------------------------------------------------------------------
        // Matrices
        // ---------------------------------------------------------------------

        glm::mat4 get_translation_matrix() const { return glm::translate(glm::mat4(1.0f), position); }
        glm::mat4 get_rotation_matrix()    const { return glm::mat4_cast(rotation); }
        glm::mat4 get_scale_matrix()       const { return glm::scale(glm::mat4(1.0f), scale); }

        /// @brief Calcula (o devuelve cacheada) la matriz local TRS.
        glm::mat4 get_local_matrix() const
        {
            if (localDirty)
            {
                cachedLocalMatrix = get_translation_matrix() * get_rotation_matrix() * get_scale_matrix();
                localDirty = false;
                worldDirty = true; // la mundial queda obsoleta
            }
            return cachedLocalMatrix;
        }

        /// @brief Calcula (o devuelve cacheada) la matriz mundial (local * matriz del padre).
        glm::mat4 get_transform_matrix() const
        {
            if (worldDirty)
            {
                cachedWorldMatrix = parent ? parent->get_transform_matrix() * get_local_matrix()
                    : get_local_matrix();
                worldDirty = false;
            }
            return cachedWorldMatrix;
        }

        // ---------------------------------------------------------------------
        // Gestión de jerarquía
        // ---------------------------------------------------------------------

        /// @brief Establece un nuevo padre. Se desvincula del anterior y actualiza listas.
        void set_parent(Transform_Component* newParent)
        {
            if (parent == newParent) return;
            detach_parent();
            parent = newParent;
            if (parent)
                parent->children.push_back(this);
            markDirty();
        }

        /// @brief Elimina el vínculo con el padre actual, si existe.
        void detach_parent()
        {
            if (parent)
            {
                auto& siblings = parent->children;
                auto it = std::find(siblings.begin(), siblings.end(), this);
                if (it != siblings.end())
                    siblings.erase(it);
                parent = nullptr;
                markDirty();
            }
        }

    private:
        /// @brief Inicializa los valores por defecto (origen, sin rotación, escala 1).
        void inicializate()
        {
            position = glm::vec3(0, 0, 0);
            rotation = glm::vec3(0, 0, 0);  // implícitamente cuaternión identidad
            scale = glm::vec3(1, 1, 1);
        }

        /// @brief Marca la caché local y mundial como sucias y propaga a los hijos.
        void markDirty()
        {
            localDirty = true;
            worldDirty = true;
            for (auto* child : children)
                child->markDirty();
        }
    };
}