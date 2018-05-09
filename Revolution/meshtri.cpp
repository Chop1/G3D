#include "meshtri.h"
#include "matrices.h"



void MeshTri::clear()
{
	m_points.clear();
	m_normals.clear();
	m_line_indices.clear();
	m_tri_indices.clear();
}


int MeshTri::add_vertex(const Vec3& P)
{
	// ajouter P à m_points
	// renvoyer l'indice du point
    m_points.push_back(P);
    return m_points.size()-1;
}

int MeshTri::add_normal(const Vec3& N)
{
	// ajouter P à m_normals
    m_normals.push_back(N);
	// renvoyer l'indice du point
    return m_normals.size()-1;
}

void MeshTri::add_line(int i1, int i2)
{
    m_line_indices.push_back(i1);
    m_line_indices.push_back(i2);
}


void MeshTri::add_tri(int i1, int i2, int i3)
{
    m_tri_indices.push_back(i1);
    m_tri_indices.push_back(i2);
    m_tri_indices.push_back(i3);
}

void MeshTri::add_quad(int i1, int i2, int i3, int i4)
{
    add_tri(i1, i2, i3);
    add_tri(i1, i3, i4);
}


void MeshTri::create_pyramide()
{
	clear();

	// ajouter les 5 points
    Vec3 x1(-1, -1, 0);
    Vec3 x2(1, -1, 0);
    Vec3 x3(1, 1, 0);
    Vec3 x4(-1, 1, 0);
    Vec3 x5(0, 0, 1);

    add_vertex(x1); // 0
    add_vertex(x2);
    add_vertex(x3);
    add_vertex(x4);
    add_vertex(x5);


	// ajouter les 8 lignes (segments)
    add_line(0, 1);
    add_line(1, 2);
    add_line(2, 3);
    add_line(3, 0);
    add_line(0, 4);
    add_line(1, 4);
    add_line(2, 4);
    add_line(3, 4);


	// ajouter les 5 faces
    add_tri(0, 1, 4);
    add_tri(1, 2, 4);
    add_tri(2, 3, 4);
    add_tri(3, 0, 4);

    add_quad(3, 2, 1, 0);


	gl_update();
}

void MeshTri::anneau()
{
	clear();
//    std::vector<Vec3> yoloVecteur;
//    Vec3 yoloPoint;
//	// ajouter les points
//    for(int i=0; i<360; i+=10)
//    {
//        yoloPoint()
//        yoloVecteur.push_back(
//    }

	// ajouter les lignes
	// ajouter les faces
	gl_update();
}

void MeshTri::spirale()
{
	clear();
	// ajouter les points
	// ajouter les lignes
	// ajouter les faces
	gl_update();
}


void MeshTri::revolution(const std::vector<Vec3>& poly)
{
	clear();
    int N = 10;
    int n = poly.size();
    int nb_column = 360/N;
	// ajouter les points
    for(unsigned int i =0; i<360; i+=N)
    {
        Mat4 rota = rotateY(i);

        for(Vec3 p : poly)
        {
            add_vertex(Vec3(rota*Vec4(p, 1)));
        }
    }

    // ajouter les faces
    for(int i=0; i<nb_column-1; ++i)
    {
        for(unsigned int j=0; j<n; ++j)
        {
            int k = i*n + j;
            add_quad(k-1, k, k+n, k-1+n);

            add_line(k, k+N);
            add_line(k, k-1);
            add_line(k-1, k-1+N);
            add_line(k+N, k-1+N);
        }
    }

    for(int j =0; j<n; ++j)
    {
        for(int i=0; i<nb_column; ++i)
        {
            int k = i*n+j;
            int kk = ((i+1)%nb_column)*n+j;
            add_line(k, kk);
        }
    }

	// ajouter les normales au sommets
	// ajouter les lignes


	// Faire varier angle 0 -> 360 par pas de D degre
	// Faire tourner les sommets du polygon -> nouveau points

	// on obtient une grille de M x N (360/D x poly.nb) points

	// pour la creation des quads qui relient ces points,
	// attention la derniere rangee doit etre reliee a la premiere

	gl_update();
}




MeshTri::MeshTri()
{}



void MeshTri::gl_init()
{
	m_shader_flat = new ShaderProgramFlat();
	m_shader_phong = new ShaderProgramPhong();
	m_shader_color = new ShaderProgramColor();

	//VBO
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vbo2);

	//VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_flat->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_flat->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	//VAO2
	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_phong->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_phong->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	glEnableVertexAttribArray(m_shader_phong->idOfNormalAttribute);
	glVertexAttribPointer(m_shader_phong->idOfNormalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);


	//EBO indices
	glGenBuffers(1, &m_ebo);
	glGenBuffers(1, &m_ebo2);
}

void MeshTri::gl_update()
{
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * m_points.size() * sizeof(GLfloat), &(m_points[0][0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (!m_normals.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
		glBufferData(GL_ARRAY_BUFFER, 3 * m_normals.size() * sizeof(GLfloat), &(m_normals[0][0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//EBO indices
	if (!m_tri_indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_tri_indices.size() * sizeof(int), &(m_tri_indices[0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	if (!m_line_indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_line_indices.size() * sizeof(int), &(m_line_indices[0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}



void MeshTri::set_matrices(const Mat4& view, const Mat4& projection)
{
	viewMatrix = view;
	projectionMatrix = projection;
}

void MeshTri::draw_lines(const Vec3& color)
{
	m_shader_color->startUseProgram();

	m_shader_color->sendViewMatrix(viewMatrix);
	m_shader_color->sendProjectionMatrix(projectionMatrix);

	glBindVertexArray(m_vao);

	if (has_faces())
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0,1.0);
		glUniform3f(m_shader_color->idOfColorUniform, 0,0,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
        glDrawElements(GL_TRIANGLES, m_tri_indices.size(),GL_UNSIGNED_INT,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	glUniform3fv(m_shader_color->idOfColorUniform, 1, glm::value_ptr(color));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo2);
	glDrawElements(GL_LINES, m_line_indices.size(),GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	m_shader_color->stopUseProgram();
}


void MeshTri::draw_flat(const Vec3& color)
{
	m_shader_flat->startUseProgram();

	m_shader_flat->sendViewMatrix(viewMatrix);
	m_shader_flat->sendProjectionMatrix(projectionMatrix);

	glUniform3fv(m_shader_flat->idOfColorUniform, 1, glm::value_ptr(color));

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(),GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	m_shader_flat->stopUseProgram();
}


void MeshTri::draw_smooth(const Vec3& color)
{
	m_shader_phong->startUseProgram();

	m_shader_phong->sendViewMatrix(viewMatrix);
	m_shader_phong->sendProjectionMatrix(projectionMatrix);

	glUniform3fv(m_shader_phong->idOfColorUniform, 1, glm::value_ptr(color));

	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
	glDrawElements(GL_TRIANGLES, m_tri_indices.size(),GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	m_shader_phong->stopUseProgram();
}




