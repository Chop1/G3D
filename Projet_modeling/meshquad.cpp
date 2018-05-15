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

    Vec3 centre(0,0,0);

    // parcour des points
    size_t pointSize = m_points.size();
    for(unsigned int i = 0; i < pointSize; i++)
    {
        centre = Vec3(centre.x+m_points[i].x, centre.y+m_points[i].y, centre.z+m_points[i].z);
    }
    C = Vec3(centre.x/pointSize, centre.y/pointSize, centre.z/pointSize);

	// R=
    // rayon entre centre et point le plus eloigné
    double max = 0;
    for(unsigned int i = 0; i < pointSize; i++)
    {
        double tmp = std::abs(norm(vecOf(C, m_points[i])));
        if(tmp > max)
            max = tmp;
    }

    R = max;
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

double MeshQuad::dot(const Vec3& A, const Vec3& B)
{
    return B.x*A.x + B.y*A.y + B.z*A.z;
}

Vec3 MeshQuad::vecOf(const Vec3& A, const Vec3& B)
{
    return Vec3(B.x-A.x, B.y-A.y, B.z-A.z);
}

Vec3 MeshQuad::vMult(const Vec3& A, const Vec3& B)
{
    return Vec3(B.x*A.x, B.y*A.y, B.z*A.z);
}

Vec3 MeshQuad::multScal(const Vec3& A, const double& B)
{
    return Vec3(B*A.x, B*A.y, B*A.z);
}

/*
// const, a, b on elimine b
Vec3 MeshQuad::param2Cartesian(const Vec3& AX, const Vec3& AY, const Vec3& AZ)
{
    Vec3 AXtmp = (AX.x * AY.z, AX.y * AY.z, AX.z * AY.z);
    int X = AY.z;
    Vec3 AYtmp = (AY.x * AX.z, AY.y * AX.z, AY.z * AX.z);
    int Y = AX.z;

    Vec3 resTmp;

    if((AXtmp.z > 0 && AYtmp.z < 0) || (AXtmp.z < 0 && AYtmp.z > 0)) // signe dif
    {
        resTmp = (AXtmp.x - AYtmp.x, AXtmp.y - AYtmp.y, AXtmp.z + AYtmp.z);
    }
    else // meme signe
    {
        resTmp = (AXtmp.x - AYtmp.x, AXtmp.y - AYtmp.y, AXtmp.z - AYtmp.z);
    }

    AYtmp = (AY.x * AZ.z, AY.y * AZ.z, AY.z * AZ.z);
    int Y2 = AZ.z;
    Vec3 AZtmp = (AZ.x * AY.z, AZ.y * AY.z, AZ.z * AY.z);
    int X2 = AY.z;

    var res2Tmp;

    if((AYtmp.z > 0 && AZtmp.z < 0) || (AYtmp.z < 0 && AZtmp.z > 0)) // signe dif
    {
        res2Tmp = (AYtmp.x - AZtmp.x, AYtmp.y - AZtmp.y, AYtmp.z + AZtmp.z);
    }
    else // meme signe
    {
        res2Tmp = (AYtmp.x - AZtmp.x, AYtmp.y - AZtmp.y, AYtmp.z - AZtmp.z);
    }

    return Vec3(resTmp.x - res2Tmp.x, X-X2, Y-Y2);
    // prob faut renvoyer 4 elements (Mat4 ?)
}
*/

bool MeshQuad::is_points_in_tri(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C)
{// todo a revoir

    // Compute vectors
    Vec3 v0 = vecOf(A, C); // AC
    Vec3 v1 = vecOf(A, B); // AB
    Vec3 v2 = vecOf(A, P);  // AP

    // Compute dot products
    double dot00 = dot(v0, v0);
    double dot01 = dot(v0, v1);
    double dot02 = dot(v0, v2);
    double dot11 = dot(v1, v1);
    double dot12 = dot(v1, v2);

    // Compute barycentric coordinates
    double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u > 0) && (v > 0) && (u + v < 1);
}

Vec3 MeshQuad::addVec(const Vec3& P, const Vec3& V)
{
    return Vec3 (P.x + V.x, P.y + V.y, P.z + V.z);
}

// return 0 si =0, 1 si >0, -1 si <0
int MeshQuad::resousEq(const Vec3& Point, const Vec3& Normal, double& d)
{
     double res = Normal.x * Point.x + Normal.y * Point.y + Normal.z * Point.z + d;

     if(res == 0)
         return 0;
     return (res > 0)? 1 : -1;
}

bool MeshQuad::is_points_in_quad(const Vec3& P, const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D)
{
    // On sait que P est dans le plan du quad.

    // P est-il au dessus des 4 plans contenant chacun la normale au quad et une arete AB/BC/CD/DA ?
	// si oui il est dans le quad  

    Vec3 AB = vecOf(A, B);
    Vec3 BC = vecOf(B, C);
    Vec3 CD = vecOf(C, D);
    Vec3 DA = vecOf(D, A);

    // calcul de l'equation du plan (N+d) ABCD
    Vec3 N = normal_of(A, B, C);
    double d = - (N.x*A.x + N.y*A.y + N.z*A.z);

    // plan normal / AB
    Vec3 normalAB = normal_of(A, B, addVec(A, N));
    double dAB = - (normalAB.x*A.x + normalAB.y*A.y + normalAB.z*A.z);

    bool res1 = false;

    if(resousEq(P, normalAB, dAB) == -1)
    {
        res1 = true;
        //std::cout << "1dessus" << std::endl;
    }


    // plan normal / BC
    Vec3 normalBC = normal_of(B, C, addVec(B, N));
    double dBC = - (normalBC.x*B.x + normalBC.y*B.y + normalBC.z*B.z);

    bool res2 = false;

    if(resousEq(P, normalBC, dBC) == -1)
    {
        res2 = true;
//        std::cout << "2dessus" << std::endl;
    }

    // plan normal / CD
    Vec3 normalCD = normal_of(C, D, addVec(C, N));
    double dCD = - (normalCD.x*C.x + normalCD.y*C.y + normalCD.z*C.z);

    bool res3 = false;

    if(resousEq(P, normalCD, dCD) == -1)
    {
        res3 = true;
//        std::cout << "3dessus" << std::endl;
    }

    // plan normal / DA
    Vec3 normalDA = normal_of(D, A, addVec(D, N));
    double dDA = - (normalDA.x*D.x + normalDA.y*D.y + normalDA.z*D.z);

    bool res4 = false;

    if(resousEq(P, normalDA, dDA) == -1)
    {
        res4 = true;
//        std::cout << "4dessus" << std::endl;
    }





    return res1 && res2 && res3 && res4;


/*
    if (is_points_in_tri(P, A, B, C) || is_points_in_tri(P, D, B, C))
        std::cout << "P " << P.y << ", " << P.y << ", " << P.z << " dedans" << std::endl;
    else
        std::cout << "P " << P.y << ", " << P.y << ", " << P.z << " pas dedans" << std::endl;
    return is_points_in_tri(P, A, B, C) || is_points_in_tri(P, D, B, C);
*/
/*
    Vec3 v0 = vecOf(A, B); // AB
    Vec3 v1 = vecOf(A, D); // AD

    bool u = ((dot(A, v0) <= dot(P, v0)) && (dot(P, v0) <= dot(B, v0 )));

    bool v = ((dot(A, v1) <= dot(P, v1)) && (dot(P, v1) <= dot(D, v1 )));

       if(u && v)
           std::cout << "dedans" << std::endl;
    return u && v;
    */
}

bool MeshQuad::intersect_ray_quad(const Vec3& P, const Vec3& Dir, int q, Vec3& inter)
{
	// recuperation des indices de points

    int a = q * 4;

    int ind1 = m_quad_indices[a];
    int ind2 = m_quad_indices[a+1];
    int ind3 = m_quad_indices[a+2];
    int ind4 = m_quad_indices[a+3];

	// recuperation des points

    Vec3 p1 = m_points[ind1];
    Vec3 p2 = m_points[ind2];
    Vec3 p3 = m_points[ind3];
    Vec3 p4 = m_points[ind4];

    if(!is_points_in_quad(P, p1, p2, p3, p4))
        return false;

    std::cout << "quad " << q << " contient le point" << std::endl;

	// calcul de l'equation du plan (N+d)
    Vec3 normal = normal_of(p1, p2, p3);
    double d = - (normal.x*p4.x + normal.y*p4.y + normal.z*p4.z);

    // calcul de l'intersection rayon plan
	// I = P + alpha*Dir est dans le plan => calcul de alpha

    double denom = dot(normal, Dir);
    if (std::abs(denom) > 0.0001f) // un epsilon (0 serait parallel au plan)
    {
        double t = dot(vecOf(p1, P), normal) / denom;

        // alpha => calcul de I // mettre dans inter
        inter = Vec3(P.x + t * Dir.x, P.y + t * Dir.y, P.z + t * Dir.z);

        std::cout << "intersection en " << inter.x << ", " << inter.y << ", " << inter.z
                  << "\nt: " << t << std::endl;
        return true;
    }



    // I dans le quad ? // is point in quad

    return false;
}

double MeshQuad::norm(Vec3 const& u)
{
    return std::sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
}

Vec3 MeshQuad::normalise(Vec3 const& u)
{
    double norme = norm(u);
    return Vec3(u.x / norme, u.y / norme, u.z / norme);
}

// ça ne semble marché que quand le cube est bien de face
int MeshQuad::intersected_closest(const Vec3& P, const Vec3& Dir)
{
	// on parcours tous les quads
	// on teste si il y a intersection avec le rayon
	// on garde le plus proche (de P)

	int inter = -1;
    Vec3 pInter;
    Vec3 pInterMem;

    // parcour des quads
    size_t quadSize = m_quad_indices.size();
    std::cout << "==================\nnbr quad: " << quadSize/4 << std::endl;
    for(unsigned int i = 0; i < quadSize; i+=4)
    {
//        std::cout << "test " << i << std::endl;
//        std::cout << "inter: " << inter << std::endl;
        if(((inter == -1) && intersect_ray_quad(P, Dir, i/4, pInter))
            || ((inter != -1) && intersect_ray_quad(P, Dir, i/4, pInter) && std::abs(norm(vecOf(P, pInter))) < std::abs(norm(vecOf(P, pInterMem)))))// abs t >=0
        {
            inter = i/4;
            pInterMem = pInter;
        }

    }
    return inter;
}


Mat4 MeshQuad::local_frame(int q)
{
	// Repere locale = Matrice de transfo avec
	// les trois premieres colones: X,Y,Z locaux
	// la derniere colonne l'origine du repere

    // ici Z = N // la normal
    // et X = AB // première arrete du quad
	// Origine le centre de la face
	// longueur des axes : [AB]/2

	// recuperation des indices de points

    int a = q * 4;

    int ind1 = m_quad_indices[a];
    int ind2 = m_quad_indices[a+1];
    int ind3 = m_quad_indices[a+2];
    int ind4 = m_quad_indices[a+3];

	// recuperation des points

    Vec3 p1 = m_points[ind1];
    Vec3 p2 = m_points[ind2];
    Vec3 p3 = m_points[ind3];
    Vec3 p4 = m_points[ind4];

    // calcul de Z:N

    Vec3 Z = normal_of(p1, p2, p3);

    // X:AB

    Vec3 X = vecOf(p1, p2);

    // Y:AC

    Vec3 Y = vecOf(p1, p3);

	// calcul du centre

    Vec3 centre(0.0, 0.0, 0.0);
    for(unsigned int i = 0; i < 4; i++)
    {
        centre = Vec3(centre.x+m_points[a+i].x, centre.y+m_points[a+i].y, centre.z+m_points[a+i].z);
    }
    centre = Vec3(centre.x/4, centre.y/4, centre.z/4);

	// calcul de la taille

    double taille = norm(X)/2;

	// calcul de la matrice

    double xRapport = norm(addVec(centre, X))/taille;
    X = vMult(X, Vec3(xRapport, xRapport, xRapport));

    double yRapport = norm(addVec(centre, Y))/taille;
    Y = vMult(Y, Vec3(yRapport, yRapport, yRapport));

    double zRapport = norm(addVec(centre, Z))/taille;
    Z = vMult(Z, Vec3(zRapport, zRapport, zRapport));

    Mat4 res(Vec4(X, 0), Vec4(Y,0), Vec4(Z,0), Vec4(centre,1));

    return res;
}

void MeshQuad::extrude_quad(int q)
{
	// recuperation des indices de points

    int a = q * 4;

    int ind1 = m_quad_indices[a];
    int ind2 = m_quad_indices[a+1];
    int ind3 = m_quad_indices[a+2];
    int ind4 = m_quad_indices[a+3];

	// recuperation des points

    Vec3 p1 = m_points[ind1];
    Vec3 p2 = m_points[ind2];
    Vec3 p3 = m_points[ind3];
    Vec3 p4 = m_points[ind4];

	// calcul de la normale

    Vec3 N = normal_of(p1, p2, p3);

    // calcul de la hauteur // hauteur ? hauteur d'un triangle ??
    // "distance proportionnelle à la racine carré de son aire"

    double aire = norm(vecOf(p1, p2))*norm(vecOf(p1, p3));
    //double distance = std::sqrt(aire);
    double distance = 1;

	// calcul et ajout des 4 nouveaux points
    double p1rapport = norm(addVec(p1, N))/distance;
    double p2rapport = norm(addVec(p2, N))/distance;
    double p3rapport = norm(addVec(p3, N))/distance;
    double p4rapport = norm(addVec(p4, N))/distance;


//    int ip1 = add_vertex(vMult(p1, Vec3(p1rapport, p1rapport, p1rapport)));
//    int ip2 = add_vertex(vMult(p2, Vec3(p2rapport, p2rapport, p2rapport)));
//    int ip3 = add_vertex(vMult(p3, Vec3(p3rapport, p3rapport, p3rapport)));
//    int ip4 = add_vertex(vMult(p4, Vec3(p4rapport, p4rapport, p4rapport)));

    int ip1 = add_vertex(addVec(p1, multScal(N, 2)));
    int ip2 = add_vertex(addVec(p2, N));
    int ip3 = add_vertex(addVec(p3, N));
    int ip4 = add_vertex(addVec(p4, N));

    // on remplace le quad initial par le quad du dessu

    m_quad_indices[a] = ip1;
    m_quad_indices[a+1] = ip2;
    m_quad_indices[a+2] = ip3;
    m_quad_indices[a+3] = ip4;

	// on ajoute les 4 quads des cotes
    // SENS TRIGO

//    add_quad(ind2, ip2, ip3, ind3);
//    add_quad(ind3, ip3, ip4, ind4);
//    add_quad(ind4, ip4, ip1, ind1);
//    add_quad(ind1, ip1, ip2, ind2);

    add_quad(ip2, ind2, ind3, ip3);
    add_quad(ip3, ind3, ind4, ip4);
    add_quad(ip4, ind4, ind1, ip1);
    add_quad(ip1, ind1, ind2, ip2);

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

