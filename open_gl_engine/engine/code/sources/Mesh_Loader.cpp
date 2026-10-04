#include <Mesh_Loader.hpp>

#include <assimp/scene.h>   // Definición completa de aiMesh y aiFace, etc.
#include <glad/gl.h>        // Si quieres seguir usando GLuint
#include <cassert>          // assert
#include <Math.hpp>         // two_pi, calculate_circunference_point, etc.
#include <Heightmap.hpp>

namespace open_gl_engine
{
    // Para ahorrar escritura, podemos traer nombres al ámbito del .cpp
    using namespace std;
    using namespace glm;

    // ----------------------------------------------------------------------
    // Vértices
    // ----------------------------------------------------------------------
    vector<Vertex> load_imported_mesh(const aiMesh& imported_mesh)
    {
        vector<Vertex> vertex_vector;
        for (unsigned int index = 0; index < imported_mesh.mNumVertices; ++index)
        {
            vec2 texCoord(0.0f);
            if (imported_mesh.mTextureCoords[0])
            {
                texCoord.x = imported_mesh.mTextureCoords[0][index].x;
                texCoord.y = imported_mesh.mTextureCoords[0][index].y;
            }
            vertex_vector.emplace_back(
                vec3(imported_mesh.mVertices[index].x, imported_mesh.mVertices[index].y, imported_mesh.mVertices[index].z),
                vec3(imported_mesh.mNormals[index].x, imported_mesh.mNormals[index].y, imported_mesh.mNormals[index].z),
                texCoord);
        }
        return vertex_vector;
    }

    vector<Vertex> load_cone_mesh()
    {
        vector<Vertex> vertex_vector;
        unsigned int sections = 8;

        vec3 normal_down = vec3(0.f, -1.f, 0.f);
        vec3 coordinate_top = vec3(0.f, 1.f, 0.f);
        float angle_rotate = two_pi / float(sections);
        vertex_vector.reserve(4 * sections + 2);

        vertex_vector.emplace_back(vec3(0.f, 0.f, 0.f), normal_down, vec2(0.5f, 0.5f));
        for (unsigned n = 0; n < sections; ++n)
        {
            float angle = n * angle_rotate;
            vec3 point = calculate_circunference_point(n, angle_rotate);
            // Coordenadas UV: centradas en (0.5,0.5), radio 0.5
            vec2 uv(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
            vertex_vector.emplace_back(point, normal_down, uv);
        }
        for (unsigned n = 0; n < sections; ++n)
        {
            vec3 cordinate_n = calculate_circunference_point(n, angle_rotate);
            vec3 cordinate_nplus = calculate_circunference_point(n + 1, angle_rotate);
            vec3 normal = calculate_normal_three_point(coordinate_top, cordinate_n, cordinate_nplus);

            float u0 = float(n) / sections;
            float u1 = float(n + 1) / sections;
            float uTop = (float(n) + 0.5f) / sections;

            vertex_vector.emplace_back(cordinate_n, normal, vec2(u0, 0.0f));
            vertex_vector.emplace_back(cordinate_nplus, normal, vec2(u1, 0.0f));
            vertex_vector.emplace_back(coordinate_top, normal, vec2(uTop, 1.0f));
        }
        return vertex_vector;
    }

    vector<Vertex> load_plane_mesh()
    {
        vector<Vertex> vertex_vector;
        GLuint rows = 3;     // Podemos seguir usando GLuint aquí (viene de glad/gl.h)
        GLuint colums = 3;

        vertex_vector.reserve((colums + 1) * (rows + 1));

        vec3 starter_vertex = vec3{ -1, 0.f, 1 };
        float tile_width = 2.f / float(colums);
        float tile_height = 2.f / float(rows);

        for (unsigned j = 0; j < rows + 1; ++j)
        {
            for (unsigned i = 0; i < colums + 1; ++i)
            {
                vec2 uv(float(i) / colums, float(j) / rows);
                vertex_vector.emplace_back(starter_vertex, vec3(0, 1, 0), uv);
                starter_vertex.x += tile_width;
            }
            starter_vertex.x = -1;
            starter_vertex.z -= tile_height;
        }
        return vertex_vector;
    }

    vector<Vertex> load_cilinder_mesh()
    {
        vector<Vertex> vertex_vector;
        unsigned int sections = 8;

        vertex_vector.reserve(6 * sections + 2);
        float angle_rotate = two_pi / float(sections);
        vec3 cordinates0 = vec3{ 0.f, 0.f, 0.f };
        vec3 cordinates1 = vec3{ 0.f, 1.f, 0.f };
        vec3 normal_down = vec3{ 0.f, -1.f, 0.f };
        vec3 normal_top = vec3{ 0.f, 1.f, 0.f };

        vertex_vector.emplace_back(cordinates0, normal_down, vec2(0.5f, 0.5f));
        for (unsigned n = 0; n < sections; ++n)
        {
            float angle = n * angle_rotate;
            vec3 point = calculate_circunference_point(n, angle_rotate);
            vec2 uv(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
            vertex_vector.emplace_back(point, normal_down, uv);
        }
        vertex_vector.emplace_back(cordinates1, normal_top, vec2(0.5f, 0.5f));
        for (unsigned n = 0; n < sections; ++n)
        {
            float angle = n * angle_rotate;
            vec3 point = calculate_circunference_point_with_high(n, angle_rotate, 1);
            vec2 uv(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
            vertex_vector.emplace_back(point, normal_top, uv);
        }
        for (unsigned n = 0; n < sections; ++n)
        {
            vec3 cordinate_first = calculate_circunference_point(n, angle_rotate);
            vec3 cordinate_second = calculate_circunference_point(n + 1, angle_rotate);
            vec3 cordinate_thrird = calculate_circunference_point_with_high(n, angle_rotate, 1);
            vec3 normal = calculate_normal_three_point(cordinate_first, cordinate_second, cordinate_thrird);

            float u0 = float(n) / sections;
            float u1 = float(n + 1) / sections;

            // Franja vertical: v=0 abajo, v=1 arriba
            vertex_vector.emplace_back(cordinate_first, normal, vec2(u0, 0.0f));
            vertex_vector.emplace_back(cordinate_second, normal, vec2(u1, 0.0f));
            vertex_vector.emplace_back(cordinate_thrird, normal, vec2(u0, 1.0f));
            vertex_vector.emplace_back(calculate_circunference_point_with_high(n + 1, angle_rotate, 1), normal, vec2(u1, 1.0f));
        }
        return vertex_vector;
    }
    std::vector<Vertex> load_sphere_mesh()
    {
        const int sectors = 20;          // divisiones horizontales (meridianos)
        const int stacks = 10;          // divisiones verticales (paralelos)
        const float radius = 1.0f;

        // Pasos angulares precalculados
       
        const float divisions_of_stacks = 1.0f / stacks;
        const float divisions_of_sectors = 1.0f / sectors;
        std::vector<Vertex> vertices;
        vertices.reserve((stacks + 1) * (sectors + 1));

        for (int stack = 0; stack <= stacks; ++stack) 
        {
            float polar_angle = stack * pi * divisions_of_stacks;                    // ángulo desde el polo norte (0) al polo sur (pi)
            float y = glm::cos(polar_angle);                  // altura del anillo en el eje Y
            float ring_radius = glm::sin(polar_angle);                  // radio horizontal de este anillo
            float texture_v = stack * divisions_of_stacks;    // coordenada de textura vertical (0 = norte, 1 = sur)

            for (int sector = 0; sector <= sectors; ++sector) 
            {
                float azimuth_angle = sector * two_pi * divisions_of_sectors;            // ángulo alrededor del eje Y (0 a 2pi)
                float texture_u = sector * divisions_of_sectors; // coordenada de textura horizontal

                float x = ring_radius * glm::cos(azimuth_angle);
                float z = ring_radius * glm::sin(azimuth_angle);

                glm::vec3 position(x, y, z);
                glm::vec3 normal = position;                            // en esfera de radio 1 centrada en origen, la normal es el vector de posición
                glm::vec2 texture_coords(texture_u, texture_v);

                vertices.emplace_back(position, normal, texture_coords);
            }
        }
        return vertices;
    }
    std::vector<Vertex> load_cube_mesh()
    {
        // 6 caras, 4 vértices cada una -> 24 vértices (normales diferentes por cara)
        std::vector<Vertex> vertices;
        vertices.reserve(24);

        float half = 0.5f;
        // posición y normal de cada cara (front, back, left, right, top, bottom)
        struct Face {
            glm::vec3 normal;
            glm::vec3 point0, point1, point2, point3;
        };

        Face faces[6] = 
        {
            { glm::vec3(0, 0, 1) , glm::vec3(-half, -half, half), glm::vec3(half, -half, half), glm::vec3(half,  half, half), glm::vec3(-half,  half, half) }, // front
            { glm::vec3(0, 0,-1) , glm::vec3(half, -half,-half), glm::vec3(-half, -half,-half), glm::vec3(-half,  half,-half), glm::vec3(half,  half,-half) }, // back
            { glm::vec3(-1, 0, 0), glm::vec3(-half, -half,-half), glm::vec3(-half, -half, half), glm::vec3(-half,  half, half), glm::vec3(-half,  half,-half) }, // left
            { glm::vec3(1, 0, 0) , glm::vec3(half, -half, half), glm::vec3(half, -half,-half), glm::vec3(half,  half,-half), glm::vec3(half,  half, half) }, // right
            { glm::vec3(0, 1, 0) , glm::vec3(-half,  half, half), glm::vec3(half,  half, half), glm::vec3(half,  half,-half), glm::vec3(-half,  half,-half) }, // top
            { glm::vec3(0,-1, 0) , glm::vec3(-half, -half,-half), glm::vec3(half, -half,-half), glm::vec3(half, -half, half), glm::vec3(-half, -half, half) }  // bottom
        };

        for (const auto& face : faces) {
            // cada cara con coordenadas UV fijas (0,0) -> (1,1) en los 4 vértices
            vertices.emplace_back(face.point0, face.normal, glm::vec2(0, 1));
            vertices.emplace_back(face.point1, face.normal, glm::vec2(1, 1));
            vertices.emplace_back(face.point2, face.normal, glm::vec2(1, 0));
            vertices.emplace_back(face.point3, face.normal, glm::vec2(0, 0));
        }
        return vertices;
    }
    std::vector<Vertex> load_terrain_mesh(const Heightmap& heightmap)
    {
        std::vector<Vertex> vertices;
        vertices.reserve(heightmap.width * heightmap.height);

        float step_x = heightmap.world_size_xz / (heightmap.width - 1);
        float step_z = heightmap.world_size_xz / (heightmap.height - 1);
        float half_size = heightmap.world_size_xz * 0.5f;

        for (int z = 0; z < heightmap.height; ++z)
        {
            for (int x = 0; x < heightmap.width; ++x)
            {
                // Posición del vértice actual
                float pos_x = x * step_x - half_size;
                float pos_z = z * step_z - half_size;
                float height = heightmap.data[z * heightmap.width + x] * heightmap.max_world_height;
                glm::vec3 position(pos_x, height, pos_z);

                // Coordenada de textura
                float u = static_cast<float>(x) / (heightmap.width - 1);
                float v = static_cast<float>(z) / (heightmap.height - 1);
                glm::vec2 texcoord(u, v);

                // --- Cálculo de la normal usando diferencias finitas ---
                // Vecinos con clamping en los bordes
                int x_left = std::max(x - 1, 0);
                int x_right = std::min(x + 1, heightmap.width - 1);
                int z_up = std::max(z - 1, 0);
                int z_down = std::min(z + 1, heightmap.height - 1);

                // Posiciones de los vecinos (solo necesitamos las alturas y las coordenadas X/Z)
                float left_x = x_left * step_x - half_size;
                float left_h = heightmap.data[z * heightmap.width + x_left] * heightmap.max_world_height;
                float right_x = x_right * step_x - half_size;
                float right_h = heightmap.data[z * heightmap.width + x_right] * heightmap.max_world_height;
                float up_z = z_up * step_z - half_size;
                float up_h = heightmap.data[z_up * heightmap.width + x] * heightmap.max_world_height;
                float down_z = z_down * step_z - half_size;
                float down_h = heightmap.data[z_down * heightmap.width + x] * heightmap.max_world_height;

                // Vectores tangentes (en X y en Z)
                glm::vec3 tangent_x(right_x - left_x, right_h - left_h, 0.0f);
                glm::vec3 tangent_z(0.0f, down_h - up_h, down_z - up_z);

                glm::vec3 normal = glm::cross(tangent_z, tangent_x);
                if (glm::length(normal) > 0.0001f)
                    normal = glm::normalize(normal);
                // -----------------------------------------------

                vertices.emplace_back(position, normal, texcoord);
            }
        }

        return vertices;
    }
    vector<Vertex> load_skybox_cube_mesh()
    {
        
        vector<Vertex> vertices(8);
  
        vertices[0] = { {-1,-1,-1}, {0,0,0}, {-1,-1} };
        vertices[1] = { { 1,-1,-1}, {0,0,0}, {-1,-1} };
        vertices[2] = { { 1, 1,-1}, {0,0,0}, {-1,-1} };
        vertices[3] = { {-1, 1,-1}, {0,0,0}, {-1,-1} };
        vertices[4] = { {-1,-1, 1}, {0,0,0}, {-1,-1} };
        vertices[5] = { { 1,-1, 1}, {0,0,0}, {-1,-1} };
        vertices[6] = { { 1, 1, 1}, {0,0,0}, {-1,-1} };
        vertices[7] = { {-1, 1, 1}, {0,0,0}, {-1,-1} };
        return vertices;
    }
    std::vector<Vertex> load_quad_mesh()
    {
        
        std::vector<Vertex> vertices =
        {
            { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0,0,1), glm::vec2(0.0f, 1.0f) },
            { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0,0,1), glm::vec2(0.0f, 0.0f) },
            { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0,0,1), glm::vec2(1.0f, 0.0f) },
            { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0,0,1), glm::vec2(1.0f, 1.0f) }
        };
        return vertices;
    }
    // ----------------------------------------------------------------------
    // Índices
    // ----------------------------------------------------------------------
    vector<unsigned int> load_imported_mesh_index(const aiMesh& imported_mesh)
    {
        vector<unsigned int> index_vector;
        index_vector.reserve(imported_mesh.mNumFaces * 3);
        for (unsigned int j = 0; j < imported_mesh.mNumFaces; ++j)
        {
            assert(imported_mesh.mFaces[j].mNumIndices == 3 && "Error Mesh not triangulated");
            index_vector.push_back(imported_mesh.mFaces[j].mIndices[0]);
            index_vector.push_back(imported_mesh.mFaces[j].mIndices[1]);
            index_vector.push_back(imported_mesh.mFaces[j].mIndices[2]);
        }
        return index_vector;
    }

    vector<unsigned int> load_cone_mesh_index()
    {
        vector<unsigned int> index_vector;
        unsigned int sections = 8;
        for (unsigned i = 0; i < sections - 1; ++i)
        {
            index_vector.push_back(0);
            index_vector.push_back(i + 1);
            index_vector.push_back(i + 2);
        }
        index_vector.push_back(0);
        index_vector.push_back(sections);
        index_vector.push_back(1);

        for (unsigned i = 0; i < sections; ++i)
        {
            index_vector.push_back(3 * i + sections + 3);
            index_vector.push_back(3 * i + sections + 2);
            index_vector.push_back(3 * i + sections + 1);
        }
        return index_vector;
    }

    vector<unsigned int> load_plane_mesh_index()
    {
        vector<unsigned int> index_vector;
        GLuint rows = 3;
        GLuint colums = 3;
        index_vector.reserve(colums * rows * 2 * 3);

        GLint t = 0;
        for (unsigned int j = 0; j < rows; ++j)
        {
            for (unsigned int i = 0; i < colums; ++i)
            {
                unsigned int n = t + colums + 1;
                index_vector.push_back(t);
                index_vector.push_back(n + 1);
                index_vector.push_back(n);
                index_vector.push_back(t);
                index_vector.push_back(t + 1);
                index_vector.push_back(n + 1);
                ++t;
            }
            ++t;
        }
        return index_vector;
    }

    vector<unsigned int> load_cilinder_mesh_index()
    {
        vector<unsigned int> index_vector;
        unsigned int sections = 8;
        index_vector.reserve(sections * 12);

        for (unsigned i = 0; i < sections - 1; ++i)
        {
            index_vector.push_back(0);
            index_vector.push_back(i + 1);
            index_vector.push_back(i + 2);
        }
        index_vector.push_back(sections);
        index_vector.push_back(1);
        index_vector.push_back(0);

        for (unsigned i = 0; i < sections - 1; ++i)
        {
            index_vector.push_back(i + sections + 3);
            index_vector.push_back(i + sections + 2);
            index_vector.push_back(sections + 1);
        }
        index_vector.push_back(sections + 1);
        index_vector.push_back(sections + 2);
        index_vector.push_back(2 * sections + 1);

        for (unsigned i = 0; i < sections; ++i)
        {
            unsigned int n = i * 4;
            index_vector.push_back(n + 2 * sections + 4);
            index_vector.push_back(n + 2 * sections + 3);
            index_vector.push_back(n + 2 * sections + 2);

            index_vector.push_back(n + 2 * sections + 4);
            index_vector.push_back(n + 2 * sections + 5);
            index_vector.push_back(n + 2 * sections + 3);
        }
        return index_vector;
    }
    std::vector<unsigned int > load_sphere_mesh_index()
    {
        const int sectors = 20;
        const int stacks = 10;
        std::vector<unsigned int> indices;
        indices.reserve(stacks * sectors * 6);

        for (int i = 0; i < stacks; ++i) 
        {
            for (int j = 0; j < sectors; ++j) 
            {
                unsigned int first = i * (sectors + 1) + j;
                unsigned int second = first + sectors + 1;

                // dos triángulos por cuadrilátero
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
        return indices;
    }
    std::vector<unsigned> load_cube_mesh_index()
    {
        std::vector<unsigned> indices;
        indices.reserve(36); // 6 caras * 2 triángulos * 3 índices
        for (unsigned i = 0; i < 6; ++i) {
            unsigned base = i * 4;
            // primer triángulo
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            // segundo triángulo
            indices.push_back(base + 2);
            indices.push_back(base + 3);
            indices.push_back(base);
        }
        return indices;
    }
    std::vector<unsigned int> load_terrain_mesh_index(const Heightmap& heightmap)
    {
        std::vector<unsigned int> indices;
        indices.reserve((heightmap.width - 1) * (heightmap.height - 1) * 6);

        for (int z = 0; z < heightmap.height - 1; ++z)
        {
            for (int x = 0; x < heightmap.width - 1; ++x)
            {
                unsigned int top_left = z * heightmap.width + x;
                unsigned int top_right = top_left + 1;
                unsigned int bottom_left = (z + 1) * heightmap.width + x;
                unsigned int bottom_right = bottom_left + 1;

                // Primer triángulo
                indices.push_back(top_left);
                indices.push_back(bottom_left);
                indices.push_back(top_right);

                // Segundo triángulo
                indices.push_back(top_right);
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
            }
        }

        return indices;
    }
    vector<unsigned int> load_skybox_cube_mesh_index()
    {
       
        return {
           
            1, 0, 3,  1, 3, 2,
   
            4, 5, 6,  4, 6, 7,
           
            5, 1, 2,  5, 2, 6,
          
            0, 4, 7,  0, 7, 3,
        
            3, 6, 2,  3, 7, 6,
            
            0, 1, 5,  0, 5, 4
        };
    }

    std::vector<unsigned> load_quad_mesh_index()
    {
        // Dos triángulos
        return { 0, 1, 2,  0, 2, 3 };
    }
} 