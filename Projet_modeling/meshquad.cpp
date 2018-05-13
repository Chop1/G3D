#include "meshquad.h"
#include "matrices.h"


void MeshQuad::clear()
{
}

int MeshQuad::add_vertex(const Vec3& P)
{
    // ajouter P à m_points
    // renvoyer l'indice du point
    m_points.push_back(P);
    return m_points.size()-1;
}


void MeshQuad::add_quad(int i1, int i2, int i3, int i4)
{
    m_quad_indices.push_back(i1);
    m_quad_indices.push_back(i2);
    m_quad_indices.push_back(i3);
    m_quad_indices.push_back(i4);
}

void MeshQuad::convert_quads_to_tris(const std::vector<int>& quads, std::vector<int>& tris)
{
	tris.clear();
	tris.reserve(3*quads.size()/2);

	// Pour chaque quad on genere 2 triangles
    for(unsigned int i=0; i<quads.size(); i+=4)
    {
        tris.push_back(quads.at(i));
        tris.push_back(quads.at(i+1));
        tris.push_back(quads.at(i+2));

        tris.push_back(quads.at(i));
        tris.push_back(quads.at(i+2));
        tris.push_back(quads.at(i+3));
    }

	// Attention a repecter l'orientation des triangles

}

void MeshQuad::convert_quads_to_edges(const std::vector<int>& quads, std::vector<int>& edges)
{
	edges.clear();
	edges.reserve(quads.size()); // ( *2 /2 !)
	// Pour chaque quad on genere 4 aretes, 1 arete = 2 indices.
	// Mais chaque arete est commune a 2 quads voisins !
	// Comment n'avoir qu'une seule fois chaque arete ?
    int a, b, c, d;

    for(unsigned int i=0; i<quads.size(); )
    {
        a = quads[i++];
        b = quads[i++];
        c = quads[i++];
        d = quads[i++];

        if(a<b)
        {
            edges.push_back(a);
            edges.push_back(b);
        }

        if(b<c)
        {
            edges.push_back(b);
            edges.push_back(c);
        }

        if(c<d)
        {
            edges.push_back(c);
            edges.push_back(d);
        }

        if(d<a)
        {
            edges.push_back(a);
            edges.push_back(d);
        }
    }
}


void MeshQuad::bounding_sphere(Vec3& C, float& R)
{
	// C=
    // centre de gravité (m de tout les points entre eux)
    // note: pas minimum
    // TODO remplacer les valeurs par defaut
    C = {0, 0, 0};

	// R=
    // rayon entre centre et point le plus eloigné
    R = 10;
}


void MeshQuad::create_cube()
{
	clear();
	// ajouter 8 sommets (-1 +1)
    Vec3 x1(-1, 1, 1);
    Vec3 x2(-1, -1, 1);
    Vec3 x3(1, -1, 1);
    Vec3 x4(1, 1, 1);
    Vec3 x5(1, 1, -1);
    Vec3 x6(1, -1, -1);
    Vec3 x7(-1, 1, -1);
    Vec3 x8(-1, -1, -1);


    add_vertex(x1); // 0
    add_vertex(x2);
    add_vertex(x3);
    add_vertex(x4);
    add_vertex(x5);
    add_vertex(x6);
    add_vertex(x7);
    add_vertex(x8);

	// ajouter 6 faces (sens trigo)
    add_quad(0, 1, 2, 3);
    add_quad(3, 2, 5, 4);
    add_quad(4, 5, 7, 6);
    add_quad(6, 7, 1, 0);
    add_quad(6, 0, 3, 4);
    add_quad(7, 5, 2, 1); // 7, 1, 2, 5

	gl_update();
}

Vec3 MeshQuad::normal_of(const Vec3& A, const Vec3& B, const Vec3& C)
{
	// Attention a l'ordre des points !
	// le produit vectoriel n'est pas commutatif U ^ V = - V ^ U
	// ne pas oublier de normaliser le resultat.

    int X = (B.y-A.y)*(C.z-A.z) - (B.z-A.z)*(C.y-A.y);
    int Y = (B.z-A.z)*(C.x-A.x) - (B.x-A.x)*(C.z-A.z);
    int Z = (B.x-A.x)*(C.y-A.y) - (B.y-A.y)*(C.x-A.x);

    Vec3 res(X, Y, Z);

    res = normalize(res);

    return res;
}

Vec3 MeshQuad::vecOf(const Vec3& A, const Vec3& B)
{
    return Vec3(B.x-A.x, B.y-A.y, B.z-A.z);
}


bool MeshQuad::is_points_in_quad(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
	// On sait que P est dans le plan du quad.

	// P est-il au dessus des 4 plans contenant chacun la normale au quad et une arete AB/BC/CD/DA ?
	// si oui il est dans le quad


    // normal of quad: cross product of the 2 diago

    Vec3 centroide((A.x+B.x+C.x+D.x)/4, (A.y+B.y+C.y+D.y)/4, (A.z+B.z+C.z+D.z)/4);

    Vec3 normal(centroide, normal_of(centroide, A, B), normal_of(centroide, C, D));

    Vec3 AB = vecOf(A, B);
    Vec3 BC = vecOf(B, C);
    Vec3 CD = vecOf(C, D);
    Vec3 DA = vecOf(D, A);


    return true;
}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
	// recuperation des indices de points
	// recuperation des points

	// calcul de l'equation du plan (N+d)

    // calcul de l'intersection rayon plan
	// I = P + alpha*Dir est dans le plan => calcul de alpha

	// alpha => calcul de I

	// I dans le quad ?

    return false;
}


int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
	// on parcours tous les quads
	// on teste si il y a intersection avec le rayon
	// on garde le plus proche (de P)

	int inter = -1;

	return inter;
}


Mat4 MeshQuad::local_frame(int q)
{
	// Repere locale = Matrice de transfo avec
	// les trois premieres colones: X,Y,Z locaux
	// la derniere colonne l'origine du repere

	// ici Z = N et X = AB
	// Origine le centre de la face
	// longueur des axes : [AB]/2

	// recuperation des indices de points
	// recuperation des points

	// calcul de Z:N / X:AB -> Y

	// calcul du centre

	// calcul de la taille

	// calcul de la matrice

    return Mat4();
}

void MeshQuad::extrude_quad(int q)
{
	// recuperation des indices de points

	// recuperation des points

	// calcul de la normale

	// calcul de la hauteur

	// calcul et ajout des 4 nouveaux points

	// on remplace le quad initial par le quad du dessu

	// on ajoute les 4 quads des cotes

   gl_update();
}

void MeshQuad::transfo_quad(int q, const glm::mat4& tr)
{
	// recuperation des indices de points
	// recuperation des (references de) points

	// generation de la matrice de transfo globale:
	// indice utilisation de glm::inverse() et de local_frame

	// Application au 4 points du quad
}

void MeshQuad::decale_quad(int q, float d)
{

}

void MeshQuad::shrink_quad(int q, float s)
{

}

void MeshQuad::tourne_quad(int q, float a)
{

}





MeshQuad::MeshQuad():
	m_nb_ind_edges(0)
{}


void MeshQuad::gl_init()
{
	m_shader_flat = new ShaderProgramFlat();
	m_shader_color = new ShaderProgramColor();

	//VBO
	glGenBuffers(1, &m_vbo);

	//VAO
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_flat->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_flat->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(m_shader_color->idOfVertexAttribute);
	glVertexAttribPointer(m_shader_color->idOfVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	//EBO indices
	glGenBuffers(1, &m_ebo);
	glGenBuffers(1, &m_ebo2);
}

void MeshQuad::gl_update()
{
	//VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * m_points.size() * sizeof(GLfloat), &(m_points[0][0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::vector<int> tri_indices;
	convert_quads_to_tris(m_quad_indices,tri_indices);

	//EBO indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,tri_indices.size() * sizeof(int), &(tri_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::vector<int> edge_indices;
	convert_quads_to_edges(m_quad_indices,edge_indices);
	m_nb_ind_edges = edge_indices.size();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_nb_ind_edges * sizeof(int), &(edge_indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void MeshQuad::set_matrices(const Mat4& view, const Mat4& projection)
{
	viewMatrix = view;
	projectionMatrix = projection;
}

void MeshQuad::draw(const Vec3& color)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	m_shader_flat->startUseProgram();
	m_shader_flat->sendViewMatrix(viewMatrix);
	m_shader_flat->sendProjectionMatrix(projectionMatrix);
	glUniform3fv(m_shader_flat->idOfColorUniform, 1, glm::value_ptr(color));
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
	glDrawElements(GL_TRIANGLES, 3*m_quad_indices.size()/2,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_flat->stopUseProgram();

	glDisable(GL_POLYGON_OFFSET_FILL);

	m_shader_color->startUseProgram();
	m_shader_color->sendViewMatrix(viewMatrix);
	m_shader_color->sendProjectionMatrix(projectionMatrix);
	glUniform3f(m_shader_color->idOfColorUniform, 0.0f,0.0f,0.0f);
	glBindVertexArray(m_vao2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo2);
	glDrawElements(GL_LINES, m_nb_ind_edges,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	m_shader_color->stopUseProgram();
	glDisable(GL_CULL_FACE);
}

